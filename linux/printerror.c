/**
* @file printerror.c
* @author Jas Eckard <eckard@clemson.edu
*
* @section LICENSE
*
* (GPL license?)
*
* @section DESCRIPTION
*
* This file is used by fschecker to print out the error messages.
*/

#include <stdio.h>
#include "error.h"

int printerror(const fs_err errno)
{
        switch (errno) {
                case FS_BAD_INODE:
                        printf("bad inode\n");
                        break;

                case FS_BAD_INODE_ADDR:
                        printf("bad address in inode\n");
                        break;

                case FS_ROOT_DIR_DNE:
                        printf("root directory does not exist\n");
                        break;

                case FS_DIR_BAD_FORMAT:
                        printf("directory not properly formatted\n");
                        break;

                case FS_PARENT_DIR_MISMATCH:
                        printf("parent directory mismatch\n");
                        break;

                case FS_INODE_ADDR_USED_MARKED_FREE:
                        printf("address used by inode but marked free in bitmap\n");
                        break;

                case FS_BLOCK_USED_NOT_USED:
                        printf("bitmap marks block in use but it is not in use\n");
                        break;

                case FS_ADDR_USED_MULT:
                        printf("address used more than once\n");
                        break;

                case FS_INODE_USED_NOT_IN_DIR:
                        printf("inode marked in use but not found in a directory\n");
                        break;

                case FS_INODE_IN_DIR_MARKED_FREE:
                        printf("inode referred to in directory but marked free\n");
                        break;

                case FS_FILE_BAD_REF_COUNT:
                        printf("bad reference count for file\n");
                        break;

                case FS_DIR_MULTI_IN_FS:
                        printf("directory appears more than once in file system\n");
                        break;

                case FS_SUCCESS:
                default:
                        break;
        }

        return errno;
}
