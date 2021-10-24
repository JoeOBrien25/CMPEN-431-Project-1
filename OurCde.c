#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (964036807+976939902)
//14
/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig) {

	string latencySettings;
	string latil1
	string latdl1
	string latul2
	//
	//YOUR CODE BEGINS HERE
	//
	// define lat of il1
	if (il1 == 2KB)
		latil1 = "1";

	if (il1 == 4KB)
		latil1 = "2";

	if (il1 == 8KB)
		latil1 = "3";

	if (il1 == 16KB)
		latil1 = "4";

	if (il1 == 32KB)
		latil1 = "5";

	if (il1 == 64KB)
		latil1 = "6";


	//define dl1 lat 
	if (dl1 == 2KB)
		latdl1 = "1";

	if (dl1 == 4KB)
		latdl1 = "2";

	if (dl1 == 8KB)
		latdl1 = "3";

	if (dl1 == 16KB)
		latdl1 = "4";

	if (dl1 == 32KB)
		latdl1 = "5";

	if (dl1 == 64KB)
		latdl1 = "6";


	//ul2 lat set up
	if (ul2 == 32KB)
		latul2 = "5";

	if (ul2 == 64KB)
		latul2 = "6";

	if (ul2 == 128KB)
		latul2 = "7";

	if (ul2 == 256KB)
		latul2 = "8";

	if (ul2 == 512KB)
		latul2 = "9";

	if (ul2 == 1024KB)
		latul2 = "10";

	// This is a dumb implementation.
	latencySettings = "1 1 1";


	latencySettings[0] = latil1;
	latencySettings[2] = latdl1;
	latencySettings[4] = latul2;

	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings;
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {
	// 1- 4 
	// FIXME - YOUR CODE HERE
	if (ill <= ifq)
		return 0;

	if (ill != dll)
		return 0;

	if (ul2 < (2 * (ill + dll)) )
		return 0;

	if (ul2 > 128B)
		return 0;

	if (ill > 64KB)
		return 0;

	if (ill < 2KB)
		return 0;

	if (ul2 < 32KB)
		return 0;

	if (ul2 > 1MB)
		return 0;

	
	// The below is a necessary, but insufficient condition for validating a
	// configuration.
	return isNumDimConfiguration(configuration);
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
		std::string bestEXECconfiguration, std::string bestEDPconfiguration,
		int optimizeforEXEC, int optimizeforEDP) {
			

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality - how many changeable dimensions are in each dimension. 
	// 2. GLOB_baseline - the first run block 
	// 3. NUM_DIMS - number of dimensions that can be changed 
	// 4. NUM_DIMS_DEPENDENT - dimensions that are changed based on other valus
	// 5. GLOB_seen_configurations
	
	//core 1st (0 and 1)
	//cache (2 - 10) 
	//BP (12 -14)
	//FPU (11)

	std::string nextconfiguration = currentconfiguration;
	// Continue if proposed configuration is invalid or has been seen/checked before.
	while (!validateConfiguration(nextconfiguration) ||
		GLOB_seen_configurations[nextconfiguration]) {

		// Check if DSE has been completed before and return current
		// configuration.
		if(isDSEComplete) {
			return currentconfiguration;
		}

		std::stringstream ss;

		string bestConfig;
		if (optimizeforEXEC == 1)
			bestConfig = bestEXECconfiguration;

		if (optimizeforEDP == 1)
			bestConfig = bestEDPconfiguration;

		// Fill in the dimensions already-scanned with the already-selected best
		// value.
		for (int dim = 0; dim < currentlyExploringDim; ++dim) {
			ss << extractConfigPararm(bestConfig, dim) << " ";
		}

		// Handling for currently exploring dimension. This is a very dumb
		// implementation.
		int nextValue = extractConfigPararm(nextconfiguration, currentlyExploringDim) + 1;

		if (nextValue >= GLOB_dimensioncardinality[currentlyExploringDim]) {
			nextValue = extractConfigPararm(bestConfig, currentlyExploringDim)	//GLOB_dimensioncardinality[currentlyExploringDim] - 1;
			currentDimDone = true;
		}

		ss << nextValue << " ";

		for (int dim = currentlyExploringDim-1; dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim) {
			ss << extractConfigPararm(bestConfig, dim) << " ";
		}

		// // Fill in remaining independent params with 0.
		// for (int dim = (currentlyExploringDim + 1);
		// 		dim < (NUM_DIMS - NUM_DIMS_DEPENDENT); ++dim) {
		// 	ss << "0 ";
		// }

		//
		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = ss.str();

		// Populate this object using corresponding parameters from config.
		ss << generateCacheLatencyParams(configSoFar);

		// Configuration is ready now.
		nextconfiguration = ss.str();

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone) {
			if (currentlyExploringDim == 10){
				currentlyExploringDim = 12;
			}
			else if (currentlyExploringDim == 14){
				currentlyExploringDim = 11;
			}
			else {
				currentlyExploringDim += 1;
			}
			currentDimDone = false;
		}

		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == 11 && currentDimDone == true){
			isDSEComplete = true;
		}
	}
	return nextconfiguration;
}

