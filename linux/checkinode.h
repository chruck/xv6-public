/**
* @file checkinode.h
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

#ifndef checkinode_h
#define checkinode_h

#include <stdio.h>      // FILE
#include "error.h"      // rc_err, fs_err, printerror()

fs_err checkinodes(FILE *xv6_fs_img);

#endif  // checkinode_h
