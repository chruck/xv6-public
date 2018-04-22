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
* Header file to check directories.
*/

#ifndef checkdir_h
#define checkdir_h

#include <stdio.h>      // FILE
#include "error.h"      // err, printerror()

err checkdirectories(FILE *xv6_fs_img);

#endif  // checkdir_h
