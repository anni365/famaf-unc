#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define N 4096
#define TIMES 4 

static char path[] = "12verduiops";
static char data[N];

int
main(int argc, char *argv[])
{
  int rfd, wfd;
  int pid = getpid();
  int i;

  path[0] = '0' + (pid / 10);
  path[1] = '0' + (pid % 10);

  memset(data, 'a', sizeof(data));

  for(;;) {
    int start = uptime();
    int bytes = 2 * N * TIMES;

    wfd = open(path, O_CREATE | O_WRONLY);
    rfd = open(path, O_RDONLY);

    for(i = 0; i < TIMES; ++i) {
      write(wfd, data, N);
    }
    for(i = 0; i < TIMES; ++i) {
      read(rfd, data, N);
    }
    close(wfd);
    close(rfd);
    int end = uptime();
    long elapsed = (long) end - (long) start;

    printf(1, "\t\t\t\t\t%d: %d VerduIOPS\n", pid, (int) (bytes / elapsed));
  }

  exit();
  return 0;
}
