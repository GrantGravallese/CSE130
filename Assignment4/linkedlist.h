// SOURCE 1: https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
// SOURCE 2: https://www.includehelp.com/ds/singly-linked-list-implementation-in-c.aspx
// Modified by: Dylan Yong

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "simulator.h"
#include "scheduler.h"

//struct contains information related to a specific thread
struct metadata {
    unsigned int turn_start;
    unsigned int turn_end;
    unsigned int turn_final;
    unsigned int wait_start;
    unsigned int wait_end;
    unsigned int wait_final;
    unsigned int cpu_final;
};

//contains a thread, metadata of the thread and a next node pointer
struct node {
    thread_t *data;
    struct metadata *meta;
    struct node *next;
};

//contains alist
struct list {
    struct node *head;
    struct node *tail;
};

//initalize a new linked list
void init_list(struct list *lp);

//create a node and return its pointer
struct node *create_node(thread_t *t, bool meta_flag);

//create meta data struct
struct metadata *meta_init();

//display the list
void print_list(struct list *lp);

//is list empty
bool is_empty(struct list *lp);

//Add new item at the end of list.
void push_at_tail(struct list *lp, thread_t *data, bool meta_flag);

//Delete item from the end of list.
thread_t *pop_from_tail(struct list *lp);

//Add new item at begning of the list.
void push_at_head(struct list *lp, thread_t *data, bool meta_flag);

//Delete item from Start of list.
thread_t *pop_from_head(struct list *lp);

//find a link with given key
struct node *find(struct list *lp, thread_t *data);

//sort list by tid
void tid_sort(struct list *lp);

//this function will sort the list by thread priority
void priority_sort(struct list *lp);

//this function will sort the list by thread length
void length_sort(struct list *lp);

//this function will sort the list by thread length left
void remainder_sort(struct list *lp, struct list *ref);

//return size of list
int get_size(struct list *lp);

//return metadata of list
struct metadata *get_metadata(struct list *lp, thread_t *t);

//set the start of a turnaround period
void set_turn_start(struct list *lp, thread_t *t, unsigned int value);

//set the end of a turnaround period
void set_turn_end(struct list *lp, thread_t *t, unsigned int value);

//set start of waiting period
void set_wait_start(struct list *lp, thread_t *t, unsigned int value);

//set end of waiting period
void set_wait_end(struct list *lp, thread_t *t, unsigned int value);

//increment cpu usage
void increment_cpu(struct list *lp, thread_t *t);


// ********* MAY ADD IN LATER ASSIGNMENTS **********

// //delete a link with given key
// struct node* delete(int key);

// void reverse(struct node** head_ref);
