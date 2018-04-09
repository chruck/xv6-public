/**
* @file checkdir.h
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

#ifndef checkdir_h
#define checkdir_h

#include <stdio.h>      // FILE
#include "error.h"     // rc_err, fs_err, printerror()

fs_err checkdirectories(FILE *xv6_fs_img);

#endif  // checkdir_h
