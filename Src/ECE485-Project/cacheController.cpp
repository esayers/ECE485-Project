#include "cacheController.h"
#define SILENT
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
	cout << "Cache Hits : " << dec << hits << endl;
	cout << "Cache Misses: " << misses << endl;
	if (hits + misses)
		cout << "Cache Hit Ratio: " << setprecision(2) << fixed << (hits * 100.0) / (hits + misses) << "%" << endl;
	cout << "Cache Reads: " << reads << endl;
	cout << "Cache Writes: " << writes << endl;
}

void CacheController::PrintCache()
{
	cout << endl;
	cout << "  Cache Contents" << endl;
	cout << "===========================================" << endl;
	for (unsigned int i = 0; i < MainCache->num_sets; i++)
	{
		Cache_set* curSet = MainCache->sets[i]; // Set at index i
		if (curSet != NULL)
		{
			cout << "  Set: 0x" << setw(5) << setfill('0') << hex << i << ", LRU Bits: ";

			// For each bool in array print a one or zero
			for (unsigned int j = 0; j < (MainCache->assoc - 1); ++j)
			{
				if (curSet->lru_state[j])
					cout << "1";
				else
					cout << "0";
			}
			cout << endl;

			for (unsigned int j = 0; j < MainCache->assoc; j++)
			{
				Cache_line* curLine = curSet->lines[j]; // Line at index j
				if (curLine != NULL)
				{
					cout << "    Line: 0x" << setw(2) << setfill('0') << hex << j;
					cout << ", Tag: 0x" << setw(8) << setfill('0') << hex << curLine->Tag;
					cout << ", MESIF: ";
					switch (curLine->State)
					{
					case MESIF_EXCLUSIVE :
						cout << "Exclusive";
						break;
					case MESIF_FORWARD:
						cout << "Forward";
						break;
					case MESIF_INVALID:
						cout << "Invalid";
						break;
					case MESIF_MODIFIED:
						cout << "Modified";
						break;
					case MESIF_SHARED:
						cout << "Shared";
						break;
					}
					cout << endl;
				}
			}
		cout << endl;
		}
	}
	cout << "===========================================" << endl << endl;
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
	int oldTag;

	//If no line was returned, or if the line was returned and is marked invalid, read from the higher cache
	if (CacheRslt == NULL || (CacheRslt != NULL && CacheRslt->State == MESIF_INVALID))
	{
		
		
			
		if (ReadfromRam(address, false))
		{
			bool EvictLine = MainCache->PlaceLineInCache(address, MESIF_FORWARD, &oldTag);
			if (EvictLine)
			{
				unsigned int oldAddress = (oldTag << (MainCache->ByteSelectLength + MainCache->IndexLength) | address & ~(~0 << (MainCache->ByteSelectLength + MainCache->IndexLength)));
				BusOperation(WRITE, oldAddress, GetSnoopResult(oldAddress));
				MessageToL1Cache(INVALIDATE, oldAddress);
			}
			

		}
		else
		{
			if (MainCache->PlaceLineInCache(address, MESIF_EXCLUSIVE, &oldTag))
			{
				unsigned int oldAddress = (oldTag << (MainCache->ByteSelectLength + MainCache->IndexLength) | address & ~(~0 << (MainCache->ByteSelectLength + MainCache->IndexLength)));
				BusOperation(WRITE, oldAddress, GetSnoopResult(oldAddress));
				MessageToL1Cache(INVALIDATE, oldAddress);
			}
		}
	}

	MessageToL1Cache(READ, address);
	
}

//Handles a write request from the L1 cache
void CacheController::WriteRequestFromL1Cache(unsigned int address)
{
	
	int oldTag;
	Cache_line* LineRslt = MainCache->LookupCacheLine(address);


	if (LineRslt == NULL)
	{
		ReadfromRam(address, true);
		if (MainCache->PlaceLineInCache(address, MESIF_MODIFIED, &oldTag))
		{
			unsigned int oldAddress = (oldTag << (MainCache->ByteSelectLength + MainCache->IndexLength) | address & ~(~0 << (MainCache->ByteSelectLength + MainCache->IndexLength)));
			BusOperation(WRITE, oldAddress, GetSnoopResult(oldAddress));
			MessageToL1Cache(INVALIDATE, oldAddress);
		}

	}
	else
	{
		BusOperation(INVALIDATE, address, GetSnoopResult(address));
		LineRslt->State = MESIF_MODIFIED;
	}


}

//Handles a read from memory. Returns true if snoop was successful, false if not
	bool CacheController::ReadfromRam(unsigned int address, bool Rwim)
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
			MessageToL1Cache(INVALIDATE, address);
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
				PutSnoopResult(HITM, address);
				BusOperation(WRITE, address, GetSnoopResult(address));
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
	void CacheController::SnoopWrite(unsigned int address)
	{
		Cache_line* lineRslt = MainCache->LookupCacheLine(address);

		if (lineRslt != NULL)
		{
			lineRslt->State = MESIF_INVALID;
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
				PutSnoopResult(HITM, address);
				BusOperation(WRITE, address, GetSnoopResult(address));
				lineRslt->State = MESIF_INVALID;
			}
				break;
			default:
				break;

			}

		}
		MessageToL1Cache(INVALIDATE, address);
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
		cout << "BusOp: ";
		switch (busOp)
		{
		case READ:
			cout << "Read";
			break;
		case WRITE:
			cout << "Write";
			break;
		case RWIM:
			cout << "RWIM";
			break;
		case INVALIDATE:
			cout << "Inval.";
			break;
		}
		cout << ", Address: 0x" << hex << address << ", Snoop Result: " ;
		switch (SnoopResult)
		{
		case NOHIT:
			cout << "No Hit" << endl;
			break;
		case HIT:
			cout << "Hit" << endl;
			break;
		case HITM:
			cout << "Hit Modified" << endl;
			break;
		}
		
	
#endif
		
	}
	void CacheController::PutSnoopResult(snoopOperationType busOp, unsigned int address)
	{
#ifndef SILENT
			//printf("SnoopResult: Address: %#o, SnoopResult: %d\n", address, busOp);
		cout << "SnoopResult: Address: " << hex << address << "Snoop Result: " << busOp << endl;
#endif
	}
	void CacheController::MessageToL1Cache(busOperationType busOp, unsigned int address)
	{
#ifndef SILENT
		cout << "To L1: BusOp: ";

		switch (busOp)
		{
		case READ:
			cout << "Read";
			break;
		case WRITE:
			cout << "Write";
			break;
		case RWIM:
			cout << "RWIM";
			break;
		case INVALIDATE:
			cout << "Inval.";
			break;
		}
		cout << ", Address: 0x" << hex << address << endl;
#endif
	}