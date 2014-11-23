/*
*Bus Operation types
*/
enum busOperationType
{
	/* Bus Read */
	READ = 1,
	/* Bus Write */
	WRITE,
	/* Bus Invalidate */
	INVALIDATE,

	/* Bus Read With Intent to Modify*/
	RWIM
};



enum Mesif_state {
	MESIF_INVALID,
	MESIF_EXCLUSIVE,
	MESIF_MODIFIED,
	MESIF_SHARED,
	MESIF_FORWARD,
};

/*
*Snoop Result types
*/
enum snoopOperationType
{
	/* No hit */
	NOHIT = 0,
	/* Hit */
	HIT,
	/* Hit to modified line*/
	HITM
};
