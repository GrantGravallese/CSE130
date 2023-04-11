/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "cartman.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>


/*
 * Callback when CART on TRACK arrives at JUNCTION in preparation for
 * crossing a critical section of track.
 */


void arrive(unsigned int cart, enum track track, enum junction junction) 
{

    

}

void depart(unsigned int cart, enum track track, enum junction junction){
        

}



/*
 * Start the CART Manager for TRACKS tracks.
 *
 * Return is optional, i.e. entering an infinite loop is allowed, but not
 * recommended. Some implementations will do nothing, most will initialise
 * necessary concurrency primitives.
 */
void cartman(unsigned int tracks) 
{
}
