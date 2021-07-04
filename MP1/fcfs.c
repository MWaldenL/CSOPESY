#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
  int pid, t_arrival, t_burst;
  int t_start, t_end, t_wait, t_turnaround;
} process_fcfs;  

typedef struct gantt_fcfs {
  int gantt_processes[MAX];
  int gantt_timestamps[MAX+1];
  int process_length;
} gantt_fcfs;

void init_gantt_fcfs(gantt_fcfs* g, int init_start_time) {
  g->gantt_timestamps[0] = init_start_time;
}

void update_gantt_fcfs(gantt_fcfs* g, int pid, int end_time) {
  int ind = g->process_length;
  g->gantt_processes[ind] = pid;
  g->gantt_timestamps[ind+1] = end_time;
  g->process_length += 1;
}

void print_gantt_fcfs(gantt_fcfs g) {
  printf("Gantt Chart:\n");
  for (int i = 0; i < g.process_length; i++)
    printf(".-------");
  printf(".\n");

  for (int i = 0; i < g.process_length; i++)
    if (g.gantt_processes[i] == -1)
      printf("| ----- ", g.gantt_processes[i]);
    else
      printf("| P%-5d", g.gantt_processes[i]);
  printf("|\n");

  for (int i = 0; i < g.process_length; i++)
    printf(":-------");
  printf(":\n");

  printf("%-3d", g.gantt_timestamps[0]);
  for (int i = 0; i < g.process_length; i++) {
    printf("     %-3d", g.gantt_timestamps[i+1]);
  }
  printf("\n");
}

int t_arrival_asc_fcfs(const void* a, const void* b) {
  process_fcfs* p1 = (process_fcfs*) a;
  process_fcfs* p2 = (process_fcfs*) b;
  return p1->t_arrival - p2->t_arrival;
}

void print_fcfs(process_fcfs* plist, int y) {
  for (int i=0; i < y; i++) {
    process_fcfs p = plist[i];
    printf("P[%d]\n", p.pid);
    printf("Start Time: %d End time: %d\n", p.t_start, p.t_end);
    printf("Waiting time: %d\n", p.t_wait);
    printf("Turnaround time: %d\n", p.t_turnaround);
    printf("****************************\n");
  }
  float t_avg = 0.0;
  for (int i=0; i < y; i++) {
    t_avg += plist[i].t_wait;
  }
  t_avg /= y;
  printf("\nAverage waiting time: %.1lf\n", t_avg);
}

int max(int a, int b) {
  return a > b ? a : b;
}

void fcfs(FILE* fp, int y) {
  process_fcfs plist[y+1];
  gantt_fcfs gc = { .process_length = 0 };
  for (int i=0; i < y; i++) {
    int a, b, c; fscanf(fp, "%d%d%d\n", &a, &b, &c);
    process_fcfs p = {.pid = a, .t_arrival = b, .t_burst = c,
      .t_wait = 0, .t_turnaround = 0};
    plist[i] = p;
  }
  int last = plist[0].t_arrival, diff = 0; // check if arrival times are different
  for (int i=1; i < y; i++) {
    if (last != plist[i].t_arrival) {
      diff = 1;
      break;
    }
    last = plist[i].t_arrival;
  }
  if (diff) { // qsort messes up ordering for equal values
    qsort(plist, y, sizeof(process_fcfs), t_arrival_asc_fcfs);
  }
  int t_burst_last = 0;
  for (int i=0; i < y; i++) {
    plist[i].t_start = i > 0 ? max(plist[i].t_arrival, plist[i-1].t_end) : plist[i].t_arrival;
    plist[i].t_end = plist[i].t_start + plist[i].t_burst;
    if (i == 0)
      init_gantt_fcfs(&gc, plist[0].t_start);
    if (plist[i].t_start > gc.gantt_timestamps[i])
      update_gantt_fcfs(&gc, -1, plist[i].t_start);
    update_gantt_fcfs(&gc, plist[i].pid, plist[i].t_end);
  }
  for (int i=0; i < y; i++) {
    plist[i].t_wait = plist[i].t_start - plist[i].t_arrival;
  }
  for (int i=0; i < y; i++) {
    plist[i].t_turnaround = plist[i].t_wait + plist[i].t_burst;
  }
  print_fcfs(plist, y);
  print_gantt_fcfs(gc);
}
    