/************************************************************************
 *
 * CSE130 Assignment 5
 *
 * Copyright (C) 2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 * 
 ************************************************************************/

#include "simulator.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>



//initialize a page struct to add to the queue
typedef struct enrichedPageStruct{
	unsigned int e_page;
	int usageCount;
	int timeLastUsed;
	int referenceBit;
}enriched_p;

void *myQueue;
unsigned int myFrames;
enum algorithm theAlgorithm;
int algorithmCase;

int mallocNum = 0;
int freeNum = 0;

// //For testing by printing queue
// static bool printCompare(void *outer, void *inner){
// 	//printf("Queue page: %d, Time Priority #%d, Usage Priority %d, Reference Bit %d, Inserting page: %d\n",((enriched_p*) outer)->e_page, ((enriched_p*) outer)->timeLastUsed, ((enriched_p*) outer)->usageCount, ((enriched_p*) outer)->referenceBit, *((unsigned int *)inner)); 
// 	return false;
// }

//create comparitor func to compare the pages themselves
static bool pageCompare(void *outer, void *inner) {
	//printf("Bool: %d\n", (((enriched_p*)outer)->e_page) == *(unsigned int*)inner);

	return (((enriched_p*) outer)->e_page == *((unsigned int *)inner));
}

//comparitor func to compare the times the pages were created
static int timeCompare(void *a, void *b) {
	//printf("Queue pageTime: %d, page #%d\n CompareTime: %d\n",((enriched_p*) a)->timeLastUsed, ((enriched_p*) a)->e_page, *((int *)b)); 

	return (((enriched_p*) a)->timeLastUsed - ((enriched_p*) b)->timeLastUsed );
	
}

//comparitor func to compare the amount of times the pages are used
static int usageCountCompare(void *a, void *b) {
	//printf("A Usage: %d, B Usage: %d\n",((enriched_p*) a)->usageCount, ((enriched_p*) b)->usageCount);
	//printf("Bool: %d\n", (((enriched_p*)outer)->e_page) == *(unsigned int*)inner);

	return (((enriched_p*) a)->usageCount - ((enriched_p*) b)->usageCount );
}



/**
 * Initialise your ALGORITHM pager with FRAMES frames (physical pages).
 */
 //store the number of frames, queue size is according to frame number
void pager_init(enum algorithm algorithm, unsigned int frames) {
	//printf("inside pager_init\n");
	
	//set algorithm type for use in pager_request
	theAlgorithm = algorithm;

	myQueue = queue_create();
	//if (myQueue == NULL) printf("QUEUE IS NULL, QUEUE_CREATE FAILED\n");

	//printf("queueSize is %d\n", frames);
	if (theAlgorithm == FIRST_IN_FIRST_OUT){
		//printf("CASE FIFO\n");
		myFrames = frames;
		algorithmCase = 0;

	}
	else if (theAlgorithm == LEAST_RECENTLY_USED){
		//printf("CASE LRU\n");
		myFrames = frames;
		algorithmCase = 1;
	}
	else if (theAlgorithm == LEAST_FREQUENTLY_USED){
		//printf("CASE LFU\n");
		myFrames = frames;
		algorithmCase = 2;
	}
	else if (theAlgorithm == SECOND_CHANCE){
		//printf("CASE SECOND CHANCE\n");
		myFrames = frames;
		algorithmCase = 3;
	}
	else if (theAlgorithm == CIRCULAR_QUEUE){
		//printf("CASE CIRCULAR QUEUE\n");
		myFrames = frames;
		algorithmCase = 4;
	}


}

/**
 * Free any dynamically allocated resources.
 */
void pager_destroy() {

	for(int i = 0; i <= queue_size(myQueue); i++){

		freeNum++;
		printf("freeing, %d frees so far\n",freeNum);
		free(queue_dequeue(myQueue));
		
	}
	queue_destroy(myQueue);
	
}

/**
 * A request has been made for virtual page PAGE. If your pager does
 * not currently have PAGE in physical memory, request it from the 
 * simulator.
 */
