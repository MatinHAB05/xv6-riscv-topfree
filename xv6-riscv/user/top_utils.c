#include "user/top.h"

void str_append(char *dest, const char *src) {
  int i = strlen(dest);
  while (*src != '\0') {
    dest[i++] = *src++;
  }
  dest[i] = '\0';
}

char *state_to_string(int state) {
  switch (state) {
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

void print_padded(char *s, int width) {
  int len = strlen(s);
  printf("%s", s);
  for (int i = len; i < width; i++)
    printf(" ");
}

// ---------------------------------------------------------
// FLOAT SIMULATORS (Calculates 1 decimal place via integer math)
// ---------------------------------------------------------

void format_cpu(long ticks, int up_ticks, char opt, char *buf) {
  buf[0] = '\0';
  char tmp[16];

  if (opt == 'p') {
    if (up_ticks <= 0)
      up_ticks = 1;
    // Scale by 1000 to get 1 decimal place (e.g., 1256 for 12.56%)
    long pct_x100 = (ticks * 100 * 100) / up_ticks;
    itoa(pct_x100 / 100, buf, 10); // Whole number
    str_append(buf, ".");
    itoa(pct_x100 % 100, tmp, 10); // Fractional number
    str_append(buf, tmp);
    str_append(buf, "%");
  } else if (opt == 's') {
    itoa(ticks / 10, buf, 10);
    str_append(buf, ".");
    itoa(ticks % 10, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "s");
  } else {
    itoa(ticks, buf, 10); // Raw Ticks
  }
}

void format_mem(long mem_bytes, char opt, char *buf) {
  buf[0] = '\0';
  char tmp[16];

  if (opt == 'p') {
    long pct_x100 = (mem_bytes * 100 * 100) / TOTAL_MEM_BYTES;
    itoa(pct_x100 / 100, buf, 10);
    str_append(buf, ".");
    itoa(pct_x100 % 100, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "%");
  } else if (opt == 'K') {
    long k_x100 = (mem_bytes * 100) / 1024; // 4.84 -> 484
    itoa(k_x100 / 100, buf, 10);
    str_append(buf, ".");
    itoa(k_x100 % 100, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "K");
  } else if (opt == 'M') {
    long m_x1000 = (mem_bytes * 1000) / (1024 * 1024);
    itoa(m_x1000 / 1000, buf, 10);
    str_append(buf, ".");
    itoa(m_x1000 % 1000, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "M");
  } else if (opt == 'G') {
    long g_x10000 = (mem_bytes * 10000) / ((long)1024 * 1024 * 1024);
    itoa(g_x10000 / 10000, buf, 10);
    str_append(buf, ".");
    itoa(g_x10000 % 10000, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "G");
  } else {
    itoa(mem_bytes, buf, 10);
  }
}

void format_time(long ticks, char opt, char *buf) {
  buf[0] = '\0';
  char tmp[16];

  if (opt == 'm') {
    itoa(ticks * 100, buf, 10);
    str_append(buf, "ms");
  } else if (opt == 'M') {
    // Minutes with 1 decimal
    long min_x10 = (ticks * 10) / 600;
    itoa(min_x10 / 10, buf, 10);
    str_append(buf, ".");
    itoa(min_x10 % 10, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "m");
  } else {
    itoa(ticks / 10, buf, 10);
    str_append(buf, ".");
    itoa(ticks % 10, tmp, 10);
    str_append(buf, tmp);
    str_append(buf, "s");
  }
}

void sort_processes(struct u_proc *p, int n, char sort_opt) {
  if (sort_opt == 'n')
    return;

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      int swap = 0;
      if (sort_opt == 'c') {
        if (p[j].me.cpu_ticks < p[j + 1].me.cpu_ticks)
          swap = 1;
      } else if (sort_opt == 'm') {
        if (p[j].me.memory < p[j + 1].me.memory)
          swap = 1;
      } else if (sort_opt == 't') {
        if (p[j].me.cpu_ticks < p[j + 1].me.cpu_ticks)
          swap = 1;
      } else if (sort_opt == 'p') {
        if (p[j].me.pid < p[j + 1].me.pid)
          swap = 1;
      } else if (sort_opt == 'i') {
        if (p[j].parent.pid < p[j + 1].parent.pid)
          swap = 1;
      }

      if (swap) {
        struct u_proc temp = p[j];
        p[j] = p[j + 1];
        p[j + 1] = temp;
      }
    }
  }
}

void print_help(void) {
  // clang-format off
    printf("xv6 Top Process Monitor\n");
    printf("Usage: top [options]\n\n");
    printf("Options:\n");
    printf("  --help, -h               Show help and exit\n");
    printf("  --iter [number]          Number of iterations (default: 5 ; zero means +inf)\n");
    printf("  --cpu [p|t|s]  CPU output: percent, raw ticks, or seconds\n");
    printf("  --mem [p|K|M|G]          Memory unit: percent, KB, MB, GB\n");
    printf("  --sort [pid|cpu|mem|time|ppid]    Sort processes\n");
    printf("  --time [min|sec|m]       Time format: minutes, seconds, ms\n");
    printf("  --pause [number]         Pause amount between updates\n");
    printf("  --unit_pause [m|s|min]   Pause unit\n");
  // clang-format on
}

void print_percentage(long part, long total) {
  if (total == 0) {
    printf("0.00%%");
    return;
  }
  long ratio = (part * 10000) / total;
  int whole = (int)(ratio / 100);
  int frac = (int)(ratio % 100);

  if (frac < 10) {
    printf("%d.0%d%%", whole, frac); // شبیه‌سازی %02d در xv6
  } else {
    printf("%d.%d%%", whole, frac);
  }
}

void print_size(long bytes) {
  if (bytes < 1024) {
    printf("%d B", (int)bytes);
  } else if (bytes < 1024 * 1024) {
    int whole = (int)(bytes / 1024);
    int frac = (int)(((bytes % 1024) * 100) / 1024);
    if (frac < 10)
      printf("%d.0%d KB", whole, frac);
    else
      printf("%d.%d KB", whole, frac);
  } else if (bytes < 1024 * 1024 * 1024) {
    int whole = (int)(bytes / (1024 * 1024));
    int frac = (int)(((bytes % (1024 * 1024)) * 100) / (1024 * 1024));
    if (frac < 10)
      printf("%d.0%d MB", whole, frac);
    else
      printf("%d.%d MB", whole, frac);
  } else {
    long gb = 1024 * 1024 * 1024;
    int whole = (int)(bytes / gb);
    int frac = (int)(((bytes % gb) * 100) / gb);
    if (frac < 10)
      printf("%d.0%d GB", whole, frac);
    else
      printf("%d.%d GB", whole, frac);
  }
}