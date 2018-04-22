/**
* @file checkinode.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* This file checks the inodes.
*/

#include <string.h>          // memmove(3)
#include "debug.h"           // debug(), checkifdebugging()
#include "checkinode.h"

err checkinodes(FILE *xv6_fs_img, struct superblock *sb)
{
        const long offset = sb->inodestart * BSIZE;
        uchar buf[BSIZE] = "";

        debug("Begin reading inodes");

        if (0 != fseek(xv6_fs_img, offset, SEEK_SET)) {
                debug("error reading inodes");
                return BAD_FS_FILE_SEEK;
        }

        if (1 != fread(&buf, BSIZE, 1, xv6_fs_img)) {
                debug("error reading inodes:  short");
                return BAD_FS_FILE_READ;
        }

        memmove(sb, buf, sizeof(struct superblock));

        debug("Superblock:\n\tsize=%d\n\tnblocks=%d\n\tninodes=%d\n"
              "\tnlog=%d\n\tlogstart=%d\n\tinodestart=%d\n\tbmapstart=%d",
              sb->size, sb->nblocks, sb->ninodes, sb->nlog, sb->logstart,
              sb->inodestart, sb->bmapstart);

        debug("End reading inodes");

        return SUCCESS;
}
