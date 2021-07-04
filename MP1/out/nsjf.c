#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 100

typedef struct {
  int pid, t_arrival, t_burst;
  int t_start, t_end, t_wait, t_turnaround;
  int done;
} process_nsjf;  

typedef struct gantt_nsjf {
  int gantt_processes[MAX];
  int gantt_timestamps[MAX+1];
  int process_length;
} gantt_nsjf;

void init_gantt_nsjf(gantt_nsjf* g, int init_start_time) {
  g->gantt_timestamps[0] = init_start_time;
}

void update_gantt_nsjf(gantt_nsjf* g, int pid, int end_time) {
  int ind = g->process_length;
  g->gantt_processes[ind] = pid;
  g->gantt_timestamps[ind+1] = end_time;
  g->process_length += 1;
}

void print_gantt_nsjf(gantt_nsjf g) {
  printf("Gantt Chart:\n");
  for (int i = 0; i < g.process_length; i++)
    printf(".-------");
  printf(".\n");

  for (int i = 0; i < g.process_length; i++)
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

void print_nsjf(process_nsjf* plist, int y) {
  for (int i=0; i < y; i++) {
    process_nsjf p = plist[i];
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

int t_arrival_asc_nsjf(const void* a, const void* b) {
  process_nsjf* p1 = (process_nsjf*) a;
  process_nsjf* p2 = (process_nsjf*) b;
  return p1->t_arrival - p2->t_arrival;
}

int ind_nxt_proc(process_nsjf* plist, int y, int t_curr) {
  int ind = -1;
  int t_smallest_exec = __INT_MAX__;
  for (int i = 0; i < y; i++) {
    process_nsjf p = plist[i];
    if (p.done == 0 && p.t_arrival <= t_curr) {
      if (p.t_burst < t_smallest_exec) {
        ind = i;
        t_smallest_exec = p.t_burst;
      }
    }
  }
  return ind;
}

void nsjf(FILE *fp, int y) {
  process_nsjf plist[y]; 
  gantt_nsjf gc = { .process_length = 0 };

  for (int i = 0; i < y; i++) {
    int a, b, c; fscanf(fp, "%d%d%d", &a, &b, &c);
    process_nsjf p = { .pid = a, .t_arrival = b, .t_burst = c, .t_wait = 0, .t_turnaround = 0, .done = 0};
    plist[i] = p;
  }

  qsort(plist, y, sizeof(process_nsjf), t_arrival_asc_nsjf);

  int t_curr = plist[0].t_arrival;
  int ind;
  init_gantt_nsjf(&gc, t_curr);
  for (int i = 0; i < y; i++) {
    ind = ind_nxt_proc(plist, y, t_curr);

    plist[ind].t_start = t_curr;
    plist[ind].t_end = plist[ind].t_start + plist[ind].t_burst;
    plist[ind].t_wait = t_curr - plist[ind].t_arrival;
    plist[ind].t_turnaround = plist[ind].t_end - plist[ind].t_arrival;
    plist[ind].done = 1;

    update_gantt_nsjf(&gc, plist[ind].pid, plist[ind].t_end);

    t_curr += plist[ind].t_burst;
  }

  print_nsjf(plist, y);
  print_gantt_nsjf(gc);
}
