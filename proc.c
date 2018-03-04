#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "defines.h"   // NULL, TRUE, FALSE, SUCCESS, RC_ERR, STDOUT
#include "spinlock.h"

struct {
        struct spinlock lock;
        struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
int nextthreadid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void pinit(void)
{
        initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int cpuid()
{
        return mycpu() - cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu *mycpu(void)
{
        int apicid, i;

        if (readeflags() & FL_IF)
                panic("mycpu called with interrupts enabled\n");

        apicid = lapicid();
        // APIC IDs are not guaranteed to be contiguous. Maybe we should have
        // a reverse map, or reserve a register to store &cpus[i].
        for (i = 0; i < ncpu; ++i) {
                if (cpus[i].apicid == apicid)
                        return &cpus[i];
        }
        panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc *myproc(void)
{
        struct cpu *c;
        struct proc *p;

        pushcli();
        c = mycpu();
        p = c->proc;
        popcli();
        return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc *allocproc(void)
{
        struct proc *p;
        char *sp;

        acquire(&ptable.lock);

        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                if (p->state == UNUSED) {
                        goto found;
                }
        }

        release(&ptable.lock);
        return 0;

found:
        p->state = EMBRYO;
        p->pid = nextpid++;

        release(&ptable.lock);

        // Allocate kernel stack.
        if ((p->kstack = kalloc()) == 0) {
                p->state = UNUSED;
                return 0;
        }
        sp = p->kstack + KSTACKSIZE;

        // Leave room for trap frame.
        sp -= sizeof *p->tf;
        p->tf = (struct trapframe *)sp;

        // Set up new context to start executing at forkret,
        // which returns to trapret.
        sp -= 4;
        *(uint *) sp = (uint) trapret;

        sp -= sizeof *p->context;
        p->context = (struct context *)sp;
        memset(p->context, 0, sizeof *p->context);
        p->context->eip = (uint) forkret;

        return p;
}

//PAGEBREAK: 32
// Set up first user process.
void userinit(void)
{
        struct proc *p;
        extern char _binary_initcode_start[], _binary_initcode_size[];

        p = allocproc();

        initproc = p;
        if ((p->pgdir = setupkvm()) == 0)
                panic("userinit: out of memory?");
        inituvm(p->pgdir, _binary_initcode_start,
                (int)_binary_initcode_size);
        p->sz = PGSIZE;
        memset(p->tf, 0, sizeof(*p->tf));
        p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
        p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
        p->tf->es = p->tf->ds;
        p->tf->ss = p->tf->ds;
        p->tf->eflags = FL_IF;
        p->tf->esp = PGSIZE;
        p->tf->eip = 0;         // beginning of initcode.S

        safestrcpy(p->name, "initcode", sizeof(p->name));
        p->cwd = namei("/");

        // this assignment to p->state lets other cores
        // run this process. the acquire forces the above
        // writes to be visible, and the lock is also needed
        // because the assignment might not be atomic.
        acquire(&ptable.lock);

        p->state = RUNNABLE;

        release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n)
{
        uint sz;
        struct proc *curproc = myproc();

        sz = curproc->sz;
        if (n > 0) {
                if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
                        return -1;
        } else if (n < 0) {
                if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
                        return -1;
        }
        curproc->sz = sz;
        switchuvm(curproc);
        return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int fork(void)
{
        int i, pid;
        struct proc *np;
        struct proc *curproc = myproc();

        // Allocate process.
        if ((np = allocproc()) == 0) {
                return -1;
        }
        // Copy process state from proc.
        if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
                kfree(np->kstack);
                np->kstack = 0;
                np->state = UNUSED;
                return -1;
        }
        np->sz = curproc->sz;
        np->parent = curproc;
        *np->tf = *curproc->tf;

        // Clear %eax so that fork returns 0 in the child.
        np->tf->eax = 0;

        for (i = 0; i < NOFILE; i++)
                if (curproc->ofile[i])
                        np->ofile[i] = filedup(curproc->ofile[i]);
        np->cwd = idup(curproc->cwd);

        safestrcpy(np->name, curproc->name, sizeof(curproc->name));

        pid = np->pid;

        acquire(&ptable.lock);

        np->state = RUNNABLE;

        release(&ptable.lock);

        return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
// TODO:  Work the same with threads
void exit(void)
{
        struct proc *curproc = myproc();
        struct proc *p;
        int fd;

        if (curproc == initproc)
                panic("init exiting");

        // Close all open files.
        for (fd = 0; fd < NOFILE; fd++) {
                if (curproc->ofile[fd]) {
                        fileclose(curproc->ofile[fd]);
                        curproc->ofile[fd] = 0;
                }
        }

        begin_op();
        iput(curproc->cwd);
        end_op();
        curproc->cwd = 0;

        acquire(&ptable.lock);

        // Parent might be sleeping in wait().
        wakeup1(curproc->parent);

        // Pass abandoned children to init.
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                if (p->parent == curproc) {
                        p->parent = initproc;
                        if (p->state == ZOMBIE)
                                wakeup1(initproc);
                }
        }

        // Jump into the scheduler, never to return.
        curproc->state = ZOMBIE;
        sched();
        panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
// TODO:  wait for child proc that doesn't share addr space with this
// proc.  Also, free addr space if this is last ref to it
int wait(void)
{
        struct proc *p;
        int havekids, pid;
        struct proc *curproc = myproc();

        acquire(&ptable.lock);
        for (;;) {
                // Scan through table looking for exited children.
                havekids = 0;
                for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                        if (p->parent != curproc)
                                continue;
                        havekids = 1;
                        if (p->state == ZOMBIE) {
                                // Found one.
                                pid = p->pid;
                                kfree(p->kstack);
                                p->kstack = 0;
                                freevm(p->pgdir);
                                p->pid = 0;
                                p->parent = 0;
                                p->name[0] = 0;
                                p->killed = 0;
                                p->state = UNUSED;
                                release(&ptable.lock);
                                return pid;
                        }
                }

                // No point waiting if we don't have any children.
                if (!havekids || curproc->killed) {
                        release(&ptable.lock);
                        return -1;
                }
                // Wait for children to exit.  (See wakeup1 call in proc_exit.)
                sleep(curproc, &ptable.lock);   //DOC: wait-sleep
        }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void scheduler(void)
{
        struct proc *p;
        struct cpu *c = mycpu();

        c->proc = 0;

        for (;;) {
                // Enable interrupts on this processor.
                sti();

                // Loop over process table looking for process to run.
                acquire(&ptable.lock);
                for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                        if (p->state != RUNNABLE)
                                continue;

                        // Switch to chosen process.  It is the process's job
                        // to release ptable.lock and then reacquire it
                        // before jumping back to us.
                        c->proc = p;
                        switchuvm(p);
                        p->state = RUNNING;

                        swtch(&(c->scheduler), p->context);
                        switchkvm();

                        // Process is done running for now.
                        // It should have changed its p->state before coming back.
                        c->proc = 0;
                }
                release(&ptable.lock);

        }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void)
{
        int intena;
        struct proc *p = myproc();

        if (!holding(&ptable.lock))
                panic("sched ptable.lock");
        if (mycpu()->ncli != 1)
                panic("sched locks");
        if (p->state == RUNNING)
                panic("sched running");
        if (readeflags() & FL_IF)
                panic("sched interruptible");
        intena = mycpu()->intena;
        swtch(&p->context, mycpu()->scheduler);
        mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void yield(void)
{
        acquire(&ptable.lock);  //DOC: yieldlock
        myproc()->state = RUNNABLE;
        sched();
        release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void)
{
        static int first = 1;

        // Still holding ptable.lock from scheduler.
        release(&ptable.lock);

        if (first) {
                // Some initialization functions must be run in the context
                // of a regular process (e.g., they call sleep), and thus cannot
                // be run from main().
                first = 0;
                iinit(ROOTDEV);
                initlog(ROOTDEV);
        }
        // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void sleep(void *chan, struct spinlock *lk)
{
        struct proc *p = myproc();

        if (p == 0)
                panic("sleep");

        if (lk == 0)
                panic("sleep without lk");

        // Must acquire ptable.lock in order to
        // change p->state and then call sched.
        // Once we hold ptable.lock, we can be
        // guaranteed that we won't miss any wakeup
        // (wakeup runs with ptable.lock locked),
        // so it's okay to release lk.
        if (lk != &ptable.lock) {       //DOC: sleeplock0
                acquire(&ptable.lock);  //DOC: sleeplock1
                release(lk);
        }
        // Go to sleep.
        p->chan = chan;
        p->state = SLEEPING;

        sched();

        // Tidy up.
        p->chan = 0;

        // Reacquire original lock.
        if (lk != &ptable.lock) {       //DOC: sleeplock2
                release(&ptable.lock);
                acquire(lk);
        }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void wakeup1(void *chan)
{
        struct proc *p;

        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
                if (p->state == SLEEPING && p->chan == chan)
                        p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan)
{
        acquire(&ptable.lock);
        wakeup1(chan);
        release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid)
{
        struct proc *p;

        acquire(&ptable.lock);
        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                if (p->pid == pid) {
                        p->killed = 1;
                        // Wake process from sleep if necessary.
                        if (p->state == SLEEPING)
                                p->state = RUNNABLE;
                        release(&ptable.lock);
                        return 0;
                }
        }
        release(&ptable.lock);
        return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void procdump(void)
{
        static char *states[] = {
                [UNUSED]   "unused",
                [EMBRYO]   "embryo",
                [SLEEPING] "sleep ",
                [RUNNABLE] "runble",
                [RUNNING]  "run   ",
                [ZOMBIE]   "zombie"
        };
        int i;
        struct proc *p;
        char *state;
        uint pc[10];

        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                if (p->state == UNUSED)
                        continue;
                if (p->state >= 0 && p->state < NELEM(states)
                    && states[p->state])
                        state = states[p->state];
                else
                        state = "???";
                cprintf("%d %s %s", p->pid, state, p->name);
                if (p->state == SLEEPING) {
                        getcallerpcs((uint *) p->context->ebp + 2, pc);
                        for (i = 0; i < 10 && pc[i] != 0; i++)
                                cprintf(" %p", pc[i]);
                }
                cprintf("\n");
        }
}

static inline void cp_code_and_data(struct proc *oldproc,
                                    struct proc *newproc)
{
        newproc->sz = oldproc->sz;

        // Copy process state from proc.
        /*
           if ((newproc->pgdir = copyuvm(oldproc->pgdir, oldproc->sz)) == 0) {
           kfree(newproc->kstack);
           newproc->kstack = 0;
           newproc->state = UNUSED;
           return -1;
           }
         */

        newproc->pgdir = oldproc->pgdir;
        *newproc->tf = *oldproc->tf;

        for (int i = 0; i < NOFILE; i++) {
                if (oldproc->ofile[i]) {
                        newproc->ofile[i] = filedup(oldproc->ofile[i]);
                }
        }

        newproc->cwd = idup(oldproc->cwd);

        safestrcpy(newproc->name, oldproc->name, sizeof(oldproc->name));

}

// TODO:  Change from fork() to clone()
// Most of clone() was based on fork()
int clone(void (*fcn)(void *), void *arg, void *stack)
{
        int pid;

        uint ustack[] = {0xffffffff, (uint)arg};
        struct proc *newproc;
        struct proc *curproc = myproc();

        // Check stack alignment
        if ((0 != ((uint) stack % PGSIZE)
             || PGSIZE > (curproc->sz - (uint) stack))) {
                return -1;
        }
        // Allocate process.
        if (0 == (newproc = allocproc())) {
                return -1;
        }

        // A thread has same code and data as parent, but
        // different stack:
        cp_code_and_data(curproc, newproc);

        //newproc->threadstack = (uint)stack;
        //newproc->threadstack = (char *)stack;
        //newproc->kstack = (char *)stack;
        newproc->kstack = stack;

        // state done at the end, locked

        // TODO:  error if multiple procs (threads) w/ same pid?
        // Use pid from parent thread
        pid = newproc->pid;

        newproc->parent = curproc;

        // trapframe same, except for a few things
        newproc->tf->ebp = newproc->tf->esp;
        // Clear %eax so that clone (fork) returns 0 in the child.
        newproc->tf->eax = 0;
        newproc->tf->eip = (uint) fcn;
        /*
           */
           //newproc->tf->esp = (uint)stack+PGSIZE;
           // TODO:  (2)*4 is some "magic number"
           //newproc->tf->esp = stack + PGSIZE - (2)*4;
           newproc->tf->esp = (uint)stack + PGSIZE - 2 * sizeof(void *);

           //ustack[0] = 0xffffffff;
           //ustack[1] = (uint)arg;
           // TODO:  (2)*4 is some "magic number"
           copyout(newproc->pgdir, newproc->tf->esp, ustack, 2 * sizeof(void *));
           /*
         */
        newproc->tf->esp = (uint) stack;

        // context, chan, killed is part of stack

        newproc->threadid = nextthreadid++;

        // TODO:  threadstack?  (commented above, kstack)

        acquire(&ptable.lock);

        newproc->state = RUNNABLE;

        release(&ptable.lock);

        return pid;
}

// Syscall:  Create a kernel thread which shares the calling process'
// addres space and file discriptors.  New process uses `stack` as its
// user stack, which is passed arg and uses a fake return PC (0xffffffff).
// Stack is 1 aligned page.  Thread starts executing at address of `fcn`.
// Return is PID of new thread.
// prototype:  int clone(void(*fcn)(void *), void *arg, void *stack)
int sys_clone(void)
{
        int rc = 0;
        void *arg = (void *)0;
        void *stack = (void *)0;
        void (*fcn)(void*) = (void *)0;

        //if (0 > (rc = argptr(0, (void *)&fcn, sizeof(fcn)))) {
        if (0 > (rc = argptr(0, (char **)fcn, sizeof(fcn)))) {
                return rc;
        }

        //if (0 > (rc = argptr(1, (void *)&arg, sizeof(arg)))) {
        if (0 > (rc = argptr(1, arg, sizeof(arg)))) {
                return rc;
        }

        //if (0 > (rc = argptr(2, (void *)&stack, sizeof(stack)))) {
        if (0 > (rc = argptr(2, stack, sizeof(stack)))) {
                return rc;
        }

        return clone(fcn, arg, stack);
}

// TODO:  Change from wait() to join()
// Based on wait()
int join(int pid)
{
        //int wait(void)
        {
                struct proc *p;
                //int havekids, pid;
                int exists = FALSE;
                struct proc *curproc = myproc();

                acquire(&ptable.lock);

                do {
                        // Scan through table looking for child thread.
                        //havekids = 0;
                        exists = FALSE;

                        for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
                                //if (p->parent != curproc)
                                if (p->pid == pid) {
                                        if (p->parent != curproc) {
                                                release(&ptable.lock);
                                                return RC_ERR;
                                        }
                                        //continue;
                                        // found it!
                                        exists = TRUE;
                                        break;
                                }

                                /*
                                //havekids = 1;
                                if (p->state == ZOMBIE) {
                                        // Found one.
                                        pid = p->pid;
                                        kfree(p->kstack);
                                        p->kstack = 0;
                                        freevm(p->pgdir);
                                        p->pid = 0;
                                        p->parent = 0;
                                        p->name[0] = 0;
                                        p->killed = 0;
                                        p->state = UNUSED;
                                        release(&ptable.lock);
                                        return pid;
                                }
                                */
                        }

                        /*
                        // No point waiting if we don't have any children.
                        if (!havekids || curproc->killed) {
                                release(&ptable.lock);
                                return -1;
                        }
                        */

                        // Wait for child thread to exit.  (See wakeup1 call in proc_exit.)
                        if (exists) {
                                sleep(curproc, &ptable.lock);
                        }
                } while (exists);
        }

        release(&ptable.lock);
        return SUCCESS;
}

// Syscall:  Wait for a kernel thread, then join to the parent.
// Returns 0 (JOIN_SUCCESS) or -1 (JOIN_FAILURE)
// prototype:  int join(int pid)
int sys_join(void)
{
        int rc = 0;
        int pid = 0;

        if (0 > (rc = argint(0, &pid))) {
                return rc;
        }

        return join(pid);
}
