Changes made to original xv6 source code for Project 3
===

Makefile:
- Changed `CFLAGS`'s `-O2` to `-Og` for debugging.
- Added `systhread.o` to `OBJS`

readme-eckard.md:
- This file, explaining all files that were changed or created.

syscall.c:
- Referenced external functions `sys_clone()` and `sys_join()`.
- Appended `sys_clone` and `sys_join` to `syscalls`.

syscall.h:
- Set `SYS_clone` to `22`.
- Set `SYS_join` to `23`.

systhread.c:
- Created to implement `sys_clone()` and `sys_thread()`.

/*
thread.c:
- Created as a thread library
*/

usys.S:
- Appended `clone` and `join` to `SYSCALL` macro calls.

---

NOTES:
- `Project1.pdf` refers to `getprocsinfo()` and `getprocinfo()`, but
for consistancy I'm only using `getprocsinfo()`.
- It also refers to `testgetprocinfo` and `testprocinfo`, but I called
my program, file, and function `testgetprocsinfo` to follow the name of
the function it was testing.
- Furthermore, I changed the prototype to:
```
        int getprocsinfo(struct procinfo **)
```
because the `procinfo` struct only has "two data members, an integer
__pid__ and a string __pname__" but the requirement for the syscall is
that it "returns the PID value and process name for each of the
processes that exists in the system at the time of the call", thus
requiring an "array".

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

testgetprocsinfo.c:
- Created to have several tests against `getprocsinfo()` syscall.

user.h:
- Added `struct procinfo` declaration.
- Appended `getprocsinfo` prototype.

===

NOTES:

Makefile:
- Appended `_testgetprocsinfo` to `UPROGS`.
- Added `testgetprocsinfo.c` to `EXTRA`.
- Appended `_testderefnull` and `_testshmem` to `UPROGS`.
- Added `testderefnull.c` and `testshmem.c` to `EXTRA`.

proj2tests.c:
- Program created to test Project 2 Part A and Part B modifications
(`testderefnull` and `testshmem`).


testderefnull.c:
- Program created to test dereferencing a NULL pointer.

testshmem.c:
- Program created to have several tests against `shmem_*()` syscalls.

vm.c:
- Implementation of `sys_shmem_count()` and `sys_shmem_access()`
functions.
