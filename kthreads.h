/**
* @file kthreads.h
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPLv3
*
* @section DESCRIPTION
*
* Header file for xv6 kernel threads.
*/

#ifndef kthreads_h
#define kthreads_h

#define NUM_CONS 2

typedef int lock_t;
typedef int kthread_t;

int init_lock(lock_t *lock);
int lock_acquire(lock_t *lock);
int lock_release(lock_t *lock);
kthread_t thread_create(void consumer(void *arg), int []);
int thread_join(kthread_t thread);

#endif  // kthreads_h
