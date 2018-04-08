/**
* @file fscheck.c
* @author Jas Eckard <eckard@clemson.edu
*
* @section LICENSE
*
* (GPL license?)
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
                        - inode unallocated
                        - inode not valid type (`T_FILE`, `T_DIR`, `T_DEV`)
                - `bad address in inode`
                        - in-use inode
                        - block addr invalid (incl indirect blocks)
                - `root directory does not exist`
                        - root directory does not exist
                        - root directory is not inode number 1
                - `directory not properly formatted`
                        - dir doesn't contain `.` and `..`
                - `parent directory mismatch`
                        - `..` doesn't refer to parent inode
                        - parent doesn't point back to this dir
                - `address used by inode but marked free in bitmap`
                        - in-use inode
                        - each addr in use also marked 'in use' in bitmap
                - `bitmap marks block in use but it is not in use`
                        - block marked in-use in bitmap not actually in
                          use in inode/indirect block
                - `address used more than once`
                        - in-use inode
                        - address used more than once
                - `inode marked in use but not found in a directory`
                        - inode marked used in inode table
                        - not referred to in any directory
                - `inode referred to in directory but marked free`
                        - inode number referred to in valid directory
                        - not marked in-use in inode table
                - `bad reference count for file`
                        - ref count (num of links) for reg files don't
                          match num of times file is reft to in dirs
                        - ie, hard link broke
                - `directory appears more than once in file system`
                        - dir appears in more than 1 other dir
                        - ie, extra dir link
        - invoked:  `fscheck file_system_image`
                - if fs img DNE
                        - stderr:  `image not found`
                        - rc:  1
                - success rc:  0
        - look at `fs.img`, created by `mkfs` and `mkfs.c`

*/

int main(const int argc, const char *argv[])
{
        return 0;
}