void pager_request(unsigned int page) {

	//printf("entered pager_request()\n");

	mallocNum++;
	printf("mallocing, %d mallocs so far\n",mallocNum);
	enriched_p *myPage = malloc(sizeof(enriched_p));
	


	myPage->e_page = page;

	//declare the time that the page has been used
	myPage->timeLastUsed = sim_time();
	
	if(algorithmCase == 3){
		myPage->referenceBit = 0;
	}
	else if (algorithmCase == 4){
		myPage->referenceBit = 1;
	}

	//iterate usage count for my page(????? not sure about this)
	myPage->usageCount = 0;

	//handle each algorithm on a case by case basis, use switch
	switch(algorithmCase){
		case(0):;
			
			//printf("alg is fifo\n");
			//try to find page within the queue
			//printf("finding page %d\n", myPage->e_page);
			enriched_p *foundFIFO = queue_find(myQueue, pageCompare, &(myPage->e_page));
			
			if(foundFIFO == NULL){
				//printf("didn't find the page within the queue, checking for room\n");
				//if there is no more room in the queue, push to tail and pop head
				int qSize = queue_size(myQueue);
				if ( qSize == myFrames){
					//printf("no more room in queue, queue size is %d\n", qSize);
					
					//if not found, pop the front of the queue and push the page to the tail of the queue
					//printf("popping front and pushing %d to tail\n", myPage->e_page);
					freeNum++;
					printf("freeing, %d frees so far\n",freeNum);
					free(queue_dequeue(myQueue));
					
					
					//free(queue_dequeue(myQueue));

					sim_get_page(page);
					queue_enqueue(myQueue, (void*)myPage);


				}
				//if there is room in the queue, enqueue the page using a ptr
				else{
					//printf("room in queue. enqueueing page %d\n", myPage->e_page);

					//calling sim_get_page
					sim_get_page(page);

					//enqueue the enriched page struct
					queue_enqueue(myQueue, (void*)myPage);
				}
			}
			else{
			//if found, do nothing
				//printf("found the page already within the queue, doing nothing\n");
		
		
				freeNum++;
				printf("freeing, %d frees so far\n",freeNum);
				free(myPage);
			}

			break;



		case(1):;
			//printf("alg is lru\n");
			//printf("#######################################\n");
			//printf("sim_time() is %d\n",myPage->timeLastUsed+1);
			//need to keep track of past knowledge of the page, use sim_time() in order to get the time of the sim for the page
			//can't you basically just do all of the setup for fifo?
			//check whether the page is in the list
			//if not, find the OLDEST page in the queue

			//find the page
			//printf("finding page %d\n", myPage->e_page);
			enriched_p *foundLRU = queue_find(myQueue, pageCompare, &(myPage->e_page));
			
			

			//if(foundLRU!=NULL) printf("found page exists and has a sim_time() of %d\n",foundLRU->timeLastUsed);

			if(foundLRU == NULL){
				//printf("didn't find the page within the queue, checking for room\n");
				//if there is no more room in the queue, pop the head of the queue and push to tail
				int qSize = queue_size(myQueue);
				if ( qSize == myFrames){
					//printf("no more room in queue, queue size is %d\n", qSize);



					//sort the queue before dequeueing
					//printf("sorting\n");
					queue_sort(myQueue, timeCompare);
					
					
					//printf("popping %d and pushing %d to tail\n", ((enriched_p*)(queue_head(myQueue)))->e_page, myPage->e_page);
					freeNum++;
					printf("freeing, %d frees so far\n",freeNum);
					free(queue_dequeue(myQueue));
					
					//printf("calling sim_get_page, should fault\n");
					sim_get_page(page);
					myPage->timeLastUsed = sim_time();
					
					queue_enqueue(myQueue, myPage);
					

				}
				//if there is room in the queue, enqueue the page using a ptr
				else{
					//printf("room in queue. enqueueing page %d\n", myPage->e_page);
					//calling sim_get_page
					sim_get_page(page);
					//enqueue the enriched page struct
					myPage->timeLastUsed = sim_time();
					
					queue_enqueue(myQueue, myPage);
					
				}
			}
			else{
				//printf("found the page in the queue\n");
				//if found, sort the queue according to priority, with the most recent being first

				//printf("sorting by oldest page\n");
				foundLRU->timeLastUsed = sim_time();
				//printf("sorting\n");
				queue_sort(myQueue, timeCompare);
				//sim_get_page(page);
				//printf("queue is now sorted. Head is %d\n", ((enriched_p*) queue_head(myQueue))->e_page);
				//queue is now in order of time last accessed
				freeNum++;
				printf("freeing, %d frees so far\n",freeNum);
				free(myPage);
			}

			

			break;


		case(2):;
			//printf("alg is lfu\n");
			//printf("#######################################\n");
			
			//printf("sim_time() is %d\n",myPage->timeLastUsed+1);
			//printf("usage count is %d\n",myPage->usageCount);

			//find the page
			//printf("finding page %d\n", myPage->e_page);
			enriched_p *foundLFU = queue_find(myQueue, pageCompare, &(myPage->e_page));

			if(foundLFU == NULL){
				//printf("didn't find the page within the queue, checking for room\n");
				//if there is no more room in the queue, pop the head of the queue and push to tail
				int qSize = queue_size(myQueue);
				if ( qSize == myFrames){
					//printf("no more room in queue, queue size is %d\n", qSize);

					//sort the queue before dequeueing
					//printf("sorting\n");
					queue_sort(myQueue, usageCountCompare);
					
					
					//printf("popping %d and pushing %d to tail\n", ((enriched_p*)(queue_head(myQueue)))->e_page, myPage->e_page);
					freeNum++;
					printf("freeing, %d frees so far\n",freeNum);
					free(queue_dequeue(myQueue));
					
					//printf("calling sim_get_page, should fault\n");
					sim_get_page(page);
					myPage->timeLastUsed = sim_time();
					
					queue_enqueue(myQueue, myPage);
					queue_sort(myQueue, usageCountCompare);

				}
				//if there is room in the queue, enqueue the page using a ptr
				else{
					//printf("room in queue. enqueueing page %d\n", myPage->e_page);
					//calling sim_get_page
					sim_get_page(page);


					//enqueue the enriched page struct
					myPage->timeLastUsed = sim_time();
					
					queue_enqueue(myQueue, myPage);
					queue_sort(myQueue, usageCountCompare);
				}
			}
			else{
				//printf("found page exists and has a usage count of %d\n",foundLFU->usageCount);
				//if found, sort the queue according to priority, with the most recent being first

				//printf("sorting by oldest page\n");

				//add +1 to the found page reference
				foundLFU->usageCount++;

				foundLFU->timeLastUsed = sim_time();
				//printf("sorting\n");
				queue_sort(myQueue, usageCountCompare);

				freeNum++;
				printf("freeing, %d frees so far\n",freeNum);
				free(myPage);
				//sim_get_page(page);
				//printf("queue is now sorted. Head is %d\n", ((enriched_p*) queue_head(myQueue))->e_page);
				//queue is now in order of time last accessed
			}
			break;



		case(3):;
			//base Second Chance off of FIFO
			//printf("#######################################\n");
			//printf("sim_time() is %d\n",myPage->timeLastUsed+1);
			//try to find page within the queue
			//printf("finding page %d\n", myPage->e_page);
			//printing queue
			//queue_find(myQueue, printCompare, &(myPage->e_page));
			enriched_p *foundSECOND = queue_find(myQueue, pageCompare, &(myPage->e_page));
			
			if(foundSECOND == NULL){
				//printf("didn't find the page within the queue, checking for room\n");
				//if there is no more room in the queue, push to tail and pop head
				int qSize = queue_size(myQueue);
				if ( qSize == myFrames){
					//printf("no more room in queue, queue size is %d\n", qSize);
					
					// //if not found, pop the front of the queue and push the page to the tail of the queue
					// printf("popping front and pushing %d to tail\n", myPage->e_page);
					
					// void* freePtr = queue_dequeue(myQueue);
					// free(freePtr);
					

					//printf("******************************entering loop***************************\n");
					enriched_p* dummyPageSecondChance;    
					for(int i = 0; i<=queue_size(myQueue);i++){    
						//dequeue the head
						dummyPageSecondChance = queue_dequeue(myQueue);
						//printf("head dequeued, value is %d, reference bit is %d\n", dummyPageSecondChance->e_page, dummyPageSecondChance->referenceBit);
						//check whether the reference bit is zero
						if(dummyPageSecondChance->referenceBit == 0){
							//if the reference bit is zero, it is marked for removal
							//so free the memory
							//printf("recognized that ref bit is zero, freeing memory and breaking\n");
							freeNum++;
							printf("freeing, %d frees so far\n",freeNum);
							free(dummyPageSecondChance);
							break;
						}
						else{
							//if the reference bit is 1, set the reference bit to 0
							//printf("$\n$\n$\n$\n$\nreference bit was not zero, setting to zero and enqueing %d\n", dummyPageSecondChance->e_page);
							dummyPageSecondChance->referenceBit = 0;
							//then enqueue the struct back onto the tail
							queue_enqueue(myQueue, dummyPageSecondChance);
						}
					}

					//continue to push the thing in myPage
					sim_get_page(page);
					myPage->referenceBit = 0;
					queue_enqueue(myQueue, (void*)myPage);


				}
				//if there is room in the queue, enqueue the page using a ptr
				else{
					//printf("room in queue. enqueueing page %d\n", myPage->e_page);

					//enqueue the enriched page struct
					myPage->referenceBit = 0;
					queue_enqueue(myQueue, (void*)myPage);

					//calling sim_get_page
					sim_get_page(page);
				}
			}
			else{
				//printf("found value in queue, doing nothing but setting the value to 1\n");
				foundSECOND->referenceBit = 1;
			}
	
			break;
		case(4):;
			//base Circular off of FIFO
			//printf("#######################################\n");
			//printf("sim_time() is %d\n",myPage->timeLastUsed+1);
			//try to find page within the queue
			//printf("finding page %d\n", myPage->e_page);
			//printing queue
			//queue_find(myQueue, printCompare, &(myPage->e_page));
			enriched_p *foundCIRCULAR = queue_find(myQueue, pageCompare, &(myPage->e_page));
			
			if(foundCIRCULAR == NULL){
				//printf("didn't find the page within the queue, checking for room\n");
				//if there is no more room in the queue, push to tail and pop head
				int qSize = queue_size(myQueue);
				if ( qSize == myFrames){
					//printf("no more room in queue, queue size is %d\n", qSize);
					

					

					//printf("******************************entering loop***************************\n");
					enriched_p* dummyPageCircular;    
					for(int i = 0; i<=queue_size(myQueue);i++){    

						dummyPageCircular = queue_dequeue(myQueue);


						if(dummyPageCircular->referenceBit == 0){
							//printf("recognized that dummyPageCircular #%d has reference bit 0\n", dummyPageCircular->e_page);
							freeNum++;
							printf("freeing, %d frees so far\n",freeNum);
							free(dummyPageCircular);
							break;
						}
						else{
							//printf("recognized that dummyPageCircular #%d has reference bit 1, swapping to 0 and enqueueing\n", dummyPageCircular->e_page);
							dummyPageCircular->referenceBit = 0;
							queue_enqueue(myQueue, dummyPageCircular);
						}
					}


					sim_get_page(page);
					//printf("enqueueing myPage #%d, setting reference bit to 1\n", myPage->e_page);
					myPage->referenceBit = 1;
					queue_enqueue(myQueue, (void*)myPage);


				}

				else{
					//printf("room in queue. enqueueing page %d\n", myPage->e_page);

					//enqueue the enriched page struct
					myPage->referenceBit = 1;
					queue_enqueue(myQueue, (void*)myPage);

					//calling sim_get_page
					sim_get_page(page);
				}
			}
			else{
				//printf("found page %d in queue, doing nothing and setting reference bit to 1\n",foundCIRCULAR->e_page);
				foundCIRCULAR->referenceBit = 1;
			}

			break;
	}
	
}

