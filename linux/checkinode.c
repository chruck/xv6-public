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
#include "../types.h"        // uint
#include "../stat.h"         // T_FILE, T_DIR, T_DEV
#include "../param.h"        // LOGSIZE
#include "debug.h"           // debug(), checkifdebugging()
#include "checkinode.h"

err loadinodetable(FILE *xv6_fs_img, struct superblock *sb,
                   //struct dinode *inodetbl[NINODES])
                   struct dinode **inodetbl)
{
        err rc = SUCCESS;
        struct dinode inode = {};

        debug("Load inode table");

        //*inodetbl = malloc(sizeof(struct dinode) * NINODES);

        /*
        if (NULL == *inodetbl) {
                debug("unable to malloc %d inode structures", NINODES);
                return CANT_MALLOC;
        }
        */

        for (int i = 0; NINODES > i; ++i) {
                bzero(&inode, sizeof(struct dinode));
                //rc = rinode(xv6_fs_img, i, &inode, sb->inodestart);
                rc = rinode(xv6_fs_img, i, &inode, LOGSIZE + 2);
                if (SUCCESS != rc) {
                        debug("Problem reading inode #%d", i);
                        return rc;
                }
                memmove(*inodetbl + i, &inode, sizeof(struct dinode));
        }

        debug("Finish loading inode table");

        return rc;
}

err checktype(struct dinode inode)
{
        err rc = SUCCESS;

        switch (inode.type) {
                case T_FILE:
                case T_DIR:
                case T_DEV:
                        break;
                default:
                        rc = FS_BAD_INODE;
                        break;
        }

        return rc;
}

err checkaddr(struct dinode inode)
{
        err rc = SUCCESS;

                // rc = FS_BAD_INODE_ADDR;

        return rc;
}

err checkrootdir(struct dinode inode)
{
        err rc = SUCCESS;

                // rc = FS_ROOT_DIR_DNE;

        return rc;
}

bool inuse(struct dinode inode)
{
        if (0 == inode.type
            && 0 == inode.major
            && 0 == inode.minor
            && 0 == inode.nlink
            && 0 == inode.size
            && 0 == inode.addrs[0]) {
                return false;
        }
        return true;
}

err checkinode(struct dinode inode)
{
        err rc = checktype(inode);
        if (SUCCESS != rc) {
                return rc;
        }

        rc = checkaddr(inode);
        if (SUCCESS != rc) {
                return rc;
        }
        return rc;
}

err checkinodes(FILE *xv6_fs_img, struct superblock *sb,
                //struct dinode *inodetbl[NINODES])
                struct dinode **inodetbl)
{
        err rc = SUCCESS;

        debug("Begin reading inodes");

        rc = loadinodetable(xv6_fs_img, sb, inodetbl);

        if (SUCCESS == rc) {
                printf("Checking inode:  ");
                // first inode is 1, not 0, and it should be '/'
                rc = checkrootdir((*inodetbl)[1]);

                for (int i = 1; NINODES > i; ++i) {
                        if (SUCCESS != rc) {
                                break;
                        }
                        if (!inuse((*inodetbl)[i])) {
                                continue;
                        }
                        printf("%d ", i);

                        rc = checkinode((*inodetbl)[i]);
                }
                printf("\n");
        }

        debug("End reading inodes");

        return rc;
}
