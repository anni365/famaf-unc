#include "types.h" 
#include "user.h"

int 
main (int argc, char *argv[])
{
  int time = uptime(); 
  printf(1,"The uptime is: %d \n", time);
  exit();
}
