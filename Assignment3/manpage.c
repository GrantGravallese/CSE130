/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/
//REFERENCES:
//https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
//https://www.cplusplus.com/reference/mutex/mutex/lock/
//https://docs.oracle.com/cd/E19455-01/806-5257/sync-12/index.html


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include "manpage.h"

#define THREADNUM 7
/*
 * Create a thread for each of seven manpage paragraphs and have them synchronize
 * their invocations of showParagraph() so the manual page displays as follows:
 *
 --------- 8< ---------------------------------------------------
 
A semaphore S is an unsigned-integer-valued variable.
Two operations are of primary interest:
 
P(S): If processes have been blocked waiting on this semaphore,
 wake one of them, else S <- S + 1.
 
V(S): If S > 0 then S <- S - 1, else suspend execution of the calling process.
 The calling process is said to be blocked on the semaphore S.
 
A semaphore S has the following properties:

1. P(S) and V(S) are atomic instructions. Specifically, no
 instructions can be interleaved between the test that S > 0 and the
 decrement of S or the suspension of the calling process.
 
2. A semaphore must be given an non-negative initial value.
 
3. The V(S) operation must wake one of the suspended processes. The
 definition does not specify which process will be awakened.

 --------- 8< ---------------------------------------------------
 *
 * As supplied, shows random single messages.
 */

pthread_mutex_t theMutex;

int count = 0;

void* threadParagraph(void* args){
 
  //get paragraph id
  int pid = getParagraphId(); // pid = 'Paragraph Id"
  
  //infinite while loop
  while(1){
    
    //wait for the mutex lock
    pthread_mutex_lock(&theMutex);

    //if the pid matches the counter, enter logic
    if(pid == count){
      
      //show the sentence
      showParagraph();

      //increment the count
      count++;

      //unlock the mutex thread
      pthread_mutex_unlock(&theMutex);
      
      //exit
      pthread_exit(0);

    }
    //otherwise, unlock the mutex and wait
    pthread_mutex_unlock(&theMutex);
  }
}

void manpage() 
{ 
  //initialize the mutex
  pthread_mutex_init(&theMutex, 0);
  
  //initialize array of threads
  pthread_t threadArr[THREADNUM];

  //initialize the mutex, exiting if there is a failure
  if (pthread_mutex_init(&theMutex, 0) != 0 ) exit(EXIT_FAILURE);

  //create the pthreads using pthread_create and the initialization function
  for (int i=0; i<THREADNUM;i++) pthread_create(&threadArr[i], 0, threadParagraph, 0);

  //wait for the pthreads using pthread_join
  for (int i=0; i<THREADNUM;i++) pthread_join(threadArr[i], 0);

  //destroy the mutex
  pthread_mutex_destroy(&theMutex);

  //return
  return;
}
