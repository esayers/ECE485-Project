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

	lru_state = new bool[assoc - 1];
	for (unsigned int i = 0; i < (assoc - 1); i++)
	{
		lru_state[i] = false;
	}

}

// Destructor for cache set
Cache_set::~Cache_set(void)
{
	for (unsigned int i = 0; i < assoc; ++i) delete lines[i];
	delete[] lines;
	delete[] lru_state;
}

//Looks up cache line based on the tag and returns the cache line object, null if no tag matches.
Cache_line* Cache_set::LookUpCacheLine(unsigned int tag)
{
	for (unsigned int i = 0; i < assoc; i++)
	{
		Cache_line* LineRslt = lines[i];
		if (LineRslt != NULL && LineRslt->Tag == tag)
		{
			UpdateLru(i, 0, assoc-2);
			return LineRslt;
		}
	}
	return NULL;
}

//Place the line in cache and evicts a line if necessary.  Returns "true" if a line needed to be evicted and was in the modified state
bool Cache_set::placeLineInCache(unsigned int tag, Mesif_state mesifStatus)
{
	for (unsigned int i = 0; i < assoc; i++)
	{
		Cache_line* LineRslt = lines[i];
		if (LineRslt == NULL)
		{
			lines[i] = new Cache_line(tag, mesifStatus);
			UpdateLru(i, 0, assoc - 2);
			return false;
		}
		else if (LineRslt->State == MESIF_INVALID)
		{
			delete lines[i];
			lines[i] = new Cache_line(tag, mesifStatus);
			UpdateLru(i, 0, assoc - 2);
			return false;
		}
		
	}

	//If it looped through all the lines and none of them were invalid or empty, then we need to evict a line
	int IndexToEvict = FindEvictLineInLru(0, assoc-2);
	int OrigMesifStatus = lines[IndexToEvict]->State;
	lines[IndexToEvict]->State = mesifStatus;
	lines[IndexToEvict]->Tag = tag;
	return OrigMesifStatus == MESIF_MODIFIED;



}

//Update the LRU bits when doing a read in the cache
void Cache_set::UpdateLru(int lineIndex, int startRange, int endRange)
{
	//If we've come to the point where we're only referring to one bit, then we need to set it and return.
	
	if ((endRange - startRange) == 0)
	{
		//If the lineIndex is odd, we want to put a 1, if not, a 0
		lru_state[endRange] = (lineIndex % 2) == 0;
		return;
	}
	else
	{
		int DistToMidpoint = (endRange - startRange) / 2;
		int LruMidpoint = DistToMidpoint + startRange;


		//If the line index is greater than the midpoint, we want to point the LRU
		//to the opposite side, which is left, if it is less than the midpoint, we want
		//to point it to the right side
		lru_state[LruMidpoint] = !(lineIndex > (LruMidpoint));
		
		//if the address is on the right side, recursively call this function, except
		//restrict the range to right half
		if (!lru_state[LruMidpoint])
			UpdateLru(lineIndex, startRange + DistToMidpoint + 1, endRange);

		//else, call this function again, but restrict the range to the left half.
		else
			UpdateLru(lineIndex, startRange, endRange - (DistToMidpoint + 1));

	}
}

//Find the line index of a cache line in the set. Return the index of an available line
int Cache_set::FindEvictLineInLru(int startRange, int endRange)
{
	//If we've come to the point where we're only referring to one bit, then we need to set it and return.

	if ((endRange - startRange) == 0)
	{
		//If the lineIndex is odd, we want to put a 1, if not, a 0
		lru_state[endRange] = !lru_state[endRange];


		//if the lru state is false, then we need to add one, else, we 
		//can return the index. Note that we flipped the bit above, so we need
		//to reverse the ternary logic.
		return endRange + (lru_state[endRange]==false)?1:0;
	}
	else
	{
		int DistToMidpoint = (endRange - startRange) / 2;
		int LruMidpoint = DistToMidpoint + startRange;

		//Flip the LRU bit at the midpoint
		lru_state[LruMidpoint] = !lru_state[LruMidpoint];

		//if the LRU branch is on the right side, recursively call this function, except
		//restrict the range to right half
		//Note that we flipped the bit above, so we need to make sure to go to the 
		//opposite side
		if (!lru_state[LruMidpoint])
			return FindEvictLineInLru(startRange + DistToMidpoint + 1, endRange);

		//else, call this function again, but restrict the range to the left half.
		else
			return FindEvictLineInLru(startRange, endRange - (DistToMidpoint + 1));

	}
}