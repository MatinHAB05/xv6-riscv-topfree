#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  int state = toggle_debug();

  if (state == 1) {
    printf("Kernel debug mode has been enabled (ON).\n");
  } else if (state == 0) {
    printf("Kernel debug mode has been disabled (OFF).\n");
  } else {
    printf("Error: Failed to toggle debug mode.\n");
  }

  exit(0);
}