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
* This file is ...
*/

#ifndef checkblk_h
#define checkblk_h

#include <stdio.h>      // FILE
#include "error.h"     // rc_err, fs_err, printerror()

fs_err checkblocks(FILE *xv6_fs_img);

#endif  // checkblk_h
