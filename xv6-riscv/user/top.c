#include "kernel/types.h"
#include "user/user.h"

#define NAME_WIDTH 16
#define STATE_WIDTH 10
#define PID_WIDTH 6
#define ISKILLED_WIDTH 8
#define PARENT_WIDTH 30

static char *
state_to_string(int state)
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

void print_padded(char *s, int width)
{
    int len = strlen(s);
    printf("%s", s);

    for (int i = len; i < width; i++)
        printf(" ");
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

    print_padded("PID", PID_WIDTH);
    print_padded("STATE", STATE_WIDTH);
    print_padded("KILLED", ISKILLED_WIDTH);
    print_padded("NAME", NAME_WIDTH);
    print_padded("PARENT\tPID\tNAME", PARENT_WIDTH);
    printf("\n");

    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++)
    {
        char buf[16];

        itoa(procs[i].me.pid, buf, 10);
        print_padded(buf, PID_WIDTH);

        print_padded(state_to_string(procs[i].me.state), STATE_WIDTH);

        itoa(procs[i].me.is_killed, buf, 10);
        print_padded(buf, ISKILLED_WIDTH);

        print_padded(procs[i].me.name, NAME_WIDTH);

        printf("\t");
        itoa(procs[i].parent.pid, buf, 10);
        print_padded(buf, PID_WIDTH);
        printf("\t");
        print_padded(procs[i].parent.name, NAME_WIDTH);

        printf("\n");
    }

    exit(0);
}
