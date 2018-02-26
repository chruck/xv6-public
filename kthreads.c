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

#include "types.h"      // uchar, NULL
#include "param.h"      // NCPU
#include "mmu.h"        // taskstate, PGSIZE
#include "user.h"       // clone(), malloc()
//#include "syscall.h"    // clone
//#include "proc.h"       // clone
#include "kthreads.h"

// Simple spin lock:
// TODO:  use x86 atomic exchange (see xv6 kernel for something close)
int init_lock(lock_t *lock)
{
        return lock_init(lock);
}

int lock_init(lock_t *lock)
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

struct kthread thread_create(void (*start_routine)(void *), void *arg)
{
        // TODO:  create new user stack
        void *stack = malloc(PGSIZE);
        struct kthread new_thread;

        // TODO:  use #define PGROUNDUP(sz)  (((sz)+PGSIZE-1) &
        // ~(PGSIZE-1)) for malloc'd page (needs to be aligned)


        // create child thread and get it running
        uint pid = clone(start_routine, arg, stack);

        new_thread.pid = pid;

        return new_thread;
}

int thread_join(struct kthread thread)
{
        int rc = 0;

        // TODO:  Parent pid?
        rc = join(thread.pid);

        // TODO:  free user stack

        return rc;
}
