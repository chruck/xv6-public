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

#include <string.h>          // memmove(3)
#include "debug.h"           // debug(), checkifdebugging()
#include "mkfstools.h"       // rsect()
#include "readsuperblock.h"  // readsuperblock()

err readsuperblock(FILE *xv6_fs_img, struct superblock *sb)
{
        err rc = SUCCESS;
        uchar buf[BSIZE] = "";

        debug("Begin reading superblock");

        if (SUCCESS != (rc = rsect(xv6_fs_img, 1, &buf))) {
                debug("error reading superblock");
                return rc;
        }

        memmove(sb, buf, sizeof(struct superblock));

        debug("Superblock:\n\tsize=%d\n\tnblocks=%d\n\tninodes=%d\n"
              "\tnlog=%d\n\tlogstart=%d\n\tinodestart=%d\n\tbmapstart=%d",
              sb->size, sb->nblocks, sb->ninodes, sb->nlog, sb->logstart,
              sb->inodestart, sb->bmapstart);

        debug("End reading superblock");

        return SUCCESS;
}
