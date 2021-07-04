#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 100

typedef struct {
  int pid, t_arrival, t_burst;
  int t_start[101], t_end[101], t_wait, t_turnaround;
  int i;
} process_psjf;  

typedef struct gantt_psjf {
  int gantt_processes[MAX];
  int gantt_timestamps[MAX+1];
  int process_length;
} gantt_psjf;

void init_gantt_psjf(gantt_psjf* g, int init_start_time) {
  g->gantt_timestamps[0] = init_start_time;
}

void update_gantt_psjf(gantt_psjf* g, int pid, int end_time) {
  int ind = g->process_length;
  g->gantt_processes[ind] = pid;
  g->gantt_timestamps[ind+1] = end_time;
  g->process_length += 1;
}

void print_gantt_psjf(gantt_psjf g) {
  printf("Gantt Chart:\n");
  for (int i = 0; i < g.process_length; i++) {
    if (g.gantt_timestamps[i+1] == -1) {
      i++;
    }
    printf(".-------");
  }
  printf(".\n");

  for (int i = 0; i < g.process_length; i++)
    if (g.gantt_processes[i] == -1) {
      printf("| ----- ", g.gantt_processes[i]);
      if (g.gantt_timestamps[i+1] == -1) {
        i++;
      }
    } else {
      printf("| P%-5d", g.gantt_processes[i]);
    }
  printf("|\n");

  for (int i = 0; i < g.process_length; i++) {
    if (g.gantt_timestamps[i+1] == -1) {
      i++;
    } printf(":-------");
  }
  printf(":\n");

  printf("%-3d", g.gantt_timestamps[0]);
  for (int i = 0; i < g.process_length; i++) {
    if (g.gantt_timestamps[i+1] != -1) {
      printf("     %-3d", g.gantt_timestamps[i+1]);
    }
  }
  printf("\n");
}

void print_psjf(process_psjf* plist, int y) {
  for (int i=0; i < y; i++) {
    process_psjf p = plist[i];
    printf("P[%d]\n", p.pid);
    for (int i=0; i < p.i; i++) {
      printf("Start time: %d End time: %d\n", p.t_start[i], p.t_end[i]);
    }
    printf("Waiting time: %d\n", p.t_wait);
    printf("Turnaround time: %d\n", p.t_turnaround);
    printf("************************************\n");
  }
  float t_avg = 0.0;
  for (int i=0; i < y; i++) {
    t_avg += plist[i].t_wait;
  }
  t_avg /= y;
  printf("Average waiting time: %.1lf\n", t_avg);
}

int t_arrival_asc_psjf(const void* a, const void* b) {
  process_psjf* p1 = (process_psjf*) a;
  process_psjf* p2 = (process_psjf*) b;
  return p1->t_arrival - p2->t_arrival;
}

int t_wait_asc(const void* a, const void* b) {
  process_psjf* p1 = (process_psjf*) a;
  process_psjf* p2 = (process_psjf*) b;
  return p1->t_wait - p2->t_wait;
}

int get_total_burst(process_psjf* plist, int y) {
  int t_total_burst = 0;
  for (int i=0; i < y; i++) {
    t_total_burst += plist[i].t_burst;
  }
  return t_total_burst;
}

int ind(process_psjf* plist, int y, int p) {
  for (int i=0; i < y; i++) {
    if (plist[i].pid == p) return i;
  } return -1;
}

int ind_min_burst(process_psjf* plist, int y, int t_cur) {
  process_psjf min = {.pid = -1, .t_burst = 1e9};
  for (int i=0; i < y; i++) {
    if (plist[i].t_arrival <= t_cur && plist[i].t_burst > 0 && plist[i].t_burst < min.t_burst) {
      min = plist[i]; // process must have already arrived
    }
  }
  return ind(plist, y, min.pid);
}

void psjf(FILE *fp, int y) {
  process_psjf plist[y]; 
  gantt_psjf gc = { .process_length = 0 };
  for (int i=0; i < y; i++) {
    int a, b, c; fscanf(fp, "%d%d%d\n", &a, &b, &c);
    process_psjf p = { .pid = a, .t_arrival = b, .t_burst = c,
      .t_wait = 0, .t_turnaround = 0, .i = 0 };
    plist[i] = p;
  }

  // sort by arrival and populate p_incoming[] DAT
  qsort(plist, y, sizeof(process_psjf), t_arrival_asc_psjf);
  int t_max_arrival = plist[y-1].t_arrival;
  int t_total_burst = get_total_burst(plist, y);
  int T = max(t_total_burst, t_max_arrival + plist[y-1].t_burst);
  process_psjf p_incoming[T+1];
  for (int t=0; t <= T; t++) { // dat for p_incoming jobs
    p_incoming[t].pid = -1;
  }
  for (int i=0; i < y; i++) {
    p_incoming[plist[i].t_arrival] = plist[i];
  }

  // PSJF
  init_gantt_psjf(&gc, plist[0].t_arrival);
  int cur = 0;
  plist[0].t_start[0] = 0; // init
  for (int t=0; t < T; t++) {
    int t_cur = cur != -1 ? plist[cur].t_burst : 1e9; // check for existing process
    int t_incoming = p_incoming[t].t_burst;
    if (p_incoming[t].pid != -1 && t_incoming > 0 && t_incoming < t_cur) { // update current process
      if (cur != -1) { // there is an existing process
        plist[cur].t_end[plist[cur].i] = t, plist[cur].i++;
        update_gantt_psjf(&gc, plist[cur].pid, plist[cur].t_end[plist[cur].i-1]);
        cur = ind(plist, y, p_incoming[t].pid); 
        plist[cur].t_start[plist[cur].i] = t;
      } else { // no process - set new process
        cur = ind(plist, y, p_incoming[t].pid); 
        plist[cur].t_start[plist[cur].i] = t;
        update_gantt_psjf(&gc, -1, plist[cur].t_start[plist[cur].i]);
      }
    }
    if (cur != -1) {
      plist[cur].t_burst--; // live time update
    }
    
    if (plist[cur].t_burst == 0) {
      plist[cur].t_end[plist[cur].i] = t + 1;
      update_gantt_psjf(&gc, plist[cur].pid, plist[cur].t_end[plist[cur].i]);
      plist[cur].i++;
      cur = ind_min_burst(plist, y, t);
      if (cur != -1) {
        plist[cur].t_start[plist[cur].i] = t + 1; 
      }
    }
  }

  // t_wait
  for (int i=0; i < y; i++) {
    plist[i].t_wait = plist[i].t_start[0] - plist[i].t_arrival;
    for (int j=1; j < plist[i].i; j++) {
      plist[i].t_wait += plist[i].t_start[j] - plist[i].t_end[j-1];
    }
  }
  
  // t_turnaround
  for (int i=0; i < y; i++) {
    plist[i].t_turnaround = plist[i].t_wait;
    for (int j=0; j < plist[i].i; j++) {
      plist[i].t_turnaround += plist[i].t_end[j] - plist[i].t_start[j];
    }
  }

  // sort by waiting time and print
  qsort(plist, y, sizeof(process_psjf), t_wait_asc);
  print_psjf(plist, y);
  print_gantt_psjf(gc);
}