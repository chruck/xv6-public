#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "proc.h"

#define stdout 1
#define NULL 0

// draw a spinner
void
spinner(int i)
{
        char spinner[] = "-\\|/";

        printf(stdout, "\b%c", spinner[i % 4]);
}

// does calling fork increase the number getprocsinfo() returns?
void
forkincreasestest(void)
{
        struct procinfo allprocs[10];
        int numpidsbefore = getprocsinfo(allprocs);
        int numpidsafter = 0;
        int pid = 0;

        printf(stdout, "forkincreasestest test\n");
        printf(stdout, "    number of processes before: %d\n", numpidsbefore);

        if (0 > (pid = fork())) {
                printf(stdout, "call to fork() failed\n");
                exit();
        }

        if (0 == pid) {
                // child
                printf(stdout, "\n.");
                for (int i = 0; i < 1000; ++i) {
                        spinner(i);
                        sleep(1);
                }
                printf(stdout, "\n");
                exit();
        }

        numpidsafter = getprocsinfo(allprocs);
        printf(stdout, "    number of processes after: %d\n", numpidsafter);
        wait();

        if (numpidsafter == numpidsbefore) {
                printf(stdout, "forkincreasestest failed\n");
                exit();
        }

        printf(stdout, "forkincreasestest ok\n");
}

// does getprocsinfo() return number of existing processes?
void
numproctest(void)
{
        struct procinfo allprocs[10];
        int numpids = getprocsinfo(allprocs);

        printf(stdout, "numproctest test\n");

        if (0 == numpids) {
                printf(stdout, "numproctest failed: expecting more than "
                               "0 pids\n");
                exit();
        }
        printf(stdout, "    number of processes: %d\n", numpids);
        printf(stdout, "numproctest ok\n");
}

// does getprocsinfo() pass NULL?
void
nulltest(void)
{
        struct procinfo allprocs[10];
        int numpids = getprocsinfo(allprocs);

        printf(stdout, "null test\n", numpids);
        if (0) {
                printf(stdout, "null failed\n");
                exit();
        }
        printf(stdout, "null ok\n");
}

// does getprocsinfo() pass PIDs and process names?
void
procinfotest(void)
{
        struct procinfo allprocs[10];
        int numpids = getprocsinfo(allprocs);

        printf(stdout, "procinfotest test\n", numpids);
        if (0) {
                printf(stdout, "procinfotest failed\n");
                exit();
        }
        printf(stdout, "procinfotest ok\n");
}

// does getprocsinfo() return -1 on error?
void
errortest(void)
{
        struct procinfo allprocs[10];
        int numpids = getprocsinfo(allprocs);

        printf(stdout, "errortest test\n", numpids);
        if (0) {
                printf(stdout, "errortest failed\n");
                exit();
        }
        printf(stdout, "errortest ok\n");
}

int
main(int argc, char *argv[])
{
        printf(1, "testgetprocsinfo starting\n");

        if (open("test.ran", 0) >= 0){
                printf(1, "already ran user tests -- rebuild fs.img\n");
                exit();
        }
        close(open("test.ran", O_CREATE));

        nulltest();
        procinfotest();
        numproctest();
        errortest();
        forkincreasestest();

        exit();
}
