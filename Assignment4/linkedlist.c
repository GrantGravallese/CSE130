// SOURCE 1: https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
// SOURCE 2: https://www.includehelp.com/ds/singly-linked-list-implementation-in-c.aspx
// Modified by: Dylan Yong

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "linkedlist.h"

//initialize list
void init_list(struct list *lp) {
    lp->head = NULL;
    lp->tail = NULL;
}

//create a node for the list
struct node *create_node(thread_t *t, bool meta_flag) {
    struct node *temp_node;
    temp_node = (struct node *) malloc(sizeof(struct node));
    temp_node->data = t;
    temp_node->next = NULL;
	if(!meta_flag) {
		temp_node->meta = NULL;
	}
	else {
		temp_node->meta = meta_init();
	}

    return temp_node;
}

//initialize the meta data struct
struct metadata *meta_init() {
	struct metadata * temp_meta;
	temp_meta = (struct metadata *) malloc(sizeof(struct metadata));
	temp_meta->turn_start = 0;
	temp_meta->turn_end = 0;
	temp_meta->turn_final = 0;
	temp_meta->wait_start = 0;
	temp_meta->wait_end = 0;
	temp_meta->wait_final = 0;
	temp_meta->cpu_final = 0;
	return temp_meta;
}

//display the list's tids
void print_list(struct list *lp) {
   struct node *ptr = lp->head;
	
   //start from the beginning
   while(ptr != NULL) {
      printf(" (%d) ",ptr->data->tid);
      ptr = ptr->next;
   }
   printf("\n");
}

//check if list is empty
bool is_empty(struct list *lp) {
   return lp->head == NULL;
}

//return the size of the list
int get_size(struct list *lp) {
   int counter = 0;
   struct node *current;
	
   for(current = lp->head; current != NULL; current = current->next) {
      counter++;
   }
	
   return counter;
}

//Add new item at the end of list.
void push_at_tail(struct list *lp, thread_t *data, bool meta_flag) {
	struct node *new_node;
	new_node = create_node(data, meta_flag);

	//if list is empty.
	if(lp->head == NULL) {
		lp->head = new_node;
		lp->tail = new_node;
	}
	else {
		lp->tail->next  = new_node;
		lp->tail = lp->tail->next;
	}		
}

//Delete item from the end of list.
thread_t *pop_from_tail(struct list *lp) {
	struct node *temp;
	thread_t *data = NULL;

	if(lp->tail == NULL) {	
		return NULL;
	}
	else {
		temp = lp->head;

		while(temp->next != lp->tail) { temp = temp->next;} // traverse list until temp next is equal to tail

		data = lp->tail->data;

		lp->tail = temp; 
		lp->tail->next = NULL;		
	}	

	return data;
}

//Add new item at begning of the list.
void push_at_head(struct list *lp, thread_t *data, bool meta_flag) {
	struct node *new_node;
	new_node = create_node(data, meta_flag);

	//if list is empty.
	if(lp->head == NULL) {
		lp->head = new_node;
		lp->tail = new_node;
	}
	else {
		new_node->next = lp->head;
		lp->head = new_node;
	}		
}

//Delete item from Start of list.
thread_t *pop_from_head(struct list *lp) {
	thread_t *data = NULL;

	if(lp->head == NULL) {	
		return NULL;
	}
	else {
		data = lp->head->data;
		lp->head = lp->head->next;
	}	

	return data;
}

//find a link with given key
struct node *find(struct list *lp, thread_t *key) {
   struct node* current = lp->head; // start from the first link
   if(lp->head == NULL) { return NULL; } // if list is empty return null

   //navigate through list
   while(current->data != key) {
      if(current->next == NULL) { return NULL; } // if its the last node return null
	  else { current = current->next; } // else go to next link
   }      
  
   return current; //if data found, return the current Link
}

//all of the sorting functions below use bubble sort

//this function will sort the list by thread tid
void tid_sort(struct list *lp) {

   int i, j, k;
   struct metadata *temp_meta;
   thread_t *temp_data;
   struct node *current;
   struct node *next;
	
   int size = get_size(lp);
   k = size;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = lp->head;
      next = lp->head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data->tid > next->data->tid ) {
            temp_data = current->data;
            current->data = next->data;
            next->data = temp_data;

            temp_meta = current->meta;
            current->meta = next->meta;
            next->meta = temp_meta;
         }
			
         current = current->next;
         next = next->next;
      }
   }   
}

//this function will sort the list by thread priority
void priority_sort(struct list *lp) {

   int i, j, k;
   struct metadata *temp_meta;
   thread_t *temp_data;
   struct node *current;
   struct node *next;
	
   int size = get_size(lp);
   k = size;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = lp->head;
      next = lp->head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data->priority > next->data->priority ) {
            temp_data = current->data;
            current->data = next->data;
            next->data = temp_data;

            temp_meta = current->meta;
            current->meta = next->meta;
            next->meta = temp_meta;
         }
			
         current = current->next;
         next = next->next;
      }
   }   
}

