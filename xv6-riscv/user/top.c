#include "kernel/types.h"
#include "user/user.h"

char *
state_to_string(enum u_procstate state)
{
    switch (state)
    {
    case U_UNUSED:
        return "UNUSED";
    case U_USED:
        return "USED";
    case U_SLEEPING:
        return "SLEEPING";
    case U_RUNNABLE:
        return "RUNNABLE";
    case U_RUNNING:
        return "RUNNING";
    case U_ZOMBIE:
        return "ZOMBIE";
    default:
        return "UNKNOWN";
    }
}

int main(int argc, char *argv[])
{
    struct u_proc procs[64];

    int n = getallprocs(procs, 64);

    if (n < 0)
    {
        printf("getallprocs failed\n");
        exit(1);
    }

    printf("Found %d processes\n\n", n);

    printf("PID\tSTATE\n");
    printf("------------------\n");

    for (int i = 0; i < n; i++)
    {
        printf("%d\t%s\n",
               procs[i].pid,
               state_to_string(procs[i].state));
    }

    exit(0);
}