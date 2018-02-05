Changes made to original xv6 source code for Project 2
===

NOTES:

Makefile:
- Appended `_derefnull` and `_proj2test` to `UPROGS`.
- Added `derefnull.c` and `proj2test.c` to `EXTRA`.

defs.h:
- Prototyped `struct procinfo` and `getprocsinfo()` from `proc.c`.

proc.c:
- Created function `getprocsinfo()` that is used by the system call.
This is where the majority of the stuff happens.

proc.h:
- Appended definition of `struct procinfo`.

readme-eckard.md:
- This file, explaining all files that were changed or created.

syscall.h:
- Set `SYS_getprocsinfo` to `22`.

syscall.c:
- Referenced external function `sys_getprocsinfo()`.
- Appended `sys_getprocsinfo` to `syscalls`.

sysproc.c:
- Added `getprocsinfo` syscall as `sys_getprocsinfo()`, which calls
`getprocsinfo()`.

testgetprocsinfo.c:
- Created to have several tests against `getprocsinfo()` syscall.

user.h:
- Added `struct procinfo` declaration.
- Appended `getprocsinfo` prototype.

usys.S:
- Appended `getprocsinfo` to `SYSCALL` macro calls.
