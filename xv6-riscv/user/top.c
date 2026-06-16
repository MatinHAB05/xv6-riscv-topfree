// TODO : arg options!
// TODO : memeory K/M/G/%
// TODO : cpu ticks/Second/%
// TODO : pause time arg(fornow it is 100ms scale!)
// TODO : Sort Option
// TODO : one-view data in header(#running proccess / #process / uptime /  )
// TODO : read_timeout for q (quit)

#include "kernel/types.h"
#include "user/user.h"
#include "top.h"

void str_append(char *dest, const char *src)
{
    int i = strlen(dest);
    while (*src != '\0')
    {
        dest[i++] = *src++;
    }
    dest[i] = '\0';
}

static char *state_to_string(int state)
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

void format_cpu(long ticks, int up_ticks, char opt, char *buf)
{
    buf[0] = '\0';
    if (opt == 'p')
    {
        if (up_ticks <= 0)
            up_ticks = 1;
        long pct = (ticks * 100) / up_ticks;
        itoa(pct, buf, 10);
        str_append(buf, "%");
    }
    else if (opt == 's')

    {
        itoa(ticks / 10, buf, 10);
        str_append(buf, "s");
    }
    else
    {
        itoa(ticks, buf, 10);
    }
}

void format_mem(long mem_bytes, char opt, char *buf)
{
    buf[0] = '\0';
    if (opt == 'p')
    { // درصد
        long pct = (mem_bytes * 100) / TOTAL_MEM_BYTES;
        itoa(pct, buf, 10);
        str_append(buf, "%");
    }
    else if (opt == 'K')
    {
        itoa(mem_bytes / 1024, buf, 10);
        str_append(buf, "K");
    }
    else if (opt == 'M')
    {
        itoa(mem_bytes / (1024 * 1024), buf, 10);
        str_append(buf, "M");
    }
    else if (opt == 'G')
    {
        itoa(mem_bytes / (1024 * 1024 * 1024), buf, 10);
        str_append(buf, "G");
    }
    else
    {
        itoa(mem_bytes, buf, 10); // خام
    }
}

void format_time(long ticks, char opt, char *buf)
{
    buf[0] = '\0';
    if (opt == 'm')
    {
        itoa(ticks * 100, buf, 10);
        str_append(buf, "ms");
    }
    else if (opt == 'M')
    {
        itoa(ticks / 600, buf, 10);
        str_append(buf, "m");
    }
    else
    {
        itoa(ticks / 10, buf, 10);
        str_append(buf, "s");
    }
}

