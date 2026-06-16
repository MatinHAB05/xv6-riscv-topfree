

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

#define TOTAL_MEM_BYTES 134217728

void print_help(void);

struct u_proc procs[64];

void str_append(char *dest, const char *src);

static char *state_to_string(int state);

void print_padded(char *s, int width);

void format_cpu(long ticks, int up_ticks, char opt, char *buf);
void format_mem(long mem_bytes, char opt, char *buf);
void format_time(long ticks, char opt, char *buf);
void sort_processes(struct u_proc *p, int n, char sort_opt);
