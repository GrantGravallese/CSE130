/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>

#include <stdio.h>

#include "simulator.h"
#include "scheduler.h"
#include "linkedlist.h"

#define DEBUG 1

struct list *ready_queue;
struct list *thread_dict;
int quantum_counter, quantum_limit;
thread_t *running_thread;
enum algorithm alg;

void scheduler(enum algorithm algorithm, unsigned int quantum) { 
  ready_queue = (struct list *) malloc(sizeof(struct list));
  init_list(ready_queue);

  thread_dict = (struct list *) malloc(sizeof(struct list));
  init_list(thread_dict);
  
  quantum_counter = 0;
  quantum_limit = quantum;
  running_thread = NULL;
  alg = algorithm;
}

void tick() { // check for quantum limits
  thread_t *thr_next;
  struct node *running_ref;
  struct node *next_ref;
  int running_use, next_use;
  switch(alg) {
    case ROUND_ROBIN:
      if(quantum_counter >= quantum_limit && running_thread != NULL) {
        // add current thread to ready list
        push_at_tail(ready_queue, running_thread, false); // push thread to tail of ready queue
        set_wait_start(thread_dict, running_thread, sim_time());
        running_thread = NULL;
        
        // if ready list is not empty, pop front off
        if(!is_empty(ready_queue)) {
          thr_next = pop_from_head(ready_queue);
          set_wait_end(thread_dict, thr_next, sim_time());
          sim_dispatch(thr_next);
          running_thread = thr_next;
          quantum_counter = 0;
        }
      }
      break;
    case NON_PREEMPTIVE_PRIORITY:
      priority_sort(ready_queue);
      break;
    case PREEMPTIVE_PRIORITY:
      priority_sort(ready_queue);
      if(!is_empty(ready_queue) && running_thread != NULL) {
        thr_next = ready_queue->head->data;
        // if top of ready queue has a higher priority than current thread, preempt it
        if(thr_next->priority < running_thread->priority) {
          pop_from_head(ready_queue);
          push_at_tail(ready_queue, running_thread, false); // push thread to tail of ready queue
          set_wait_start(thread_dict, running_thread, sim_time());
          running_thread = NULL;

          set_wait_end(thread_dict, thr_next, sim_time());
          sim_dispatch(thr_next);
          running_thread = thr_next;
          quantum_counter = 0;
          
          priority_sort(ready_queue);
        }
      }
      break;
    case NON_PREEMPTIVE_SHORTEST_JOB_FIRST:
      length_sort(ready_queue);
      break;
    case PREEMPTIVE_SHORTEST_JOB_FIRST:
      length_sort(ready_queue);
      if(!is_empty(ready_queue) && running_thread != NULL) {
        thr_next = ready_queue->head->data;
        // if top of ready queue has a higher priority than current thread, preempt it
        if(thr_next->length < running_thread->length) {
          pop_from_head(ready_queue);
          push_at_tail(ready_queue, running_thread, false); // push thread to tail of ready queue
          set_wait_start(thread_dict, running_thread, sim_time());
          running_thread = NULL;

          set_wait_end(thread_dict, thr_next, sim_time());
          sim_dispatch(thr_next);
          running_thread = thr_next;
          quantum_counter = 0;
          
          length_sort(ready_queue);
        }
      }
      break;
    case NON_PREEMPTIVE_SHORTEST_REMAINING_TIME_FIRST:
      remainder_sort(ready_queue, thread_dict);
      break;
    case PREEMPTIVE_SHORTEST_REMAINING_TIME_FIRST:
      remainder_sort(ready_queue, thread_dict);
      if(!is_empty(ready_queue) && running_thread != NULL) {
        thr_next = ready_queue->head->data;
        running_ref = find(thread_dict, running_thread);
        next_ref = find(thread_dict, thr_next);
        running_use = running_ref->meta->cpu_final;
        next_use = next_ref->meta->cpu_final;
        // if top of ready queue has a higher priority than current thread, preempt it
        if((thr_next->length-next_use) < (running_thread->length-running_use)) {
          pop_from_head(ready_queue);
          push_at_tail(ready_queue, running_thread, false); // push thread to tail of ready queue
          set_wait_start(thread_dict, running_thread, sim_time());
          running_thread = NULL;

          set_wait_end(thread_dict, thr_next, sim_time());
          sim_dispatch(thr_next);
          running_thread = thr_next;
          quantum_counter = 0;
          
          remainder_sort(ready_queue, thread_dict);
        }
      }

      break;
    default:
      
      break;
  }
  // run the next thread in the waiting list if cpu inactive
  if(running_thread == NULL) {
    if(!is_empty(ready_queue)) {
      thr_next = pop_from_head(ready_queue);
      set_wait_end(thread_dict, thr_next, sim_time());
      sim_dispatch(thr_next);
      running_thread = thr_next;
      quantum_counter = 0;
    }
  }
  quantum_counter++;
}

void sim_tick() { 
  if(running_thread != NULL) {
    find(thread_dict, running_thread)->meta->cpu_final++;
  }
}

// user function
void sys_exec(thread_t *thr) { 
  push_at_tail(thread_dict, thr, true); // push thread to thread dictionary
  set_turn_start(thread_dict, thr, sim_time()); // set turnaround start time

  push_at_tail(ready_queue, thr, false); // push thread to tail of ready queue
  set_wait_start(thread_dict, thr, sim_time()); // set waiting start time
}

// user function
void sys_read(thread_t *thr) { 
  set_wait_start(thread_dict, thr, sim_time()+1); // set waiting start time
  find(thread_dict, running_thread)->meta->cpu_final++;
  running_thread = NULL;
  
}

// user function
void sys_write(thread_t *thr) {
  set_wait_start(thread_dict, thr, sim_time()+1); // set waiting start time
  find(thread_dict, running_thread)->meta->cpu_final++;
  running_thread = NULL;
}

// user function
void sys_exit(thread_t *thr) {
  set_turn_end(thread_dict, thr, sim_time()+1); //set turnaround end time
  find(thread_dict, running_thread)->meta->cpu_final++;
  running_thread = NULL;
}

// user function
void io_complete(thread_t *thr) {
  set_wait_start(thread_dict, thr, sim_time()+1); // set waiting start time
  push_at_tail(ready_queue, thr, false); // add thread to ready list
}

// user function
void io_starting(thread_t *thr) {
  set_wait_end(thread_dict, thr, sim_time());
}

stats_t *stats() { 
  int thread_count = get_size(thread_dict); 
  stats_t *stats = malloc(sizeof(stats_t));
  stats->tstats = malloc(sizeof(stats_t)*thread_count);

  tid_sort(thread_dict);

  struct node *current = thread_dict->head;
  int total_wait = 0, total_turn = 0;

  for(int i=0; i<thread_count; i++) {
    total_wait += current->meta->wait_final;
    total_turn += current->meta->turn_final;

    stats->tstats[i].tid = current->data->tid;
    stats->tstats[i].waiting_time = current->meta->wait_final;
    stats->tstats[i].turnaround_time = current->meta->turn_final;
    current = current->next;
  }
  stats->waiting_time = total_wait/thread_count;
  stats->turnaround_time = total_turn/thread_count;
  stats->thread_count = thread_count;
  return stats; 
}