/**
* @file kthreads.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPLv3
*
* @section DESCRIPTION
*
* Implementation of xv6 kthreads as defined in Project 3, and tested
* by testkthreads.c
*/

#include "kthreads.h"

int init_lock(lock_t *lock)
{
        return 0;
}

int lock_acquire(lock_t *lock)
{
        return 0;
}

int lock_release(lock_t *lock)
{
        return 0;
}

kthread_t thread_create(void consumer(void *arg), int arg[])
{
        return 0;
}

int thread_join(kthread_t thread)
{
        return 0;
}
