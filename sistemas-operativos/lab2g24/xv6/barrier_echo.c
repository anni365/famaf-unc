#include "types.h"
#include "user.h"
#include "semaphore.h"
#define SEM_BARRIER 0

int
main(int argc, char *argv[])
{
  if (sem_open(SEM_BARRIER, SEM_OPEN, 0)==-1){
  	printf(1, "La barrera no esta activa\n");  
  }else{
  	sem_down(SEM_BARRIER);
  	for(int i = 1; i < argc; i++)
    	printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
  	sem_close(SEM_BARRIER);    
  }
  exit();
}

