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
#include "x86.h"        // cli(), sti()
//#include "defs.h"       // panic()
#include "kthreads.h"

// Simple spin lock:
// Code for spin lock based on simplification of xv6's spinlock.c
// TODO:  use x86 atomic exchange (see xv6 kernel for something close)
int init_lock(lock_t *lock)
{
        return lock_init(lock);
}

        // from spinlock.c:
int lock_init(lock_t *lock)
{

        //void initlock(struct spinlock *lk, char *name)
        //{
                //lk->name = name;
                //lk->locked = 0;
                *lock = 0;
                //lk->cpu = 0;
        //}

        return (int)*lock;
}

// from spinlock.c:
// Check whether this cpu is holding the lock.
//int holding(struct spinlock *lock)
int holding(lock_t *lock)
{
        //return lock->locked && lock->cpu == mycpu();
        return *lock;
}

// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.
void pushcli(void)
{
        //int eflags;

        //eflags = readeflags();
        cli();
        //if (mycpu()->ncli == 0)
                //mycpu()->intena = eflags & FL_IF;
        //mycpu()->ncli += 1;
}

void popcli(void)
{
        //if (readeflags() & FL_IF)
                //panic("popcli - interruptible");
        //if (--mycpu()->ncli < 0)
                //panic("popcli");
        //if (mycpu()->ncli == 0 && mycpu()->intena)
                //sti();
}

int lock_acquire(lock_t *lock)
{
        // from spinlock.c:
        //void acquire(struct spinlock *lock)
        {
                pushcli();      // disable interrupts to avoid deadlock.

                if (holding(lock)) {
                        //panic("acquire");
                        printf(1, "lock_acquire()\n");
                }

                // The xchg is atomic.
                //while (xchg(&lock->locked, 1) != 0) ;
                while (xchg(lock, 1) != 0) {
                        ;
                }

                // Tell the C compiler and the processor to not move loads or stores
                // past this point, to ensure that the critical section's memory
                // references happen after the lock is acquired.
                __sync_synchronize();

                // Record info about lock acquisition for debugging.
                //lock->cpu = mycpu();
                //getcallerpcs(&lock, lock->pcs);
        }

        return 0;
}

int lock_release(lock_t *lock)
{
        // from spinlock.c:
        //void release(struct spinlock *lock)
        {
                if (!holding(lock)) {
                        //panic("release");
                        printf(1, "lock_release()\n");
                }

                //lock->pcs[0] = 0;
                //lock->cpu = 0;

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

                popcli();
        }

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
