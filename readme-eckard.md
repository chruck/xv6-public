Changes made to original xv6 source code for Project 3
===

NOTES:
Project 3 refers to a thread struct called `kthread`, and the TA test
code `testkthreads.c` uses the naming convention of `kthread_t`, so I
have created both, a `struct kthread` that has a typedef of
`kthread_t`.  Also, because the assignment asked for `lock_init()`,
but the test code calls `init_lock()`, the latter simply calls the former.

Makefile:
- Changed `CFLAGS`'s `-O2` to `-Og` for debugging.
- Added `kthreads.o` to `ULIB`.
- Added `_testkthreads` to `UPROGS`.
- Added `testkthreads.c` to `EXTRA`.

kthreads.[ch]:
- Created to implement kthreads as a library.

proc.c:
- Added `clone()`, `join()`, and corresponding syscall functions

proc.h:
- Added `thread_num` to `struct proc`.
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

usys.S:
- Appended `clone` and `join` to `SYSCALL` macro calls.
