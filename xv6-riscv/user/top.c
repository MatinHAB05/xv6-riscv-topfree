//  TODO: arg options!
//  TODO: memeory K/M/G/%
//  TODO: cpu ticks/Second/%
//  TODO: pause time arg(fornow it is 100ms scale!)
//  TODO: Sort Option
//  TODO: one-view data in header(#running proccess / #process / uptime /  )
//  TODO: read_timeout for q (quit)

#include "user/top.h"

struct u_memmory umemstat;
struct u_proc procs[64];

// TODO: Quit on 'q' Handler
// In standard xv6, read() blocks execution. To implement non-blocking "q to quit",
// you must either use the `select` system call (if you have implemented it in your kernel)
// or use O_NONBLOCK flags. Here is a placeholder sleep loop you can modify.
void wait_and_check_quit(int target_ticks)
{
    // If you have implemented select(), you would put it inside this loop
    // to check standard input (fd 0) for the 'q' character.
    pause(target_ticks);
}

// Parses argv into the config struct
void parse_args(int argc, char *argv[], struct top_config *cfg)
{
    int flag = 97;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            print_help();
            exit(0);
        }
        else if (strcmp(argv[i], "--iter") == 0 && i + 1 < argc)
        {
            cfg->max_iterations = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--cpu") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "t") == 0)
                cfg->cpu_opt = 't';
            else if (strcmp(argv[i], "s") == 0)
                cfg->cpu_opt = 's';
            else if (strcmp(argv[i], "p") == 0)
                cfg->cpu_opt = 'p';
            else
            {
                printf("[INVALID VALUE FOR --cpu]: '%s' (Valid: t, s, p)\n", argv[i]);
                flag = -97;
            }
        }
        else if (strcmp(argv[i], "--mem") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "K") == 0)
                cfg->mem_opt = 'K';
            else if (strcmp(argv[i], "M") == 0)
                cfg->mem_opt = 'M';
            else if (strcmp(argv[i], "G") == 0)
                cfg->mem_opt = 'G';
            else if (strcmp(argv[i], "p") == 0)
                cfg->mem_opt = 'p';
            else
            {
                printf("[INVALID VALUE FOR --mem]: '%s' (Valid: K, M, G, p)\n", argv[i]);
                flag = -97;
            }
        }
        else if (strcmp(argv[i], "--sort") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "cpu") == 0)
                cfg->sort_opt = 'c';
            else if (strcmp(argv[i], "mem") == 0)
                cfg->sort_opt = 'm';
            else if (strcmp(argv[i], "time") == 0)
                cfg->sort_opt = 't';
            else if (strcmp(argv[i], "pid") == 0)
                cfg->sort_opt = 'p';
            else if (strcmp(argv[i], "ppid") == 0)
                cfg->sort_opt = 'i';
            else
            {
                printf("[INVALID VALUE FOR --sort]: '%s' (Valid: cpu, mem, time, pid, ppid)\n", argv[i]);
                flag = -97;
            }
        }
        else if (strcmp(argv[i], "--time") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "min") == 0)
                cfg->time_opt = 'M';
            else if (strcmp(argv[i], "m") == 0)
                cfg->time_opt = 'm';
            else if (strcmp(argv[i], "s") == 0)
                cfg->time_opt = 's';
            else
            {
                printf("[INVALID VALUE FOR --time]: '%s' (Valid: min, m, s)\n", argv[i]);
                flag = -97;
            }
        }
        else if (strcmp(argv[i], "--pause") == 0 && i + 1 < argc)
        {
            cfg->pause_duration = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--unit_pause") == 0 && i + 1 < argc)
        {
            i++;
            if (strcmp(argv[i], "min") == 0)
                cfg->pause_unit = 'M';
            else if (strcmp(argv[i], "s") == 0)
                cfg->pause_unit = 's';
            else if (strcmp(argv[i], "m") == 0)
                cfg->pause_unit = 'm';
            else
            {
                printf("[INVALID VALUE FOR --unit_pause]: '%s' (Valid: min, s, m)\n", argv[i]);
                flag = -97;
            }
        }
        else
        {
            printf("[INVALID FLAG]: '%s'\n", argv[i]);
            flag = -97;
        }
    }

    if (flag < 0)
    {
        printf("Use '--help' or '-h' to see the usage dictionary...\n");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    // 1. Setup Defaults
    struct top_config config = {
        .max_iterations = 5,
        .cpu_opt = 'p',
        .mem_opt = 'p',
        .sort_opt = 'n',
        .time_opt = 's',
        .pause_duration = 20,
        .pause_unit = 't' // default raw ticks
    };

    // 2. Parse User Inputs
    parse_args(argc, argv, &config);

    // 3. Calculate target ticks for pausing
    int target_pause_ticks = config.pause_duration;
    if (config.pause_unit == 'm')
        target_pause_ticks = config.pause_duration / 100;
    else if (config.pause_unit == 's')
        target_pause_ticks = config.pause_duration * 10;
    else if (config.pause_unit == 'M')
        target_pause_ticks = config.pause_duration * 600;

    if (target_pause_ticks <= 0)
        target_pause_ticks = 1;

    // 4. Main Rendering Loop
    for (int iter = 0; iter < config.max_iterations; iter++)
    {
        printf("\033[2J\033[H");

        int mem_stats_ok = (getmemstats(&umemstat) == 0);

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

        sort_processes(procs, n, config.sort_opt);

        // Header view
        printf("========================================================================================================\n");
        printf(" System Uptime: %d.%ds | Total Processes: %d | Running Processes: %d\n",
               (up_ticks / 10), up_ticks % 10, n, running_count);

        if (mem_stats_ok)
        {
            long total_disk = umemstat.read_disk_counter + umemstat.write_disk_counter;
            long total_cache = umemstat.read_cache_counter + umemstat.write_cache_counter;

            printf(" RAM Free: ");
            print_size(umemstat.free_ram);
            printf(" | Cache Free: ");
            print_size(umemstat.free_cache);
            printf(" | Disk Free: ");
            print_size(umemstat.free_disk);
            printf("\n");

            printf(" Disk Reads: %d (", (int)umemstat.read_disk_counter);
            print_percentage(umemstat.read_disk_counter, total_disk);
            printf(") | Writes: %d (", (int)umemstat.write_disk_counter);
            print_percentage(umemstat.write_disk_counter, total_disk);
            printf(")\n");

            printf(" Cache Reads: %d (", (int)umemstat.read_cache_counter);
            print_percentage(umemstat.read_cache_counter, total_cache);
            printf(") | Writes: %d (", (int)umemstat.write_cache_counter);
            print_percentage(umemstat.write_cache_counter, total_cache);
            printf(")\n");

            printf(" Cache Hit Rate: ");
            print_percentage(umemstat.read_cache_hit_counter, umemstat.read_cache_counter);
            printf(" (Hits: %d)\n", (int)umemstat.read_cache_hit_counter);
        }
        else
        {
            printf(" [MEMSTAT ERROR]: Could not fetch memory, cache, and disk stats from kernel!\n");
        }

        printf("========================================================================================================\n\n");

        print_padded("PID", PID_WIDTH);
        print_padded("STATE", STATE_WIDTH);
        print_padded("KILLED", KILLED_WIDTH);
        print_padded("NAME", NAME_WIDTH);

        char title_buf[16];
        strcpy(title_buf, (config.cpu_opt == 'p' ? "CPU(%)" : (config.cpu_opt == 's' ? "CPU(s)" : "CPU(tick)")));
        print_padded(title_buf, CPU_WIDTH);

        strcpy(title_buf, (config.mem_opt == 'p' ? "MEM(%)" : "MEM(Sz)"));
        print_padded(title_buf, MEMORY_WIDTH);

        print_padded("TIME", TIME_WIDTH);
        print_padded("PARENT\tPID\tNAME", PARENT_WIDTH);
        printf("\n");

        printf("--------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < n; i++)
        {
            char buf[32];

            itoa(procs[i].me.pid, buf, 10);
            print_padded(buf, PID_WIDTH);

            print_padded(state_to_string(procs[i].me.state), STATE_WIDTH);

            itoa(procs[i].me.is_killed, buf, 10);
            print_padded(buf, KILLED_WIDTH);

            print_padded(procs[i].me.name, NAME_WIDTH);

            format_cpu(procs[i].me.cpu_ticks, up_ticks, config.cpu_opt, buf);
            print_padded(buf, CPU_WIDTH);

            format_mem(procs[i].me.memory, config.mem_opt, buf);
            print_padded(buf, MEMORY_WIDTH);

            format_time(procs[i].me.cpu_ticks, config.time_opt, buf);
            print_padded(buf, TIME_WIDTH);

            printf("\t\t");
            if (procs[i].parent.pid < 0)
            {

                print_padded("-", PID_WIDTH);
            }
            else
            {
                itoa(procs[i].parent.pid, buf, 10);
                print_padded(buf, PID_WIDTH);
            }

            printf("\t");
            print_padded(procs[i].parent.name, NAME_WIDTH);
            printf("\n");
        }

        // Wait between iterations
        wait_and_check_quit(target_pause_ticks);
    }

    exit(0);
}