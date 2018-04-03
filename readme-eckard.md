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
Part B:
        1) filesystem checker (Linux, mumble lab)
                - reads filesystem image
                - makes sure consistent
                - if not consistent, takes steps to fix problems it
                  sees, w/o fixing
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
        2) filesystem integrity (xv6)

Makefile:
- Changed `CFLAGS`'s `-O2` to `-Og` for debugging.
- Added `kthreads.o` to `ULIB`.
- Added `_testkthreads`, `_testthr` to `UPROGS`.
- Added `testkthreads.c`, `testthr.c` to `EXTRA`.

defines.h:
- Created for most common `#defines`.

kthreads.[ch]:
- Created to implement `kthreads` as a library.

proc.c:
- Added `clone()`, `join()`, and corresponding syscall functions.
- `exit()`:  for threads, keep file descriptors open.

proc.h:
- Added `threadid`, `threadstack` to `struct proc`.
- Added enum of join states.

readme-eckard.md:
- This file, explaining all files that were changed or created.

syscall.c:
- Referenced external functions `sys_clone()` and `sys_join()`.
- Appended `sys_clone` and `sys_join` to `syscalls`.

syscall.h:
- Set `SYS_clone` to `22`.
- Set `SYS_join` to `23`.

testkthreads.c:
- Program created by our TA, Tyler Allen to test Project 3.

testthr.c:
- Program created for more tests for Project 3.

user.h:
- Added prototypes for `clone()` and `join()`.

usys.S:
- Appended `clone` and `join` to `SYSCALL` macro calls.
