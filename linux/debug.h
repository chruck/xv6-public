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

static inline void checkifdebugging(void)
{
        char *debug_env = getenv("DEBUG");

        // No 'DEBUG' env var:  don't debug
        if (NULL == debug_env) {
                isdebugging = false;
                return;
        }

        switch (debug_env[0]) {
                case '1':  // "1"
                case 't':  // "true"
                case 'y':  // "yes"
                        isdebugging = true;
                        break;
                case 0:    // exists, but is empty
                default:
                        isdebugging = false;
                        break;
        }
}

#define debug(str, args...) \
        do { \
                if (isdebugging) { \
                        fprintf(stderr, "DEBUG:  " str "\n", ##args); \
                } \
        } while (false);

#endif  // debug_h
