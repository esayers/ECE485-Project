#include "ece485-cache.h"

class CacheController
{


	Cache *MainCache;

public:
	CacheController(int, int, int);
	~CacheController();

	void PerformCacheOp(int, unsigned int);
	void PrintStats();
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
	void BusOperation(busOperationType, unsigned int);
	void PutSnoopResult(snoopOperationType, unsigned int);
	void MessageToL2Cache(busOperationType, unsigned int);
};