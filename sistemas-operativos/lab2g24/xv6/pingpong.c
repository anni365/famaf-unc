#include 	"types.h"
#include 	"user.h"
#include 	"semaphore.h"
#define  	SEM_PING 	1
#define 	SEM_PONG 	0

int 
main (int argc, char *argv[])
{
	sem_open(SEM_PING, SEM_OPEN_OR_CREATE, 1);
	sem_open(SEM_PONG, SEM_OPEN_OR_CREATE, 0);
	int pid = fork();
	if (pid == -1) {
		printf(1,"Algo esta mal \n");
  }
  else {
    for(uint i=0; i<20; i++){	
  	  if (pid>0){
  	    sem_down(SEM_PING);
	  		printf(1, "ping \n");
	  		sem_up(SEM_PONG);
      }
	    else { 
        sem_down(SEM_PONG); 
			  printf(1,"pong \n");
        sem_up(SEM_PING);
	    }
	  }
	}
	sem_close(SEM_PING);
	sem_close(SEM_PONG);
	wait();
	exit();
}
