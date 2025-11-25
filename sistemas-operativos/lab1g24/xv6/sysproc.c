#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "vga.h"
#include "console.h"

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
sys_modeswitch(void)
{
	int mode;
	argint(0,&mode);
	if (mode==1){
		write_regs(g_320x200x256);
	}
	else if (mode == 0){
		write_regs(g_80x25_text);		
	}
	else {
	return -1;
	}

	return 0;
}

int
sys_plotpixel(void)
{
	int x, y, color;
	
	argint(0,&x);
	argint(1,&y);
	argint(2,&color);
	
	if (x > 319 || y > 200 || x < 0 || y < 0){
		return -1;
	}
	else {	

	uint offset = 320*y + x;
	uchar *VGA = (uchar*)P2V(0xA0000);
	VGA[offset] = color;
	}

	return 0;
}

