#include "types.h"
#include "stat.h"
#include "user.h"
#include "kthreads.h"

//#define LOCKS_ON 1
int LOCKS_ON = 1;
#define NULL (void *)0
#define stdout 1
#define out(...) printf(stdout, __VA_ARGS__)


// global lock for product
lock_t lock_g;

#define NUM_PROD 3
#define NUM_CONS 2
#define TOTAL_PRODUCTS 10000000

int testgloballock()
{
        if (!lock_g) {
                out("Global lock doesn't exist.\n");
                return -1;
        }

        if (!init_lock(&lock_g)) {
                out("Unable to init global lock.\n");
                return -2;
        }

        return 0;
}

int testthread_create()
{
        int rc = 0;
        kthread_t thr = thread_create(NULL, NULL);

        if (0 == thr.pid) {
                out("Empty thread_create() returned PID 0, weird\n");
        } else if (0 > thr.pid) {
                out("Empty thread_create() returned PID %d, as expected\n", rc);
        } else {
                out("Empty thread_create() error:  returned PID %d\n", rc);
        }

        return !rc;
}

int main(void)
{
        return testgloballock()
                && testthread_create();
}
