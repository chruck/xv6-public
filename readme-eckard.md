Changes made to original xv6 source code for Project 3
===

NOTES:
Project 3 refers to a thread struct called `kthread`, and the TA test
code `testkthreads.c` uses the naming convention of `kthread_t`, so I
have created both, a `struct kthread` that has a typedef of
`kthread_t`.  Also, because the assignment asked for `lock_init()`,
but the test code calls `init_lock()`, the latter simply calls the
former.  Also, I couldn't stand it any longer, I had to have K&R-style
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
