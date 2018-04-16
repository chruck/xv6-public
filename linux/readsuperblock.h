/**
* @file readsuperblock.h
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* This file is header to readsuperblock()
*/

#ifndef readsuperblock_h
#define readsuperblock_h

#include <stdio.h>      // FILE
#include "error.h"      // fs_err

fs_err readsuperblock(FILE *xv6_fs_img);

#endif  // readsuperblock_h
