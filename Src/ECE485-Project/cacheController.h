// ECE 485 Cache Project
// Edward Sayers
// Nathan Becker

#ifndef CACHE_CONTROLLER_H
#define CACHE_CONTROLLER_H

#include "ece485-cache.h"
#include <iomanip>

class CacheController
{


	Cache *MainCache;

public:
	CacheController(int, int, int);
	~CacheController();

	void PerformCacheOp(int, unsigned int);
	void PrintStats();
	void PrintCache();
	void ClearCache();
	
private:
	void ReadRequestFromL1Cache(unsigned int);
	void WriteRequestFromL1Cache(unsigned int);
	bool ReadfromL2Cache(unsigned int, bool Rwim);
	void SnoopInvalidate(unsigned int);
	void SnoopRead(unsigned int);
	void SnoopWrite(unsigned int);
	void SnoopRwo(unsigned int);

	//Required functions
	snoopOperationType GetSnoopResult(unsigned int);
	void BusOperation(busOperationType, unsigned int, snoopOperationType);
	void PutSnoopResult(snoopOperationType, unsigned int);
	void MessageToL2Cache(busOperationType, unsigned int);
};

#endif