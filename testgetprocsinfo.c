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
        struct procinfo allprocs[NPROC];
        int numpidsbefore = getprocsinfo((struct procinfo**)&allprocs);
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
                for (int i = 0; i < 100; ++i) {
                        spinner(i);
                        sleep(1);
                }
                printf(stdout, "\n");
                exit();
        }

        numpidsafter = getprocsinfo((struct procinfo **)&allprocs);
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
        struct procinfo allprocs[NPROC];
        int numpids = getprocsinfo((struct procinfo **)&allprocs);

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
        struct procinfo nullprocinfo = {0};
        struct procinfo allprocs[NPROC] = {0};
        int numpids = getprocsinfo((struct procinfo **)&allprocs);

        printf(stdout, "null test\n", numpids);

        if (nullprocinfo.pid == allprocs[0].pid
            && strcmp(nullprocinfo.pname, allprocs[0].pname)) {
                printf(stdout, "null failed\n");
                exit();
        }

        printf(stdout, "null ok\n");
}

// does getprocsinfo() pass PIDs and process names?
void
procinfotest(void)
{
        struct procinfo bogus = {105, "procinfotest"};
        struct procinfo allprocs[NPROC] = {bogus};
        int numpids = getprocsinfo((struct procinfo **)&allprocs);

        printf(stdout, "procinfotest test\n");
        for (--numpids; 0 <= numpids; --numpids) {
                printf(stdout, "        %d:  %d  %s\n", numpids, allprocs[numpids].pid,
                                allprocs[numpids].pname);
                if (0 >= allprocs[numpids].pid) {
                        printf(stdout, "procinfotest failed:  pid %d "
                                       "below or equal to zero\n",
                                        allprocs[numpids].pid);
                        //exit();
                }
                if (0 == allprocs[numpids].pname[0] ) {
                        printf(stdout, "procinfotest failed:  pid %d "
                                        "w/o name\n",
                                        allprocs[numpids].pid);
                        exit();
                }
        }
        printf(stdout, "procinfotest ok\n");
}

// does getprocsinfo() return -1 on error?
void
errortest(void)
{
        struct procinfo allprocs[NPROC];
        int numpids = getprocsinfo((struct procinfo **)&allprocs);

        printf(stdout, "errortest test\n");
        if (-1 == numpids) {
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
                printf(1, "already ran user tests -- rm test.ran\n");
                exit();
        }
        close(open("test.ran", O_CREATE));

        errortest();
        nulltest();
        numproctest();
        forkincreasestest();
        procinfotest();

        exit();
}
