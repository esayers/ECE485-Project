#include "cacheController.h"
using namespace std;

CacheController::CacheController(int associativity, int totalSizeBytes, int lineSizeBytes)
{
	MainCache = new Cache(totalSizeBytes, lineSizeBytes, associativity);


}

CacheController::~CacheController()
{
	
	delete MainCache;


}

//Performs a cache operation
void CacheController::PerformCacheOp(int traceOp, unsigned int address)
{
	switch (traceOp)
	{
	case 0:
	case 2:
		ReadRequestFromL1Cache(address);
		break;
	case 1:
		WriteRequestFromL1Cache(address);
		break;
	case 3:
		SnoopInvalidate(address);
		break;
	case 4:
		SnoopRead(address);
		break;
	case 5:
		SnoopWrite(address);
		break;
	case 6:
		SnoopRwo(address);
		break;
	}
}

//Prints the statistics of the cache
void CacheController::PrintStats()
{
	int hits = MainCache->CacheHits;
	int misses = MainCache->CacheMisses;
	int reads = MainCache->CacheReads;
	int writes = MainCache->CacheReads;
	printf("Cache Hits: %d\n", hits);
	printf("Cache Misses: %d\n", misses);
	if (hits + misses)
		printf("Cache Hit Ratio: %.2f%%\n", ((float) hits * 100) / ((float) (hits + misses)));
	printf("Cache Reads: %d\n",  reads);
	printf("Cache Writes: %d\n", writes);
}

void CacheController::PrintCache()
{
	printf("\n\nDumping the cache...\n\n");
	
	for (unsigned int i = 0; i < MainCache->num_sets; i++)
	{
		if (MainCache->sets[i] != NULL)
		{
			for (unsigned int j = 0; j < MainCache->assoc; j++)
			{

				if (MainCache->sets[i]->lines[j] != NULL)
				{
					printf("Set: %d		Line: %d	Tag: %#o		MESIF:%d\n", i, j, MainCache->sets[i]->lines[j]->Tag, MainCache->sets[i]->lines[j]->State);
				}
			}
		}
	}

	printf("\n\n===========================================\n\n");
	
}

//Clears the cache
void CacheController::ClearCache()
{
	int associativity;
	int lineSizeBytes;
	int totalSizeBytes;

	associativity = MainCache->assoc;
	lineSizeBytes = MainCache->LineSizeBytes;
	totalSizeBytes = MainCache->total_size;
	delete MainCache;

	MainCache = new Cache(totalSizeBytes, lineSizeBytes, associativity);
	
}

//Handles a read request from the L1 cache. 
void CacheController::ReadRequestFromL1Cache(unsigned int address)
{
	Cache_line* CacheRslt = MainCache->LookupCacheLine(address);

	//If no line was returned, or if the line was returned and is marked invalid, read from the higher cache
	if (CacheRslt == NULL || (CacheRslt != NULL && CacheRslt->State == MESIF_INVALID))
	{
		if (ReadfromL2Cache(address,false))
		{
			if (MainCache->PlaceLineInCache(address, MESIF_FORWARD))
				BusOperation(WRITE, address, GetSnoopResult(address));

		}
		else
			if (MainCache->PlaceLineInCache(address, MESIF_EXCLUSIVE))
			{
				BusOperation(WRITE, address, GetSnoopResult(address));
			}

	}
	
}

//Handles a write request from the L1 cache
void CacheController::WriteRequestFromL1Cache(unsigned int address)
{
	
	Cache_line* LineRslt = MainCache->LookupCacheLine(address);

	if (LineRslt == NULL)
	{
		ReadfromL2Cache(address, true);
		if(MainCache->PlaceLineInCache(address, MESIF_MODIFIED))
			BusOperation(WRITE, address, GetSnoopResult(address));

	}
	else
	{
		BusOperation(INVALIDATE, address, GetSnoopResult(address));
		LineRslt->State = MESIF_MODIFIED;
	}


}

