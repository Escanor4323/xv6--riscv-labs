#include "kernel/types.h"
#include "user/user.h"

#define N 64

static int a[N][N];
static int b[N][N];
static int c[N][N];

int
main(int argc, char **argv)
{
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      a[i][j] = (i + j) & 0xFF;
      b[i][j] = (i * j) & 0xFF;
      c[i][j] = 0;
    }
  }

  for (int i = 0; i < N; i++)
    for (int k = 0; k < N; k++)
      for (int j = 0; j < N; j++)
        c[i][j] += a[i][k] * b[k][j];

  int sum = 0;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      sum += c[i][j];

  printf("Time: %d ticks\n", sum & 0x7fffffff);
  exit(0);
}

