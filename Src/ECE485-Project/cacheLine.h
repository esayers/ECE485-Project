#include "CacheConstants.h"

class Cache_line;
// Single cache line
class Cache_line {

	Mesif_state State;			// MESIF state of line
	unsigned int Tag;			// Current tag

public:
	Cache_line(unsigned int, Mesif_state);
	friend class Cache;
	friend class Cache_set;
	friend class CacheController;

};