/**
* @file derefnull.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL
*
* @section DESCRIPTION
*
* This file tests null pointer dereferencing for Project 2 Part A

* Output in Linux:

tiger[00:21:20](139)(master)~/src/cpsc8240$ ./derefnull
NULL is '(null)', but something is 'something'.
Segmentation fault

* Output in xv6:


*/

//#include <stddef.h>
//#include <stdio.h>
#include "types.h"
#include "user.h"

#define NULL (void *)(0)
#define stdout 1
#define out(...) printf(stdout, __VA_ARGS__)

int main(int argc, char **argv)
{
        int *nil = NULL;
        char *null = NULL;
        char *something = "something";

        //printf("NULL is '%s', but something is '%s'.\n", null, something);
        out("NULL is '%s', but something is '%s'.\n", &null, &something);
        out("NULL is '%s', but something is '%s'.\n", *null, *something);
        out("NULL is '%s', but something is '%s'.\n", null, something);
        out("NULL is '%s', but something is '%s'.\n", &null, &something);
        //printf("NULL is '%d'.", *nil);
        out("NULL is '%d'.", *nil);
        out("NULL is '%d'.", nil);
        out("NULL is '%d'.", &nil);

        return 0;
}
