
#include "cacheSet.h"
using namespace std;
//
// Cache Set
//

// Constructor for cache set
// INPUT:	assoc, Associativity of set
Cache_set::Cache_set(unsigned int assoc)
{
	this->assoc = assoc;
	lines = new Cache_line *[assoc];
	for (unsigned int i = 0; i < assoc; ++i) lines[i] = NULL;
}

// Destructor for cache set
Cache_set::~Cache_set(void)
{
	for (unsigned int i = 0; i < assoc; ++i) delete lines[i];
	delete[] lines;
}

//Looks up cache line based on the tag and returns the cache line object, null if no tag matches.
Cache_line* Cache_set::LookUpCacheLine(unsigned int tag)
{
	
	int LineIndex;
	//Do Stuff

	UpdateLru(LineIndex, 0, assoc);
}

//Place the line in cache and evicts a line if necessary.  Returns "true" if a line needed to be evicted and was in the modified state
bool Cache_set::placeLineInCache(unsigned int tag, Mesif_state mesifStatus)
{

}

//Update the LRU bits when doing a read in the cache
void Cache_set::UpdateLru(int lineIndex, int startRange, int endRange)
{

}

//Find the line index of a cache line in the set. Return the index of an available line
int Cache_set::FindEvictLineInLru(int startRange, int endRange)
{

}