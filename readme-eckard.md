Changes made to original xv6 source code in order to add syscall getprocsinfo():
---

NOTE:  `Project1.pdf` refers to `getprocsinfo()` and `getprocinfo()`,
but for consistancy I'm only using `getprocsinfo()`.  It also refers to
`testgetprocinfo` and `testprocinfo`, but I called my program, file,
and function `testgetprocsinfo` to follow the name of the function it
was testing.

Makefile:
- Appended `_testgetprocsinfo` to `UPROGS`
- Added `testgetprocsinfo.c` to `EXTRA`

syscall.h:
- Set `SYS_getprocsinfo` to `22`

syscall.c:
- Appended `sys_getprocsinfo` to `syscalls`
- Referenced external function `sys_getprocsinfo()`

sysproc.c:
- Added `getprocsinfo` syscall as `sys_getprocsinfo()`, which calls
`getprocsinfo()`

usys.S:
- Appended `getprocsinfo` to `SYSCALL` macro calls

user.h:
- Appended `getprocsinfo` prototype

testgetprocsinfo.c:
- Created to have several tests against `getprocsinfo()` syscall

proc.h:
- Appended definition of `struct procinfo`

mmu.h:
- Surrounded file with include guard `MMU_H`

proc.c:
- Created function `getprocsinfo()` that is used by the system call.
This is where the majority of the stuff happens.

defs.h:
- Prototyped `getprocsinfo()` and `struct procinfo` from `proc.c`
