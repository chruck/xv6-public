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

typedef uint lock_t;
typedef struct kthread {
        uint pid;
        uint parent_pid;
        uint thread_num;
} kthread_t;

int init_lock(lock_t *lock);
int lock_init(lock_t *lock);
int lock_acquire(lock_t *lock);
int lock_release(lock_t *lock);
struct kthread thread_create(void (*start_routine)(void *), void *arg);
int thread_join(struct kthread thread);

#endif  // kthreads_h
