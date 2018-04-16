/**
* @file fscheck.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* This file is an xv6 filesystem checker.

TODO:

Part B1: filesystem checker, Linux
        - reads filesystem image
        - makes sure consistent
        - if not consistent, takes steps to fix problems it sees, w/o fixing
        - source file `fs.h` has basic structs:
                - superblock
                - on-disk inode format (`struct dinode`)
                - directory entry format (`struct dirent`)
        - `mkfs.c` useful to look at:  creating empty fs img
        - figure out on-disk format of xv6 simple fs
                - check if parts of structure are consistent
        - how does mkfs/fs code record persistent info?
        - read fs image, determine consistency of (print error):
                - `bad inode`
                        FS_BAD_INODE
                        - inode unallocated
                        - inode not valid type (`T_FILE`, `T_DIR`, `T_DEV`)
                - `bad address in inode`
                        FS_BAD_INODE_ADDR
                        - in-use inode
                        - block addr invalid (incl indirect blocks)
                - `root directory does not exist`
                        FS_ROOT_DIR_DNE
                        - root directory does not exist
                        - root directory is not inode number 1
                - `directory not properly formatted`
                        FS_DIR_BAD_FORMAT
                        - dir doesn't contain `.` and `..`
                - `parent directory mismatch`
                        FS_PARENT_DIR_MISMATCH
                        - `..` doesn't refer to parent inode
                        - parent doesn't point back to this dir
                - `address used by inode but marked free in bitmap`
                        FS_INODE_ADDR_USED_MARKED_FREE
                        - in-use inode
                        - each addr in use also marked 'in use' in bitmap
                - `bitmap marks block in use but it is not in use`
                        FS_BLOCK_USED_NOT_USED
                        - block marked in-use in bitmap not actually in
                          use in inode/indirect block
                - `address used more than once`
                        FS_ADDR_USED_MULT
                        - in-use inode
                        - address used more than once
                - `inode marked in use but not found in a directory`
                        FS_INODE_USED_NOT_IN_DIR
                        - inode marked used in inode table
                        - not referred to in any directory
                - `inode referred to in directory but marked free`
                        FS_INODE_IN_DIR_MARKED_FREE
                        - inode number referred to in valid directory
                        - not marked in-use in inode table
                - `bad reference count for file`
                        FS_FILE_BAD_REF_COUNT
                        - ref count (num of links) for reg files don't
                          match num of times file is reft to in dirs
                        - ie, hard link broke
                - `directory appears more than once in file system`
                        FS_DIR_MULTI_IN_FS
                        - dir appears in more than 1 other dir
                        - ie, extra dir link
        - invoked:  `fscheck file_system_image`
                - if fs img DNE
                        - stderr:  `image not found`
                        - rc:  1
                - success rc:  0
        - look at `fs.img`, created by `mkfs` and `mkfs.c`

*/

#include <stdio.h>           // printf(), fprintf()
#include "checkinode.h"      // checkinodes()
#include "checkdir.h"        // checkdirectories()
#include "checkblk.h"        // checkblocks()
#include "error.h"           // rc_err, fs_err, printerror()
#include "debug.h"           // debug(), checkifdebugging()
#include "readsuperblock.h"  // readsuperblock()

// Global vars
bool isdebugging = false;

rc_err checkparams(const int argc, const char *argv[], FILE **fp)
{
        checkifdebugging();

        if (2 != argc) {
                fprintf(stderr, "expecting 1 file system image filename\n\n");
                return NO_IMG_GIVEN;
        }

        if (NULL == (*fp = fopen(argv[1], "r"))) {
                fprintf(stderr, "image not found\n\n");
                return IMG_NOT_FOUND;
        }

        debug("Checking filesystem file '%s'.\n", argv[1]);

        return SUCCESS;
}

rc_err usage(const rc_err rc)
{
        printf("Usage:  fscheck file_system_image\n\n");

        return rc;
}

int main(const int argc, const char *argv[])
{
        rc_err rc = SUCCESS;
        fs_err err = FS_SUCCESS;
        FILE *fs_img = NULL;

        if (SUCCESS != (rc = checkparams(argc, argv, &fs_img))) {
                return usage(rc);
        }

        /*
        for ( ; FS_DIR_MULTI_IN_FS >= err; ++err) {
                printerror(err);
        }
        */

        err = readsuperblock(fs_img);

        if (FS_SUCCESS == err) {
                err = checkinodes(fs_img);
        }

        if (FS_SUCCESS == err) {
                err = checkdirectories(fs_img);
        }

        if (FS_SUCCESS == err) {
                err = checkblocks(fs_img);
        }

        fclose(fs_img);

        return printerror(err);
}
