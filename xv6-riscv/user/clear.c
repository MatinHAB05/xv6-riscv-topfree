#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  printf("\033[2J\033[H");
  exit(0);
}
