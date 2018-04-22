/**
* @file error.h
* @author Jas Eckard <eckard@clemson.edu
*
* @section LICENSE
*
* (GPL license?)
*
* @section DESCRIPTION
*
* This header file is used by fschecker for printing out the error
* messages, and has error codes.
*/

#ifndef error_h
#define error_h

typedef enum {
        SUCCESS = 0,
        IMG_NOT_FOUND = 1,
        NO_IMG_GIVEN,
        BAD_FS_FILE_SEEK,
        BAD_FS_FILE_READ,
} rc_err;

typedef enum {
        FS_SUCCESS = 0,
        FS_BAD_INODE,
        FS_BAD_INODE_ADDR,
        FS_ROOT_DIR_DNE,
        FS_DIR_BAD_FORMAT,
        FS_PARENT_DIR_MISMATCH,
        FS_INODE_ADDR_USED_MARKED_FREE,
        FS_BLOCK_USED_NOT_USED,
        FS_ADDR_USED_MULT,
        FS_INODE_USED_NOT_IN_DIR,
        FS_INODE_IN_DIR_MARKED_FREE,
        FS_FILE_BAD_REF_COUNT,
        FS_DIR_MULTI_IN_FS,
} fs_err;

int printerror(const fs_err errno);

#endif  // error_h
