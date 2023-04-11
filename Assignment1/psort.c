/************************************************************************
 * 
 * CSE130 Assignment 1
 *  
 * POSIX Shared Memory Multi-Process Merge Sort
 * 
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/

 /*
Resources:
https://stackoverflow.com/questions/21387349/how-does-the-fork-know-whether-it-is-in-child-process-and-in-parent-process
https://stackoverflow.com/questions/32165540/copy-a-structure-to-a-shared-memory-in-c-using-posix-standard
https://stackoverflow.com/questions/3902215/using-memcpy-to-copy-a-range-of-elements-from-an-array
https://man7.org/linux/man-pages/man2/shmget.2.html
https://linuxhint.com/posix-vs-unix-understanding-the-difference/
https://man7.org/linux/man-pages/man3/ftok.3.html
https://www.gnu.org/software/libc/manual/html_node/Process-Identification.html
https://man7.org/linux/man-pages/man2/wait.2.html
https://man7.org/training/download/lusp_pshm_slides.pdf
http://logan.tw/posts/2018/01/07/posix-shared-memory/
https://www.geeksforgeeks.org/posix-shared-memory-api/
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "merge.h"
#define NAME "/shmem-example"

/* 
 * Merge Sort in the current process a sub-array of ARR[] defined by the 
 * LEFT and RIGHT indexes.
 */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * Merge Sort in the current process and at least one child process a 
 * sub-array of ARR[] defined by the LEFT and RIGHT indexes.
 */
void multiProcessMergeSort(int arr[], int left, int right){
  
  //check if left and right are valid
  if(left >= right) exit(EXIT_FAILURE);

  //create a middle variable
  int middle = ( (left+right)/2 );
  
  //create shared memory
  //ID is based on the size of the local memory (so use RIGHT index)
  //int shmid = shmget( IPC_PRIVATE, (right+1)*sizeof(int), 0666 );

  int shmid = shm_open( NAME, O_CREAT | O_EXCL | O_RDWR | S_IRUSR | S_IWUSR, 0600);
  if (shmid < 0) exit(EXIT_FAILURE);

  ftruncate(shmid, (right+1)*sizeof(int));
  
  //attach to shared memory
  //int *shm = ( (int*) shmat(shmid, (void*)0, 0) );
  int *shm = (int *)mmap(0, (right+1)*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);

  //copy local memory to shared memory
  memcpy( shm, arr, (right+1)*sizeof(int) );

  //fork
  switch( fork() ){

    //if error (fork is not 0 or greater than 0), exit
    case -1:
      printf("Error: Fork() failed");
      exit(EXIT_FAILURE);
    
    //if child
    case 0:

      //sort right side of shared memory (will be from middle+1 to right)
      singleProcessMergeSort( shm, middle+1, right );

      //exit
      exit(EXIT_SUCCESS);
      

    //if parent
    default:

      //sort left side of shared memory
      singleProcessMergeSort( shm, left, middle );

      //wait for child to finish
      wait(NULL);

      //merge shared memory
      merge(shm, left, middle, right);

      //copy shared memory to local memory
      memcpy( arr, shm, (right+1)*sizeof(int) );


      //detach from shared memory
      //shmdt( shm );
      munmap(shm, (right+1)*sizeof(int));

      //destroy shared memory
      //shmctl( shmid, IPC_RMID, NULL);
      shm_unlink(NAME);
      close(shmid);

    }

return;
}

