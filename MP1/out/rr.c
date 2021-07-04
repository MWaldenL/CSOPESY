#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct process {
  int pid, t_arrival, t_burst;
  int t_start[101], t_end[101], t_wait, t_turnaround;
  int arrived, last_ind;
} process;

typedef struct node {
  struct process* proc;
  struct node* next;
} node;

typedef struct queue {
  struct node* front;
  struct node* rear;
} queue;

typedef struct gantt_rr {
  int gantt_processes[MAX];
  int gantt_timestamps[MAX+1];
  int process_length;
} gantt_rr;

void init_gantt_rr(gantt_rr* g, int init_start_time) {
  g->gantt_timestamps[0] = init_start_time;
}

void update_gantt_rr(gantt_rr* g, int pid, int end_time) {
  int ind = g->process_length;
  g->gantt_processes[ind] = pid;
  g->gantt_timestamps[ind+1] = end_time;
  g->process_length += 1;
}

void print_gantt_rr(gantt_rr g) {
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

void init_queue(queue *q) {
  q->front = NULL;
  q->rear = NULL;
}

int is_empty(queue *q) {
  return (q->front == NULL);
}

void enqueue(queue *q, process *proc) {
  node* tmp;
  tmp = malloc(sizeof(node));
  tmp->proc = proc;
  tmp->next = NULL;
  if(!is_empty(q)) {
    node *old_r = q->rear;
    old_r->next = tmp;
    q->rear = tmp;
  } else {
    q->front = q->rear = tmp;
  }
}

process* dequeue(queue *q) {
  node* tmp;
  tmp = q->front;
  q->front = tmp->next;
  return tmp->proc;
}

void print(process* plist, int y) {
  for (int i=0; i < y; i++) {
    process p = plist[i];
    printf("P[%d]\n", p.pid);
    for (int i=0; i < p.last_ind; i++) {
      printf("Start time: %d End time: %d\n", p.t_start[i], p.t_end[i]);
    }
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

int t_arrival_asc(const void* a, const void* b) {
  process* p1 = (process*) a;
  process* p2 = (process*) b;
  int c = p1->t_arrival - p2->t_arrival;
  if (c == 0)
    return 1; // stabilize qsort
  else
    return c;
}

void update_arriving_processes(process *plist, queue* rq, int y, int t_curr) {
  for (int i = 0; i < y; i++) {
    if (plist[i].t_arrival <= t_curr && plist[i].arrived == 0) {
      plist[i].arrived = 1;
      enqueue(rq, &plist[i]);
    }
  }
}

void rr(FILE* fp, int y, int z) {
  queue* rq;
  rq = malloc(sizeof(queue));
  init_queue(rq);
  process plist[y]; 
  gantt_rr gc = { .process_length = 0 };

  for (int i = 0; i < y; i++) {
    int a, b, c; fscanf(fp, "%d%d%d", &a, &b, &c);
    process p = { .pid = a, .t_arrival = b, .t_burst = c, .t_wait = 0, 
      .t_turnaround = 0, .arrived = 0, .last_ind = 0};
    plist[i] = p;
  }

  qsort(plist, y, sizeof(process), t_arrival_asc);
  int t_curr = plist[0].t_arrival;
  init_gantt_rr(&gc, t_curr);

  update_arriving_processes(plist, rq, y, t_curr);

  while (!is_empty(rq)) {
    process* curr_process = dequeue(rq);
    int p_ind = curr_process->last_ind;

    // update starting time of process at ith return
    curr_process->t_start[p_ind] = t_curr;
    // check if burst fits time quantum
    if (curr_process->t_burst > z) {
      // interrupt the current process after z time quantum
      curr_process->t_end[p_ind] = t_curr + z;
      // reduce burst
      curr_process->t_burst -= z;
    } else {
      // release after completing burst
      curr_process->t_end[p_ind] = t_curr + curr_process->t_burst;
      // reduce burst
      curr_process->t_burst = 0;
    }
    // update process waiting time
    curr_process->t_wait += p_ind == 0 ? t_curr - curr_process->t_arrival : t_curr - curr_process->t_end[p_ind-1];
    // update turnaround
    curr_process->t_turnaround = curr_process->t_end[p_ind] - curr_process->t_arrival;
    // move current time to end of process
    t_curr = curr_process->t_end[p_ind];
    // increment current index
    curr_process->last_ind += 1;

    update_gantt_rr(&gc, curr_process->pid, curr_process->t_end[p_ind]);

    // update ready queue with new arriving processes
    update_arriving_processes(plist, rq, y, t_curr);
    
    if (curr_process->t_burst > 0)
      enqueue(rq, curr_process);

    if (is_empty(rq)) {
      for (int i = 0; i < y; i++) {
        if (plist[i].arrived == 0) {
          t_curr = plist[i].t_arrival;
          update_gantt_rr(&gc, -1, t_curr);
          update_arriving_processes(plist, rq, y, t_curr);
          i = y;
        }
      }
    }
  }

  print(plist, y);
  print_gantt_rr(gc);
}

