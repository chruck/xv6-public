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

#include <stdio.h>           // FILE
#include "../types.h"        // uint
#include "../fs.h"           // BSIZE, superblock
#include "error.h"           // err

err readsuperblock(FILE *xv6_fs_img, struct superblock *sb);

#endif  // readsuperblock_h