void sort_processes(struct u_proc *p, int n, char sort_opt)
{
    if (sort_opt == 'n')
        return;
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            int swap = 0;
            if (sort_opt == 'c')
            { // CPU
                if (p[j].me.cpu_ticks < p[j + 1].me.cpu_ticks)
                    swap = 1;
            }
            else if (sort_opt == 'm')
            { // Memory
                if (p[j].me.memory < p[j + 1].me.memory)
                    swap = 1;
            }
            else if (sort_opt == 't')
            {

                if (p[j].me.cpu_ticks < p[j + 1].me.cpu_ticks)
                    swap = 1;
            }

            if (swap)
            {
                struct u_proc temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int iter_count = 5;
    char cpu_opt = 'p';
    char mem_opt = 'p';
    char sort_opt = 'n';
    char time_opt = 's';
    int pause_val = 20;
    char deghat_pause = 't';

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 && i + 1 < argc)
        {
            print_help();
        }
        else if (strcmp(argv[i], "--iter") == 0 && i + 1 < argc)
        {
            iter_count = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--cpu") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "ticks_count") == 0)
                cpu_opt = 't';
            else if (strcmp(argv[i], "timeinsecond") == 0)
                cpu_opt = 's';
            else
                cpu_opt = 'p';
        }
        else if (strcmp(argv[i], "--mem") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "K") == 0)
                mem_opt = 'K';
            else if (strcmp(argv[i], "M") == 0)
                mem_opt = 'M';
            else if (strcmp(argv[i], "G") == 0)
                mem_opt = 'G';
            else
                mem_opt = 'p';
        }
        else if (strcmp(argv[i], "--sort") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "cpu") == 0)
                sort_opt = 'c';
            else if (strcmp(argv[i], "mem") == 0)
                sort_opt = 'm';
            else if (strcmp(argv[i], "time") == 0)
                sort_opt = 't';
        }
        else if (strcmp(argv[i], "--time") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "min") == 0)
                time_opt = 'M';
            else if (strcmp(argv[i], "m") == 0)
                time_opt = 'm';
            else
                time_opt = 's';
        }
        else if (strcmp(argv[i], "--pause") == 0 && i + 1 < argc)
        {
            pause_val = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--deghat_pause") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "min") == 0)
                deghat_pause = 'M';
            else if (strcmp(argv[i], "s") == 0)
                deghat_pause = 's';
            else if (strcmp(argv[i], "m") == 0)
                deghat_pause = 'm';
        }
    }

    int target_pause_ticks = pause_val;
    if (deghat_pause == 'm')
        target_pause_ticks = pause_val / 100; // ms to ticks
    else if (deghat_pause == 's')
        target_pause_ticks = pause_val * 10; // sec to ticks
    else if (deghat_pause == 'M')
        target_pause_ticks = pause_val * 600; // min to ticks

    if (target_pause_ticks <= 0)
        target_pause_ticks = 1;

    for (int iter = 0; iter < iter_count; iter++)
    {
        printf("\033[2J\033[H");

        int n = getallprocs(procs, 64);
        if (n < 0)
        {
            printf("getallprocs failed\n");
            exit(1);
        }

        int up_ticks = uptime();
        int running_count = 0;

        for (int i = 0; i < n; i++)
        {
            if (procs[i].me.state == U_RUNNING)
                running_count++;
        }

        sort_processes(procs, n, sort_opt);

        printf("================================================================================\n");
        printf(" System Uptime: %ds | Total Processes: %d | Running Processes: %d\n",
               (up_ticks / 10), n, running_count);
        printf("================================================================================\n\n");

        print_padded("PID", PID_WIDTH);
        print_padded("STATE", STATE_WIDTH);
        print_padded("KILLED", ISKILLED_WIDTH);
        print_padded("NAME", NAME_WIDTH);

        char title_buf[16];
        strcpy(title_buf, (cpu_opt == 'p' ? "CPU(%)" : (cpu_opt == 's' ? "CPU(s)" : "CPU(tick)")));
        print_padded(title_buf, CPU_WIDTH);

        strcpy(title_buf, (mem_opt == 'p' ? "MEM(%)" : "MEM(Sz)"));
        print_padded(title_buf, MEMMORY_WIDTH);

        print_padded("TIME", TIME_WIDTH);
        print_padded("PARENT\tPID\tNAME", PARENT_WIDTH);
        printf("\n");

        printf("--------------------------------------------------------------------------------\n");

        for (int i = 0; i < n; i++)
        {
            char buf[32];

            // PID
            itoa(procs[i].me.pid, buf, 10);
            print_padded(buf, PID_WIDTH);

            // STATE
            print_padded(state_to_string(procs[i].me.state), STATE_WIDTH);

            // KILLED
            itoa(procs[i].me.is_killed, buf, 10);
            print_padded(buf, ISKILLED_WIDTH);

            // NAME
            print_padded(procs[i].me.name, NAME_WIDTH);

            // CPU
            format_cpu(procs[i].me.cpu_ticks, up_ticks, cpu_opt, buf);
            print_padded(buf, CPU_WIDTH);

            // MEMORY
            format_mem(procs[i].me.memory, mem_opt, buf);
            print_padded(buf, MEMMORY_WIDTH);

            format_time(procs[i].me.cpu_ticks, time_opt, buf);
            print_padded(buf, TIME_WIDTH);

            // PARENT INFO
            printf("          \t");
            itoa(procs[i].parent.pid, buf, 10);
            print_padded(buf, PID_WIDTH);
            printf("\t");
            print_padded(procs[i].parent.name, NAME_WIDTH);

            printf("\n");
        }

        pause(target_pause_ticks);
    }

    exit(0);
}

void print_help(void)
{
    printf("xv6 Top Process Monitor\n");
    printf("Usage: top [options]\n\n");
    printf("Options:\n");
    printf("  --help, -h               Show this help message and exit\n");
    printf("  --iter [number]          Number of screen updates (default: 5)\n");
    printf("  --cpu [p|ticks_count|timeinsecond]\n");
    printf("                           p: CPU percentage (default)\n");
    printf("                           ticks_count: Raw CPU ticks\n");
    printf("                           timeinsecond: Time spent in seconds\n");
    printf("  --mem [p|K|M|G]          Memory unit: percent(p), KB(K), MB(M), GB(G)\n");
    printf("  --sort [cpu|mem|time]    Sort processes by resource usage\n");
    printf("  --time [min|sec|m]       Process run-time unit: minutes, seconds, ms\n");
    printf("  --pause [number]         Pause duration between iterations\n");
    printf("  --deghat_pause [m|s|min] Unit for pause: ms(m), seconds(s), minutes(min)\n");
}