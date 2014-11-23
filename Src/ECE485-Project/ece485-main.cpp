// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "cacheController.h"

using namespace std;
int main(int argc, char *argv[])
{
	//Array sizes
	int associativity = 6;
	int lineSizeBytes = 64;
	int totalSizeBytes = 8192;

	
	//initialize arrays
	CacheController MainCacheController(associativity, lineSizeBytes, totalSizeBytes);

	//Open trace file and read a line
	

	ifstream traceFile("Tracefile.tc");
	if (traceFile.is_open())
	{
		
		std::string line;


		while (getline(traceFile,line))
		{
			unsigned int address;
			int traceOp;
			stringstream(line.substr(1, 1)) >> traceOp;
			stringstream(line.substr(3)) >> address;



			switch (traceOp)
			{
			case 8:
				MainCacheController.ClearCache();
			case 9:
				MainCacheController.PrintStats();
			default:
				MainCacheController.PerformCacheOp(traceOp, address);
			}


		}
	}

	//Print summary

	//Cleanup

	return 0;
}