//Handles a read from the higher cache. Returns true if snoop was successful, false if not
	bool CacheController::ReadfromL2Cache(unsigned int address, bool Rwim)
	{
		snoopOperationType SnoopRslt = GetSnoopResult(address);

		if (SnoopRslt == NOHIT)
		{
			BusOperation(Rwim?RWIM:READ, address,SnoopRslt);
			return false;
		}

		return true;

	
	}

	//Handles a snooped invalidate command
	void CacheController::SnoopInvalidate(unsigned int address)
	{
		Cache_line* lineRslt = MainCache->LookupCacheLine(address);

		if (lineRslt != NULL)
		{
			lineRslt->State = MESIF_INVALID;
		}

	}

	void CacheController::SnoopRead(unsigned int address)
	{
		Cache_line* lineRslt = MainCache->LookupCacheLine(address);

		if (lineRslt == NULL)

			PutSnoopResult(NOHIT, address);
		else
		{
			switch (lineRslt->State)
			{
			case MESIF_INVALID:
			
				PutSnoopResult(NOHIT, address);
				break;
			
			case MESIF_EXCLUSIVE:
			{
				PutSnoopResult(HIT, address);
				lineRslt->State = MESIF_SHARED;
				
			}
			break;
			case MESIF_FORWARD:
			{
				PutSnoopResult(HIT, address);
				lineRslt->State = MESIF_SHARED;
				
			}
			break;
			case MESIF_MODIFIED:
			{
				BusOperation(WRITE, address, GetSnoopResult(address));
				PutSnoopResult(HITM, address);
				lineRslt->State = MESIF_SHARED;
				
			}
			break;
			default:
				break;

			}

		}
	
	}

	//Handles a snooped write command - in this case, looks up the cache line and 
	//sees if it exists, if so, it changes it to the "shared" state
	void CacheController::SnoopWrite(unsigned int address )
	{
		Cache_line* lineRslt = MainCache->LookupCacheLine(address);

		if (&lineRslt != NULL)
		{
			lineRslt->State = MESIF_SHARED;
		}
	}

	void CacheController::SnoopRwo(unsigned int address)
	{
		Cache_line* lineRslt = MainCache->LookupCacheLine(address);

		if (lineRslt == NULL)

			PutSnoopResult(NOHIT, address);
		else
		{
			switch (lineRslt->State)
			{
			case MESIF_INVALID:
				PutSnoopResult(NOHIT, address);
				break;
			case MESIF_EXCLUSIVE:
			case MESIF_FORWARD:
			case MESIF_SHARED:
			{
				PutSnoopResult(HIT, address);
				lineRslt->State = MESIF_INVALID;
			}
				break;
			case MESIF_MODIFIED:
			{
				BusOperation(WRITE, address, GetSnoopResult(address));
				PutSnoopResult(HITM, address);
				lineRslt->State = MESIF_INVALID;
			}
				break;
			default:
				break;

			}

		}
	}

	//Required functions
	//Since there are three possible results, we're getting the remainder of dividing the address by 3 and using that to determine what result we return

	snoopOperationType CacheController::GetSnoopResult(unsigned int address)
	{
		
	
		int ModRslt = AddressUtils::GetByteSelect(MainCache->ByteSelectLength,address) % 3;

		switch (ModRslt)
		{
		case 0:
		default:
			return NOHIT;
			break;
		case 1:
			return HIT;
			break;
		case 2:
			return HITM;
			break;
		}
	}

	void CacheController::BusOperation(busOperationType busOp, unsigned int address, snoopOperationType SnoopResult)
	{
#ifndef SILENT
			//printf("BusOp: %d, Address: %#o, Snoop Result: %d\n",busOp,address, SnoopResult);
		cout << "BusOp: " << busOp << ", Address: 0x" << hex << address << ", Snoop Result: " << SnoopResult << endl;
#endif
	}
	void CacheController::PutSnoopResult(snoopOperationType busOp, unsigned int address)
	{
#ifndef SILENT
			printf("SnoopResult: Address: %#o, SnoopResult: %d\n", address, busOp);
#endif
	}
	void CacheController::MessageToL2Cache(busOperationType busOp, unsigned int address)
	{
#ifndef SILENT
		printf("MessageToL2Cache: BusOp: %d, Address: %#o\n", busOp, address);
#endif
	}