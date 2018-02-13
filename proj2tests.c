/**
* @file proj2tests.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL
*
* @section DESCRIPTION
*
* This file tests Project 2 modifications to xv6 source code.
*/

#include "types.h"  // uint
#include "user.h"   // fork(), exec()

#define NULL (void *)(0)
#define stdout 1
#define out(...) printf(stdout, __VA_ARGS__)

int main(int argc, char **argv)
{
        int rc = 0;
        int pid = fork();

        if (0 == pid) {  // child 1
                out("Running testderefnull:\n\n");
                rc = exec("testderefnull", NULL);

                if (!rc) {
                        out("\n... testderefnull failed.\n");
                }
                exit();
        }

        wait();

        if (0 == pid) {  // child 2
                out("Running testshmem:\n\n");
                rc = exec("testshmem", NULL);

                if (!rc) {
                        out("\n... testshmem failed.\n");
                }
                exit();
        }

        wait();

        return rc;
}
