/**
* @file debug.h
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL license
*
* @section DESCRIPTION
*
* This file is ...
*/

#ifndef debug_h
#define debug_h

#include <stdlib.h>  // secure_getenv()
#include <stdbool.h>  // bool, true/false

extern bool isdebugging;

void checkifdebugging(void)
{
        char *debug_env = getenv("DEBUG");

        switch (debug_env[0]) {
                case '1':  // 1
                case 't':  // true
                case 'y':  // yes
                        isdebugging = true;
                        break;
                default:
                        isdebugging = false;
                        break;
        }
}

#define debug(...)  do { \
                        if (isdebugging) { \
                                fprintf(stderr, __VA_ARGS__); \
                        } \
                    } while (false);

#endif  // debug_h
