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
#include "mkfstools.h"       // rsect()
#include "checkinode.h"

err loadinodetable(FILE *xv6_fs_img, struct superblock *sb,
                   struct dinode *inodetbl)
{
        err rc = SUCCESS;
        //const long offset = sb->inodestart * BSIZE;
        uchar buf[BSIZE] = "";

        debug("Load inode table");

        inodetbl = malloc(sizeof(struct dinode) * sb->ninodes);

        if (NULL == inodetbl) {
                debug("unable to malloc %d inode structures", sb->ninodes);
                return CANT_MALLOC;
        }

        /*
        if (0 != fseek(xv6_fs_img, offset, SEEK_SET)) {
                debug("error reading inodes");
                return BAD_FS_FILE_SEEK;
        }

        if (1 != fread(&buf, BSIZE, 1, xv6_fs_img)) {
                debug("error reading inodes:  short");
                return BAD_FS_FILE_READ;
        }
        */
        rc = rsect(xv6_fs_img, sb->inodestart, buf);

        if (SUCCESS != rc) {
                debug("Error reading inode start");
                return rc;
        }

        //memmove(inodetbl, buf, sizeof(struct dinode));

        debug("Finish loading inode table");

        return rc;
}

err checkinodes(FILE *xv6_fs_img, struct superblock *sb,
                struct dinode *inodetbl)
{
        err rc = SUCCESS;

        debug("Begin reading inodes");

        rc = loadinodetable(xv6_fs_img, sb, inodetbl);

        debug("End reading inodes");

        return rc;
}
