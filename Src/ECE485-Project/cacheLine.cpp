
#include "cacheLine.h"
using namespace std;
//
// Cache line
//

// Constructor for cache line
Cache_line::Cache_line(unsigned int tag, Mesif_state state)
{
	State = state;
	Tag = tag;
}