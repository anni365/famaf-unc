#include "types.h"
#include "stat.h"
#include "user.h"

#define N 128

static float a[N][N];
static float b[N][N];
static float c[N][N];

static void init(void) {
  int x, y;
  for (y = 0; y < N; ++y) {
    for (x = 0; x < N; ++x) {
      a[y][x] = y - x;
      b[y][x] = x - y;
      c[y][x] = 0.0f;
    }
  }
}

static void smm(float beta) {
  int x, y, k;
  for (y = 0; y < N; ++y) {
    for (x = 0; x < N; ++x) {
      for (k = 0; k < N; ++k) {
        c[y][x] += beta * a[y][k] * b[k][x];
      }
    }
  }
}

int
main(int argc, char *argv[])
{
  int start, end, elapsed;
  int ops;

  int pid = getpid();
  float beta = 1.0f;

  init();
  for (;;) {
    start = uptime();
    ops = 0;
    do {
      smm(beta);
      beta = -beta;
      ops += 3 * N * N * N;
      end = uptime();
    } while (end <= start);
    elapsed = end - start;

    printf(1, "%d: %d FrutaFLOPS\n", pid, ops / elapsed);
  }

  printf(1, "%x\n", c[0][0]);
  exit();
  return 0;
}

