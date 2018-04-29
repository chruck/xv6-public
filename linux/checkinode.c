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
                   struct dinode **inodetbl)
{
        err rc = SUCCESS;
        struct dinode inode = {};

        debug("Load inode table");

        for (int i = 0; NINODES > i; ++i) {
                bzero(&inode, sizeof(struct dinode));
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
        const int nlog = LOGSIZE;
        const int ninodeblocks = NINODES / IPB + 1;
        const int nbitmap = FSSIZE / (BSIZE * 8) + 1;
        int i = 0;

        for (i = 0; NDIRECT > i; ++i) {
                if (0 == inode.addrs[i]) {
                        continue;
                }
                if (FSSIZE < inode.addrs[i]
                    || 2 + nlog + ninodeblocks + nbitmap > inode.addrs[i]) {
                        return FS_BAD_INODE_ADDR;
                }
        }
        // indirect inodes
        if (0 == inode.addrs[NDIRECT]) {
                return SUCCESS;
        }

        return SUCCESS;
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

bool isdot(struct dirent direntry)
{
        if ('.' == direntry.name[0] && '\0' == direntry.name[1]) {
                return true;
        }

        return false;
}


bool isdotdot(struct dirent direntry)
{
        if ('.' == direntry.name[0] && '.' == direntry.name[1]
            && '\0' == direntry.name[2]) {
                return true;
        }

        return false;
}

err checkrootdir(FILE *xv6_fs_img, const struct dinode inode,
                 struct dinode **inodetbl)
{
        err rc = SUCCESS;

        if (0 != memcmp(&(*inodetbl)[1], &inode, sizeof(inode))) {
                return FS_ROOT_DIR_DNE;
        }

        if (!inuse(inode)) {
                return FS_ROOT_DIR_DNE;
        }

        return rc;
}

err checkvaliddir(FILE *xv6_fs_img, struct dinode inode)
{
        err rc = SUCCESS;
        struct dirent direntry = {};
        char buf[BSIZE] = "";
        bool hasdot = false;
        bool hasdotdot = false;

        if (T_DIR != inode.type) {
                return SUCCESS;
        }

        for (int i = 0; NDIRECT > i; ++i) {
                rc = rsect(xv6_fs_img, inode.addrs[i], buf);
                if (SUCCESS != rc) {
                        return rc;
                }
                if (NULL == memcpy(&direntry, &buf, sizeof(direntry))) {
                        return FS_DIR_BAD_FORMAT;
                }
                if (isdot(direntry)) {
                        hasdot = true;
                }
                if (isdotdot(direntry)) {
                        hasdotdot = true;
                }
        }

        if (hasdot && hasdotdot) {
                return SUCCESS;
        }

        return FS_DIR_BAD_FORMAT;
}

err checkinode(FILE *xv6_fs_img, struct dinode inode)
{
        err rc = checktype(inode);
        if (SUCCESS != rc) {
                return rc;
        }

        rc = checkaddr(inode);
        if (SUCCESS != rc) {
                return rc;
        }

        rc = checkvaliddir(xv6_fs_img, inode);
        if (SUCCESS != rc) {
                return rc;
        }

        return rc;
}

err checkinodes(FILE *xv6_fs_img, struct superblock *sb,
                struct dinode **inodetbl)
{
        err rc = SUCCESS;

        debug("Begin reading inodes");

        rc = loadinodetable(xv6_fs_img, sb, inodetbl);

        if (SUCCESS == rc) {
                printf("Checking inode:  ");
                // first inode is 1, not 0, and it should be '/'
                rc = checkrootdir(xv6_fs_img, (*inodetbl)[1], inodetbl);

                for (int i = 1; NINODES > i; ++i) {
                        if (SUCCESS != rc) {
                                break;
                        }
                        if (!inuse((*inodetbl)[i])) {
                                continue;
                        }
                        printf("%d ", i);

                        rc = checkinode(xv6_fs_img, (*inodetbl)[i]);
                }
                printf("\n");
        }

        debug("End reading inodes");

        return rc;
}
