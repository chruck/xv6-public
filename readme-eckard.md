Changes made to original xv6 source code for Project 4
===

NOTES:
- Like Project 3, this version of the source code uses K&R-style
  indentation:  The only difference to the files not mentioned below is
  indentation, for my own readability.

Part A:  xv6 priority-based scheduler
        1 = low priority
        2 = high priority
        - scheduler runs high priority
        - 2 procs w/ high:  round-robin
        - `int setpri(int num)`
                - sets priority for calling proc
                - default priority for each process = 1 (low)
                - can raise or lower
                - return -1 if `num` != 1 or 2
                - return 0 on success
        - `int getpinfo(struct pstat*)`
                - basic info of each running proc
                        - incl how long run at each priority (in clock ticks)
                        - PID
                        - used for modified `ps(1)`
        - modify `ps(1)` to utilize `getpinfo()`
        - processes' priority = 1 at creation
        - process can only be priority = 2 after `setpri()` called
        - `Makefile`:  change `CPUS := 2` &rarr; `CPUS := 1`
        - Majority of scheduler code in `proc.[ch]`
        - `pstat` structure from supplied `pstat.h` to be used in
          kernel and passed to userspace

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

Part B2:  filesystem integrity (xv6)
        - Change existing xv6 fs:
                - add protection from data corruption
        1) modify code to allow user to create new type of file that
           keeps checksum for each block it points to
        2) change fs `read()` and `write()` handled differently for
           files w/ checksums
                - `write()`:  checksum every block of file
                - `read()`:  check & make sure block still matches
                stored checksum
                        - rc = -1 if not
        3) modify `stat()` syscall to dump info about file:
                - write prog
                        - given filename
                        - prints out filesize, etc
                        - also info about checksums
        1.  understand how fs laid out on disk
                - use `fs.h`:
                        - superblock
                        - log
                        - inodes
                        - single block in-use bitmap
                        - data blocks
        2.  understand what each inode looks like
                - use `fs.h`
                        - filetype (reg file/dir)
                        - maj/min dev type (know which disk to read)
                        - reference counter (num dirs file is linked in)
                        - size (bytes)
                        - block addresses * (NDIRECT + 1)
                                - first NDIRECT addrs are direct ptrs
                                  to first NDIRECT blocks of file (if
                                  file is that big)
                                - last block (larger files):  indirect block
                                        - may contain more ptrs to blocks
                                          for larger files
                - keep inode struct as-is, but use slots for direct ptrs
                  as `(checksum, pointer)` pairs
                        - use each direct ptr slot (4 bytes) as a
                          1-byte checksum, 3-byte ptr) (limits disk
                          addresses fs can refer to 2^24)
                        - 1-byte checksum is `XOR`
                                - compute:
                                        - XOR all bytes of block
                                        - store in ptr to block
                                        - every time block written
                                - `read()`:
                                        - read in the block
                                        - compute checksum
                                        - compare to stored checksum
                                        - match:  return as usual
                                        - not:  rc = -1, don't return
                                          corrupt data
                                        - does not make replicas of
                                          blocks (doesn't need to
                                          recover)
                        - note: large files need to have direct ptrs of
                          indirect ptr (which need checksums)
                - 2 types of files:
                        1.  existing ptr-based
                        2.  new checksum-based
                                - add new file type:
                                - `stat.h`: `T_DIR`, `T_FILE`, add
                                  `T_CHECKED` (set to 4)
                                - check if files are type `T_CHECKED`,
                                  checks checksum
                                - create:
                                        - modify interface to file
                                          creation
                                        - add `O_CHECKED` flag (0x400)
                                          to `open()` syscall
                                        - when flag is present, fs
                                          should create checksum-based
                                          file
                                        - routines deeper in fs have
                                          to be modified to be passed
                                          new flag, use accordingly
        - no need to do for directories
        - follow pathts of `read()` and `write()` syscalls
        - also look at `bmap()`
        - modify `stat()` syscall to return infor about actual disk
          addrs in inode
                - `stat()` doesn't return such info
                - use `newstatstruct.h`
                        - has room to return exactly 1 byte of
                          checksum info about file
                        - XOR all of existing checksums over blocks of
                          file
                        - return in `checksum` field of stat struct
        - create new prog `filestat`
                - called:  `filestat pathname`
                        - prints out all info about file:
                                - type
                                - size
                                - new checksum
For Linux part, copy checker into `linux` subdir
For xv6 part, copy src files into `xv6` subdir of `p4` directory
Submit to canvas single archive (tar/zip) w/ all xv6 files +
`readme-eckard` file

Makefile:
- Changed `CFLAGS`'s `-O2` to `-Og` for debugging.
- Added `kthreads.o` to `ULIB`.
- Added `_testkthreads`, `_testthr` to `UPROGS`.
- Added `testkthreads.c`, `testthr.c` to `EXTRA`.

defines.h:
- Created for most common `#defines`.

linux/Makefile:
- Created to make fscheck

linux/fscheck.c:
- Code for fscheck program

linux/check*.[ch], linux/*error.[ch]:
- Code to support fscheck program

proc.c:
- Added `clone()`, `join()`, and corresponding syscall functions.

readme-eckard.md:
- This file, explaining all files that were changed or created.

syscall.c:
- Referenced external functions `sys_clone()` and `sys_join()`.
- Appended `sys_clone` and `sys_join` to `syscalls`.

syscall.h:
- Set `SYS_clone` to `22`.
- Set `SYS_join` to `23`.

testthr.c:
- Program created for more tests for Project 3.

usys.S:
- Appended `clone` and `join` to `SYSCALL` macro calls.
