#include "types.h"
#include "user.h"
#include "semaphore.h"
#define SEM_BARRIER 0

int
main(int argc, char *argv[])
{
  if (sem_open(SEM_BARRIER, SEM_OPEN, 0) ==-1){
  	printf(1, "La barrera no esta activa\n");  
  }else{
    sem_up(SEM_BARRIER);
    sem_close(SEM_BARRIER);
  }
  exit();
}

