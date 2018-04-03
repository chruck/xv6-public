Changes made to original xv6 source code for Project 4
===

NOTES:
- Like Project 3, this version of the source code uses K&R-style
  indentation:  The only difference to the files not mentioned below is
  indentation, for my own readability.

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
