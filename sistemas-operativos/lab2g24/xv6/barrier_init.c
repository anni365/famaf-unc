#include "types.h"
#include "user.h"
#include "semaphore.h"
#define SEM_BARRIER 0

int
main(int argc, char *argv[])
{
  
	if (sem_open(SEM_BARRIER, SEM_CREATE, 0)==-1){
  	printf(1, "La barrera ya fue inicializada\n");
  }
  exit();
}

