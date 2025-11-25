#include "spinlock.h"

#define MAX_SEMAPHORE 			14

#define SEM_CREATE 					1
#define SEM_OPEN						2
#define SEM_OPEN_OR_CREATE	3

struct semaphore
{
	struct spinlock lock;
	int value;
	int counter;
};

typedef struct semaphore semaphore;

extern semaphore sv[]; 
