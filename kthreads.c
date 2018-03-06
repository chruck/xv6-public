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
#include "x86.h"        // cli(), sti()
#include "defines.h"   // NULL, TRUE, FALSE, SUCCESS, RC_ERR, STDOUT
#include "kthreads.h"  // lock_t, thread_t

// Simple spin lock:
// Code for spin lock based on simplification of xv6's spinlock.c
int init_lock(lock_t *lock)
{
        return lock_init(lock);
}

int lock_init(lock_t *lock)
{
        if (NULL == lock) {
                return -1;
        }

        *lock = 0;

        return 0;
}

int lock_acquire(lock_t *lock)
{
        if (NULL == lock) {
                return -1;
        }

        // from spinlock.c's acquire():
        cli();              // disable interrupts to avoid deadlock.

        // The xchg is atomic.
        while (xchg(lock, 1) != 0) {
                ;
        }

        // Tell the C compiler and the processor to not move loads or stores
        // past this point, to ensure that the critical section's memory
        // references happen after the lock is acquired.
        __sync_synchronize();

        return 0;
}

int lock_release(lock_t *lock)
{
        if (NULL == lock) {
                return -1;
        }

        // Tell the C compiler and the processor to not move loads or stores
        // past this point, to ensure that all the stores in the critical
        // section are visible to other cores before the lock is released.
        // Both the C compiler and the hardware may re-order loads and
        // stores; __sync_synchronize() tells them both not to.
        __sync_synchronize();

        // Release the lock, equivalent to lock->locked = 0.
        // This code can't use a C assignment, since it might
        // not be atomic. A real OS would use C atomics here.
        //asm volatile ("movl $0, %0":"+m" (lock->locked):);
        asm volatile ("movl $0, %0":"+m" (*lock):);

        sti();

        return 0;
}

struct kthread thread_create(void (*start_routine)(void *), void *arg)
{
        struct kthread new_thread = {-1, -1, -1};

        if (NULL == start_routine) {
                return new_thread;
        }

        // create new user stack
        void *stack = malloc(PGSIZE);

        // create child thread and get it running
        uint pid = clone(start_routine, arg, stack);

        new_thread.pid = pid;

        return new_thread;
}

int thread_join(struct kthread thread)
{
        return join(thread.pid);
}
