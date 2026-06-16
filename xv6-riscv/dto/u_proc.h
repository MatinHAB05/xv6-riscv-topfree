
enum u_procstate
{
    U_UNUSED,
    U_USED,
    U_SLEEPING,
    U_RUNNABLE,
    U_RUNNING,
    U_ZOMBIE
};

struct u_proc
{
    int pid;
    enum u_procstate state;
};