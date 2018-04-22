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
#include <string.h>          // memmove(3)
#include "../types.h"        // uint
#include "../fs.h"           // BSIZE, superblock
#include "debug.h"           // debug(), checkifdebugging()
#include "error.h"           // rc_err, fs_err, printerror()
#include "readsuperblock.h"  // readsuperblock()

rc_err readsuperblock(FILE *xv6_fs_img, struct superblock *sb)
{
        const long offset = 1 * BSIZE;
        uchar buf[BSIZE] = "";

        debug("Begin reading superblock");

        if (0 != fseek(xv6_fs_img, offset, SEEK_SET)) {
                debug("error reading superblock");
                return BAD_FS_FILE_SEEK;
        }

        if (1 != fread(&buf, BSIZE, 1, xv6_fs_img)) {
                debug("error reading superblock:  short");
                return BAD_FS_FILE_READ;
        }

        memmove(sb, buf, sizeof(struct superblock));

        debug("Superblock:\n\tsize=%d\n\tnblocks=%d\n\tninodes=%d\n"
              "\tnlog=%d\n\tlogstart=%d\n\tinodestart=%d\n\tbmapstart=%d",
              sb->size, sb->nblocks, sb->ninodes, sb->nlog, sb->logstart,
              sb->inodestart, sb->bmapstart);

        debug("End reading superblock");

        return SUCCESS;
}
