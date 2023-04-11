//RESOURCES:
//https://stackoverflow.com/questions/3466242/multithreaded-merge-sort
//https://www.geeksforgeeks.org/merge-sort-using-multi-threading/
//https://codereview.stackexchange.com/questions/261456/multi-threaded-merge-sort
//https://malithjayaweera.com/2019/02/parallel-merge-sort/
//https://www.geeksforgeeks.org/thread-functions-in-c-c/#:~:text=In%20a%20Unix%2FLinux%20operating,for%20all%20thread%20related%20functions.&text=It%20is%20most%20effective%20on,achieving%20the%20speed%20of%20execution.
//https://man7.org/linux/man-pages/man7/pthreads.7.html
//https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

// Grant Gravallese 1704019 ggravall
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "merge.h"

//dummy argument structure for passing array and bounds for sorting and merging
struct dummyStruct {
  int *arr;
  int left;
  int right;
  int middle;
};

//function for performing single threaded merge sort within a thread, using pthread_create
void *threadMergeSort(void *args) {
  struct dummyStruct *theArgs = args;
  int *arr = theArgs->arr;
  int left = theArgs->left;
  int right = theArgs->right;
  singleThreadedMergeSort(arr, left, right);  
  pthread_exit(0);
}

// function for merging within a thread, using pthread_create
void *threadMerge(void *args) {
  struct dummyStruct *theArgs = args;
  int *arr = theArgs->arr;
  int left = theArgs->left;
  int middle = theArgs->middle;
  int right = theArgs->right;
  merge(arr, left, middle, right);
  pthread_exit(0);
}

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

void multiThreadedMergeSort(int arr[], int left, int right) 
{
  //create three threads
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;
  
  //split the array into different parts using the indecies: left, leftMiddle, middle, rightMiddle, right
  int middle = (left + (right-left)/2);
  int leftMiddle = (left + (middle-left)/2);
  int rightMiddle = (middle + (right-middle)/2);

  
  //sort the array with three threads, passing values to pthread_create using dummy structures.
  
  //first, sort the left middle to middle
  struct dummyStruct dummy1;
  dummy1.arr = arr;
  dummy1.left = leftMiddle + 1;
  dummy1.right = middle;
  pthread_create(&thread1, NULL, threadMergeSort, (void*) &dummy1);
  
  //then the middle to right middle
  struct dummyStruct dummy2;
  dummy2.arr = arr;
  dummy2.left = middle + 1;
  dummy2.right = rightMiddle;
  pthread_create(&thread2, NULL, threadMergeSort, (void*) &dummy2);

  //then right middle to right
  struct dummyStruct dummy3;
  dummy3.arr = arr;
  dummy3.left = rightMiddle + 1;
  dummy3.right = right;
  pthread_create(&thread3, NULL, threadMergeSort, (void*) &dummy3);
  
  //sort the left and leftMiddle using the main process
  singleThreadedMergeSort(arr, left, leftMiddle);

  //wait for threads 1, 2, and 3 to finish
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);

  //merge the right middle and right using T2
  dummy2.left = middle+1;
  dummy2.middle = rightMiddle;
  dummy2.right = right;
  pthread_create(&thread2, NULL, threadMerge, (void*) &dummy2);

  //process merges left left and left right
  merge(arr, left, leftMiddle, middle);

  //wait for T2 to finish
  pthread_join(thread2, NULL);

  //merge the two halves of the array
  merge(arr, left, middle, right);

}