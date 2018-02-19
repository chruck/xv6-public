/**
* @file systhread.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPLv3
*
* @section DESCRIPTION
*
* This file implements threading in the xv6 kernel
*/

#include "types.h"  // uint
#include "defs.h"   // argint()

int clone(void (*fcn)(void *), void *arg, void *stack)
{
        return 0;
}

int sys_clone(void)
{
        void *arg = (void *)0;
        void *stack = (void *)0;
        void (*fcn)(void*) = (void *)0;

        if (0 > argptr(0, (char **)fcn, sizeof(fcn))) {
                return -1;
        }

        if (0 > argptr(1, arg, sizeof(arg))) {
                return -1;
        }

        if (0 > argptr(2, stack, sizeof(stack))) {
                return -1;
        }

        return clone(fcn, arg, stack);
}

int join(int pid)
{
        return 0;
}

int sys_join(void)
{
        int pid = 0;

        if (0 > argint(0, &pid)) {
                return -1;
        }

        return join(pid);
}

