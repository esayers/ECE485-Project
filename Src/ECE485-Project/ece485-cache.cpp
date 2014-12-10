// ECE 485 Project
// Edward Sayers
// Nathan Becker

#include "ece485-cache.h"
using namespace std;



//
// Cache
//

// Constructor for cache
// INPUT:	total_size, log base 2 of total size of cache
//			line_size, log base 2 of size of one line
//			assoc_pow, log base 2 of associativity of cache
Cache::Cache(unsigned int total_size, unsigned int line_size, unsigned int assoc)
{
	LineSizeBytes = line_size;
	ByteSelectLength = (int)(log10(line_size) / log10(2));
	IndexLength = (int)(log10(total_size / assoc) / log10(2)) - ByteSelectLength;
	TagLength = 32 - ByteSelectLength - IndexLength;
	//TagLength = (int)(32 - (log10(total_size / assoc) / log10(2)));
	//IndexLength = 32 - TagLength - ByteSelectLength;
	this->assoc = assoc;
	num_sets = (1 << IndexLength);
	sets = new Cache_set *[num_sets];
	for (unsigned int i = 0; i < num_sets; ++i) sets[i] = NULL;

	//intialize all statistics
	CacheHits = 0;
	CacheMisses = 0;
	CacheReads = 0;
	CacheWrites = 0;
}

// Destructor for cache
Cache::~Cache()
{
	for (unsigned int i = 0; i < num_sets; ++i) 
		if (sets[i] != NULL)
			delete sets[i];
	delete[] sets;
}


Cache_line* Cache::LookupCacheLine(unsigned int address)
{
	CacheReads++;
	Cache_set* RsltSet = sets[AddressUtils::GetIndex(TagLength, ByteSelectLength, address)];

	Cache_line *RsltLine = NULL;
	if (RsltSet == NULL)
		CacheMisses++;
	else
	{
		RsltLine = RsltSet->LookUpCacheLine(AddressUtils::GetTag(TagLength, address));
		if (RsltLine == NULL)
			CacheMisses++;
		else
		{
			if (RsltLine->State == MESIF_INVALID)
				CacheMisses++;
			else
				CacheHits++;
		}
			
	}

	return RsltLine;

}

//Place the line in cache and evicts a line if necessary.  Returns "true" if a line needed to be evicted and was in the modified state
bool Cache::PlaceLineInCache(unsigned int address, Mesif_state mesifStatus, int *oldAddress)
{
	
	
	CacheWrites++;
	int CacheIndex = AddressUtils::GetIndex(TagLength, ByteSelectLength, address);
	Cache_set* SetRslt = sets[CacheIndex];

	if (SetRslt == NULL)
	{
		SetRslt = new Cache_set(assoc);
		sets[CacheIndex] = SetRslt;
	}

	return SetRslt->placeLineInCache(AddressUtils::GetTag(TagLength, address), mesifStatus, oldAddress);

	
}

/*
int Cache::test(void)
{
	sets[0] = new Cache_set(assoc);
	sets[0]->lines[0] = new Cache_line();
	return 0;
}
*/