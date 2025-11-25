#include "types.h"
#include "defs.h"
#include "semaphore.h"

semaphore sv[MAX_SEMAPHORE];

void
svinit(void) 
{
	int i;
	for (i = 0; i < MAX_SEMAPHORE; i++)
	{
		sv[i].counter = 0;
		initlock(&(sv[i].lock),"semaphore");
	}
}
