#include "kernel/types.h"
#include "user/user.h"

// Formatting Widths
#define PID_WIDTH 6
#define STATE_WIDTH 10
#define KILLED_WIDTH 8
#define NAME_WIDTH 16
#define CPU_WIDTH 10
#define MEMORY_WIDTH 10
#define TIME_WIDTH 10
#define PARENT_WIDTH 30

// Assuming 128 MB total physical memory for xv6
#define TOTAL_MEM_BYTES 134217728

// Configuration Struct
struct topfree_config
{
    int max_iterations;
    char cpu_opt;       // 'p' (%), 't' (ticks), 's' (seconds)
    char mem_opt;       // 'p' (%), 'K', 'M', 'G'
    char sort_opt;      // 'n' (none), 'c' (cpu), 'm' (mem), 't' (time)
    char time_opt;      // 'm' (ms), 's' (sec), 'M' (min)
    int pause_duration; // Raw pause value
    char pause_unit;    // 'm' (ms), 's' (sec), 'M' (min)
};

// Function Prototypes
void str_append(char *dest, const char *src);
void print_padded(char *s, int width);
char *state_to_string(int state);

void format_cpu(long ticks, int up_ticks, char opt, char *buf);
void format_mem(long mem_bytes, char opt, char *buf);
void format_time(long ticks, char opt, char *buf);

void sort_processes(struct u_proc *p, int n, char sort_opt);
void print_help(void);

void print_size(long bytes);
void print_percentage(long part, long total);