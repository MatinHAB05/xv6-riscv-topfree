#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "dto/u_proc.h"

uint64 read_disk_counter = 0;
uint64 write_disk_counter = 0;
uint64 read_cache_counter = 0;
uint64 write_cahce_counter = 0;
uint64 read_cache_hit_counter = 0;

uint64 sys_exit(void)
{
    int n;
    argint(0, &n);
    kexit(n);

    return 0; // not reached
}

uint64 sys_getpid(void)
{
    return myproc()->pid;
}

uint64 sys_fork(void)
{
    return kfork();
}

uint64 sys_wait(void)
{
    uint64 p;
    argaddr(0, &p);
    return kwait(p);
}

uint64 sys_sbrk(void)
{
    uint64 addr;
    int t;
    int n;

    argint(0, &n);
    argint(1, &t);
    addr = myproc()->sz;

    if (t == SBRK_EAGER || n < 0)
    {
        if (growproc(n) < 0)
        {
            return -1;
        }
    }
    else
    {
        // Lazily allocate memory for this process: increase its memory
        // size but don't allocate memory. If the processes uses the
        // memory, vmfault() will allocate it.
        if (addr + n < addr)
            return -1;
        if (addr + n > TRAPFRAME)
            return -1;
        myproc()->sz += n;
    }
    return addr;
}

uint64 sys_pause(void)
{
    int n;
    uint ticks0;

    argint(0, &n);
    if (n < 0)
        n = 0;
    acquire(&tickslock);
    ticks0 = ticks;
    while (ticks - ticks0 < n)
    {
        if (killed(myproc()))
        {
            release(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

uint64 sys_kill(void)
{
    int pid;

    argint(0, &pid);
    return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void)
{
    uint xticks;

    acquire(&tickslock);
    xticks = ticks;
    release(&tickslock);
    return xticks;
}

uint64
sys_getallprocs(void)
{
    uint64 user_buf;
    int max;

    argaddr(0, &user_buf);
    argint(1, &max);

    return kernel_getallprocs(user_buf, max);
}

uint64
sys_getmemstats(void)
{
    uint64 user_struct_addr;
    struct u_memmory kernel_stats;

    // Retrieve the pointer address passed from user space
    argaddr(0, &user_struct_addr);

    // 1. Populate the counters you implemented
    kernel_stats.read_disk_counter = read_disk_counter;
    kernel_stats.write_disk_counter = write_disk_counter;
    kernel_stats.read_cache_counter = read_cache_counter;
    kernel_stats.write_cache_counter = write_cahce_counter;
    kernel_stats.read_cache_hit_counter = read_cache_hit_counter;

    // 2. Compute dynamic subsystem space sizes
    // TODO: totoal sizes (Ram=128M / Cache = ? / Disk = ?)
    kernel_stats.free_ram = count_free_ram();
    kernel_stats.free_cache = count_free_cache();
    kernel_stats.free_disk = count_free_disk();

    // 3. Securely write kernel structure details into User virtual space
    if (copyout(myproc()->pagetable, user_struct_addr, (char *)&kernel_stats, sizeof(kernel_stats)) < 0)
        return -1;

    return 0;
}