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
* This file is the header file to checkinodes
*/

#ifndef checkinode_h
#define checkinode_h

#include <stdio.h>           // FILE
#include "../types.h"        // uint
#include "../fs.h"           // BSIZE, superblock
#include "error.h"           // err

err checkinodes(FILE *xv6_fs_img, struct superblock *sb,
                struct dinode *inodetbl);

#endif  // checkinode_h
