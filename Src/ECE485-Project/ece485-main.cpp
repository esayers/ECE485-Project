// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "cacheController.h"
#define SILENT

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
			int traceOp = -1;

			if (line.length() >= 3)
			{
				string traceOpStr = line.substr(0, 1);
				string addressStr = line.substr(2);
				traceOp = stoi(traceOpStr, nullptr, 10); // Convert trace to integer
				if (traceOp != 9 && traceOp != 8)
				address = stoi(addressStr, nullptr, 16); // Convert address to integer, treating as hex
			}
			else if (line.length() >= 1)
			{
				string traceOpStr = line.substr(0, 1);
				traceOp = stoi(traceOpStr, nullptr, 10); // Convert trace to integer
			}

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
	int associativity = 13;
	int lineSizeBytes = 64;
	//int totalSizeBytes = 16777216;
	int totalSizeBytes = 8388608;

	
	//initialize arrays
	CacheController MainCacheController(associativity, totalSizeBytes, lineSizeBytes);

	ifstream testFilePaths("TestFileList.txt");
	if (testFilePaths.is_open())
	{
		
		std::string line;


		while (getline(testFilePaths, line))
		{
			cout << "Running file: " << line;
			cout << "\n";
			RunTestFile(line, &MainCacheController);


		}

		testFilePaths.close();
	}

	//Open trace file and read a line
	

	//Print summary
	MainCacheController.PrintStats();
	system("pause");
	return 0;
}
