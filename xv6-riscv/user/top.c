#include "kernel/types.h"
#include "user/user.h"

#define NAME_WIDTH 16
#define STATE_WIDTH 10
#define PID_WIDTH 6
#define ISKILLED_WIDTH 8
#define PARENT_WIDTH 30
#define MEMMORY_WIDTH 10
#define CPU_WIDTH 8
#define TIME_WIDTH 12

struct u_proc procs[64];

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

    // TODO : arg options!
    // TODO : memeory K/M/G/%
    // TODO : cpu ticks/Second/%
    // TODO : pause time arg(fornow it is 100ms scale!)
    // TODO : Sort Option
    // TODO : one-view data in header(#running proccess / #process / uptime /  )
    // TODO : read_timeout for q (quit)

    for (int iter = 0; iter < 5; iter++)
    {
        printf("\033[2J\033[H");
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
        print_padded("CPU%", CPU_WIDTH);
        print_padded("MEM%", MEMMORY_WIDTH);
        print_padded("TIME", TIME_WIDTH);
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

            itoa(procs[i].me.cpu_ticks, buf, 10);
            print_padded(buf, CPU_WIDTH);

            itoa(procs[i].me.memory, buf, 10);
            print_padded(buf, MEMMORY_WIDTH);

            print_padded("00:00:06:100", TIME_WIDTH);

            printf("          \t");
            itoa(procs[i].parent.pid, buf, 10);
            print_padded(buf, PID_WIDTH);
            printf("\t");
            print_padded(procs[i].parent.name, NAME_WIDTH);

            printf("\n");
        }
        pause(20);
    }

    exit(0);
}
