#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  printf("Hello World , I am trying xv6 as well :)\n");
  int i = 999999999;
  while (1) {
    i--;
    if (i < -99999) {
      break;
    }
  }
  printf("Hello World , I am trying xv6 as well :)\n");

  exit(0);
}
