// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "cacheController.h"

using namespace std;


void RunTestFile(string testFile, CacheController* mainController)
{
	ifstream traceFile(testFile);
	if (traceFile.is_open())
	{

		std::string line;


		while (getline(traceFile, line))
		{
			unsigned int address = 0;
			int traceOp;
			string traceOpStr = line.substr(0, 1);
			string addressStr = line.substr(2);
			stringstream(traceOpStr) >> traceOp;
			sscanf(addressStr.c_str(), "%x", &address);




			switch (traceOp)
			{

			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				mainController->PerformCacheOp(traceOp, address);
				break;
			case 8:
				mainController->ClearCache();
				break;
			case 9:
				mainController->PrintCache();
				break;
				// Invalid Operation
			default:
				break;
			}


		}

		traceFile.close();
	}
}

int main(int argc, char *argv[])
{
	//Array sizes
	int associativity = 16;
	int lineSizeBytes = 64;
	//int totalSizeBytes = 16777216;
	int totalSizeBytes = 8388608;

	
	//initialize arrays
	CacheController MainCacheController(associativity, totalSizeBytes, lineSizeBytes);

	ifstream testFilePaths("C:\\Traces\\TestFileList.txt");
	if (testFilePaths.is_open())
	{

		std::string line;


		while (getline(testFilePaths, line))
		{
			
			RunTestFile(line, &MainCacheController);


		}

		testFilePaths.close();
	}

	//Open trace file and read a line
	

	//Print summary
	MainCacheController.PrintStats();

	return 0;
}
