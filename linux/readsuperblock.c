/**
* @file readsuperblock.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* This file reads the filesystem file for the superblock
*/

#include <stdio.h>           // FILE
#include "error.h"           // rc_err, fs_err, printerror()
#include "readsuperblock.h"  // readsuperblock()

fs_err readsuperblock(FILE *xv6_fs_img)
{
        return FS_SUCCESS;
}
