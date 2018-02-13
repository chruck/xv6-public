/**
* @file testshmem.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL license
*
* @section DESCRIPTION
*
* Testing Project 2 Part B:  shmem system calls
*/

#include "param.h"
#include "types.h"  // uint
#include "stat.h"
#include "user.h"   // fork(), exec()
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

#define NULL (void *)(0)
#define stdout 1
#define out(...) printf(stdout, __VA_ARGS__)

int main(int argc, char *argv[])
{
        int pid = fork();
        int pg_num = 1;
        void *pg_addr = NULL;

        if (0 == pid) {  // child 1
                pid = getpid();
                pg_addr = shmem_access(pg_num);
                strcpy((char *)pg_addr, "foo\0");

                out("child1[%d]: %d processes accessing shared pg %d (%s)\n",
                    pid, shmem_count(pg_num), pg_num, (char *)pg_addr);

                exit();
        }

        if (pid < 0) {
                out("fork() failed creating child 1\n");
                exit();
        }

        if (0 == (pid = fork())) {  // child 2
                pid = getpid();
                pg_addr = shmem_access(pg_num);
                strcpy((char *)pg_addr, "bar\0");

                out("child2[%d]: %d processes accessing shared pg %d (%s)\n",
                    pid, shmem_count(pg_num), pg_num, (char *)pg_addr);

                exit();
        }

        if (pid < 0) {
                out("fork() failed creating child 2\n");
                exit();
        }

        if (0 == (pid = fork())) {  // child 3
                pid = getpid();
                pg_addr = shmem_access(pg_num);
                strcpy((char *)pg_addr, "baz\0");

                out("child3[%d]: %d processes accessing shared pg %d (%s)\n",
                    pid, shmem_count(pg_num), pg_num, (char *)pg_addr);

                exit();
        }

        if (pid < 0) {
                out("fork() failed creating child 3\n");
                exit();
        }

        pg_addr = shmem_access(pg_num);
        strcpy((char *)pg_addr, "qux\0");

        out("parent[%d]: %d processes accessing shared pg %d (%s)\n",
            pid, shmem_count(pg_num), pg_num, (char *)pg_addr);

        return 0;
}
