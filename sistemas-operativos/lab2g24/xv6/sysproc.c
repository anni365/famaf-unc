#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_sem_open(void)
{
	
	int semaphore, flags, value;
	argint(0,&semaphore);
	argint(1,&flags);
	argint(2,&value);

	if (semaphore < 0 || semaphore > MAX_SEMAPHORE || flags > 3 || flags < 1 || value < 0)
		return -1;		
	else {
		acquire(&(sv[semaphore].lock));						
		if (flags == SEM_CREATE){ 
			if (sv[semaphore].counter == 0){
				sv[semaphore].value = value;
				sv[semaphore].counter++;
			}	
			else {
				release(&sv[semaphore].lock);
				return -1;
			}
		}	
		else if (flags == SEM_OPEN){ 
			if (sv[semaphore].counter > 0){ 
				sv[semaphore].counter++;
			}
			else {
				release(&sv[semaphore].lock);
				return -1;
			}	
		}	
		else if (flags == SEM_OPEN_OR_CREATE){
			if (sv[semaphore].counter == 0){
				sv[semaphore].value = value;
				sv[semaphore].counter++;
			}
			else {
				sv[semaphore].counter++;
			}
		}
	}	
	release(&sv[semaphore].lock);
	return 0;	
}

int
sys_sem_close(void)
{
	
	int semaphore;
	argint(0,&semaphore);
	
	if (semaphore < 0 || semaphore > MAX_SEMAPHORE)
		return -1;
	else {  
		acquire(&(sv[semaphore].lock));
		if (sv[semaphore].counter == 0){ 
			release(&sv[semaphore].lock);
			return 0;
		}
		else {
			sv[semaphore].counter--;
			release(&sv[semaphore].lock);
			return sv[semaphore].counter;
		}
	}
}


int 
sys_sem_up(void)
{
	
	int semaphore;
	argint(0,&semaphore);
	
	if (semaphore < 0 || semaphore > MAX_SEMAPHORE)
		return -1;
	else {
		acquire(&sv[semaphore].lock);
		if(sv[semaphore].counter == 0){
			release(&sv[semaphore].lock);
			return -1;
		}	
		else { 
			sv[semaphore].value++;
			wakeup(&sv[semaphore].value);
			release(&sv[semaphore].lock);
			return 0;
		}
	}
}	


int sys_sem_down(void)
{
	int semaphore;
	argint(0,&semaphore);

	if (semaphore < 0 || semaphore > MAX_SEMAPHORE)
		return -1;
	else { 
		acquire(&(sv[semaphore].lock));
	 	if(sv[semaphore].counter == 0){
			release(&sv[semaphore].lock);
			return -1;
		}
		else { 
			while (sv[semaphore].value == 0){ 
			sleep(&sv[semaphore].value,&sv[semaphore].lock);
			}
			sv[semaphore].value--;
			release(&sv[semaphore].lock);
			return 0;
		}
	}
}	

