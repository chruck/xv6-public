/**
* @file checkblk.h
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* Header file to check blocks
*/

#ifndef checkblk_h
#define checkblk_h

#include <stdio.h>      // FILE
#include "error.h"      // err, printerror()

err checkblocks(FILE *xv6_fs_img);

#endif  // checkblk_h
