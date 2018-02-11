Changes made to original xv6 source code for Project 2
===

NOTES:

Makefile:
- Appended `_derefnull` and `_proj2test` to `UPROGS`.
- Added `derefnull.c` and `proj2test.c` to `EXTRA`.

derefnull.c:
- Created to test dereferencing a NULL pointer.

readme-eckard.md:
- This file, explaining all files that were changed or created.

syscall.c:
- Referenced external functions `sys_shmem_access()` and
`sys_shmem_count`.
- Appended `sys_shmem_access` and `sys_shmem_count` to `syscalls`.

syscall.h:
- Set `SYS_shmem_access` to `22` and `SYS_shmem_count` to `23`.

testshmem.c:
- Program created to have several tests against `shmem_*()` syscalls.

usys.S:
- Appended `shmem_access; and `shmem_count` to `SYSCALL` macro calls.

vm.c:
- Implementation of `sys_shmem_count()` and `sys_shmem_access()`
functions.

---

defs.h:
- Prototyped `struct procinfo` and `getprocsinfo()` from `proc.c`.

proc.c:
- Created function `getprocsinfo()` that is used by the system call.
This is where the majority of the stuff happens.

proc.h:
- Appended definition of `struct procinfo`.

sysproc.c:
- Added `getprocsinfo` syscall as `sys_getprocsinfo()`, which calls
`getprocsinfo()`.

user.h:
- Added `struct procinfo` declaration.
- Appended `getprocsinfo` prototype.