//this function will sort the list by thread length
void length_sort(struct list *lp) {

   int i, j, k;
   struct metadata *temp_meta;
   thread_t *temp_data;
   struct node *current;
   struct node *next;
	
   int size = get_size(lp);
   k = size;
	
   for ( i = 0 ; i < size - 1 ; i++, k-- ) {
      current = lp->head;
      next = lp->head->next;
		
      for ( j = 1 ; j < k ; j++ ) {   

         if ( current->data->length > next->data->length ) {
            temp_data = current->data;
            current->data = next->data;
            next->data = temp_data;

            temp_meta = current->meta;
            current->meta = next->meta;
            next->meta = temp_meta;
         }
			
         current = current->next;
         next = next->next;
      }
   }   
}

//this function will sort the list by thread length left
void remainder_sort(struct list *lp, struct list *ref) {

	int i, j, k, current_use, next_use;
	struct metadata *temp_meta;
	thread_t *temp_data;
	struct node *current;
	struct node *next;
	struct node *current_ref;
	struct node *next_ref;
		
	int size = get_size(lp);
	k = size;
   	for ( i = 0 ; i < size - 1 ; i++, k-- ) {
		current = lp->head;
		next = lp->head->next;
		
		for ( j = 1 ; j < k ; j++ ) {   
			current_ref = find(ref, current->data);
			next_ref = find(ref, next->data);
			current_use = current_ref->meta->cpu_final;
			next_use = next_ref->meta->cpu_final;
			if ( (current->data->length-current_use) > (next->data->length-next_use) ) {
				temp_data = current->data;
				current->data = next->data;
				next->data = temp_data;

				temp_meta = current->meta;
				current->meta = next->meta;
				next->meta = temp_meta;
			}
				
			current = current->next;
			next = next->next;
		}
  	}  
}

void set_turn_start(struct list *lp, thread_t *t, unsigned int value) {
	struct node *temp;
	temp = find(lp, t);
	temp->meta->turn_start = value;
}
// 0 1 2 3 4 5 6 7 8
// * * * * * * (length 6 [5-0] but calculated as [6-0] start at 0 end up at 6)

void set_turn_end(struct list *lp, thread_t *t, unsigned int value) {
	struct node *tempnode;
	unsigned int start = 0, end = 0;

	tempnode = find(lp, t);
	tempnode->meta->turn_end = value;
	start = tempnode->meta->turn_start;
	end = tempnode->meta->turn_end;

	tempnode->meta->turn_final += (end-start);
}

// waiting timing
// 0 1 2 3 4 5 6 7 8 9
//     * * * * 
// start on 2, end on 6 2+4(jumps) = 6
// start on 2 and make 4 moves forward, end up on 6
void set_wait_start(struct list *lp, thread_t *t, unsigned int value) {
	struct node *tempnode;
	tempnode = find(lp, t);
	tempnode->meta->wait_start = value;
}

void set_wait_end(struct list *lp, thread_t *t, unsigned int value) {
	struct node *tempnode;
	unsigned int start = 0, end = 0, diff = 0;

	tempnode = find(lp, t);
	tempnode->meta->wait_end = value;
	start = tempnode->meta->wait_start;
	end = tempnode->meta->wait_end;
	diff = end-start;

	if(diff > 0) { tempnode->meta->wait_final += diff; } // if waiting time is greater than 0, add to waiting time total
	
	tempnode->meta->wait_start = 0;
	tempnode->meta->wait_end = 0;
}

void increment_cpu(struct list *lp, thread_t *t) {
	struct node *tempnode;

	tempnode = find(lp, t);
	tempnode->meta->cpu_final++;
}

// //delete a link with given key
// struct node* delete(int key) {

//    //start from the first link
//    struct node* current = head;
//    struct node* previous = NULL;
	
//    //if list is empty
//    if(head == NULL) {
//       return NULL;
//    }

//    //navigate through list
//    while(current->key != key) {

//       //if it is last node
//       if(current->next == NULL) {
//          return NULL;
//       } else {
//          //store reference to current link
//          previous = current;
//          //move to next link
//          current = current->next;
//       }
//    }

//    //found a match, update the link
//    if(current == head) {
//       //change first to point to next link
//       head = head->next;
//    } else {
//       //bypass the current link
//       previous->next = current->next;
//    }    
	
//    return current;
// }



// void reverse(struct node** head_ref) {
//    struct node* prev   = NULL;
//    struct node* current = *head_ref;
//    struct node* next;
	
//    while (current != NULL) {
//       next  = current->next;
//       current->next = prev;   
//       prev = current;
//       current = next;
//    }
	
//    *head_ref = prev;
// }