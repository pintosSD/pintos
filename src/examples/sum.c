#include <stdio.h>
#include <syscall.h>
#include <string.h>

int main(int argc, char **argv) {
  
  printf("%d ", pibonacci(atoi(argv[1])));
  printf("%d\n", sum(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])));

  return EXIT_SUCCESS;
}
