#include "types.h"  // uint
//#include "stat.h"
#include "user.h"  // strcpy, syscalls
#include "kthreads.h"  // lock_t, kthread

#define stdout 1
#define out(...) printf(stdout, __VA_ARGS__)

// global lock for product
lock_t lock_g;

int testnulllock(char *err)
{
        int rc = 0;

        if (-1 != (rc = init_lock(NULL))) {
                strcpy(err, "err:  init_lock() accepts NULL for lock.\n");
                return -1;
        }

        if (-1 != (rc = lock_acquire(NULL))) {
                strcpy(err, "err:  lock_acquire() accepts NULL for lock.\n");
                return -2;
        }

        if (-1 != (rc = lock_release(NULL))) {
                strcpy(err, "err:  lock_release() accepts NULL for lock.\n");
                return -3;
        }

        return 0;
}

int testgloballock(char *err)
{
        int rc = 0;

        if (0 != lock_g) {
                strcpy(err, "err:  Global lock set to locked.\n");
                return -4;
        }

        if ((rc = init_lock(&lock_g))) {
                strcpy(err, "err:  Unable to init global lock.\n");
                return -5;
        }

        return 0;
}

int testnullthread(char *err)
{
        int rc = 0;

        //if (-1 != (rc = thread_create(NULL, NULL))) {
        //struct kthread thr = thread_create(NULL, NULL);
        /*
        if (-1 != (thread_create(NULL, NULL))) {
                strcpy(err, "err:  thread_create() accepts NULL for start_routine.  \n", rc);
                return -6;
        }
        */

        struct kthread emptythr = {0};
        if (-1 != (rc = thread_join(emptythr))) {
                strcpy(err, "err:  thread_join() accepts NULL for struct kthread.\n");
                return -7;
        }

        return 0;
}

int testthread_create(char *err)
{
        int rc = 0;
        kthread_t thr = thread_create(NULL, NULL);

        if (0 == thr.pid) {
                strcpy(err, "Empty thread_create() returned PID 0, weird\n");
        } else if (0 > thr.pid) {
                strcpy(err, "Empty thread_create() returned PID, as expected\n");
        } else {
                strcpy(err, "Empty thread_create() error:  returned PID\n");
        }

        return !rc;
}

int main(void)
{
        int rc = 0;
        /*
        char err[255] = "no error";

        if ((rc = testnulllock(err))) {
                out("testnulllock() failed, rc=%d:\n\t%s", rc, err);
                return rc;
        }

        if ((rc = testgloballock(err))) {
                out("testgloballock() failed, rc=%d:\n\t%s", rc, err);
                return rc;
        }

        if ((rc = testnullthread(err))) {
                out("testnullthread() failed, rc=%d:\n\t%s", rc, err);
                return rc;
        }

        if ((rc = testthread_create(err))) {
                out("testthread_create() failed, rc=%d:\n\t%s", rc, err);
                return rc;
        }
        */

        return rc;
}
