
enum u_procstate
{
    U_UNUSED,
    U_USED,
    U_SLEEPING,
    U_RUNNABLE,
    U_RUNNING,
    U_ZOMBIE
};

struct u_proc_info
{
    int pid;
    enum u_procstate state;
    char name[16];
    int is_killed;
    long memory;
    long cpu_ticks;
};

struct u_proc
{
    struct u_proc_info me;
    struct u_proc_info parent;
};