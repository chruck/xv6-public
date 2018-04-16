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

#include <stdio.h>           // FILE, fseek(3)
#include "../types.h"        // uint
#include "../fs.h"           // BSIZE
#include "debug.h"           // debug(), checkifdebugging()
#include "error.h"           // rc_err, fs_err, printerror()
#include "readsuperblock.h"  // readsuperblock()

fs_err readsuperblock(FILE *xv6_fs_img)
{
        const long offset = 1 * BSIZE;
        char buf[BSIZE] = "";

        debug("Begin reading superblock");

        if (0 != fseek(xv6_fs_img, offset, SEEK_SET)) {
                debug("error reading superblock");
                return FS_BAD_FS_FILE_SEEK;
        }

        if (1 != fread(&buf, BSIZE, 1, xv6_fs_img)) {
                debug("error reading superblock:  short");
                return FS_BAD_FS_FILE_READ;
        }

        debug("End reading superblock");

        return FS_SUCCESS;
}
