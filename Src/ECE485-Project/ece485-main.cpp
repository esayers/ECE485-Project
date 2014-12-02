// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "cacheController.h"

using namespace std;
int main(int argc, char *argv[])
{
	//Array sizes
	int associativity = 16;
	int lineSizeBytes = 64;
	//int totalSizeBytes = 16777216;
	int totalSizeBytes = 8388608;

	
	//initialize arrays
	CacheController MainCacheController(associativity, totalSizeBytes, lineSizeBytes);

	//Open trace file and read a line
	

	ifstream traceFile("C:\\Traces\\cc1.din");
	if (traceFile.is_open())
	{
		
		std::string line;


		while (getline(traceFile,line))
		{
			unsigned int address;
			int traceOp;
			string traceOpStr = line.substr(0, 1);
			string addressStr = line.substr(2);
			stringstream(traceOpStr) >> traceOp;
			stringstream(addressStr) >> address;



			switch (traceOp)
			{

			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				MainCacheController.PerformCacheOp(traceOp, address);
				break;
			case 8:
				MainCacheController.ClearCache();
				break;
			case 9:
				MainCacheController.PrintCache();
				break;
			// Invalid Operation
			default:
				break;
			}


		}

		traceFile.close();
	}

	//Print summary
	MainCacheController.PrintStats();

	return 0;
}
