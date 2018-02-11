#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
        char *s, *last;
        int i, off;
        uint argc, sz, sp, ustack[3 + MAXARG + 1];
        struct elfhdr elf;
        struct inode *inode_p;
        struct proghdr prog_hdr;
        pde_t *pgdir, *oldpgdir;
        struct proc *curproc = myproc();

        begin_op();

        if ((inode_p = namei(path)) == 0) {
                end_op();
                cprintf("exec: fail\n");
                return -1;
        }
        ilock(inode_p);
        pgdir = 0;

        // Check ELF header
        if (readi(inode_p, (char *)&elf, 0, sizeof(elf)) != sizeof(elf))
                goto bad;
        if (elf.magic != ELF_MAGIC)
                goto bad;

        if ((pgdir = setupkvm()) == 0)
                goto bad;

        // Load program into memory.
        sz = 0;
        for (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(prog_hdr)) {
                if (readi(inode_p, (char *)&prog_hdr, off, sizeof(prog_hdr)) != sizeof(prog_hdr))
                        goto bad;
                if (prog_hdr.type != ELF_PROG_LOAD)
                        continue;
                if (prog_hdr.memsz < prog_hdr.filesz)
                        goto bad;
                if (prog_hdr.vaddr + prog_hdr.memsz < prog_hdr.vaddr)
                        goto bad;
                if ((sz = allocuvm(pgdir, sz, prog_hdr.vaddr + prog_hdr.memsz)) == 0)
                        goto bad;
                if (prog_hdr.vaddr % PGSIZE != 0)
                        goto bad;
                if (loaduvm(pgdir, (char *)prog_hdr.vaddr, inode_p, prog_hdr.off, prog_hdr.filesz)
                    < 0)
                        goto bad;
        }
        iunlockput(inode_p);
        end_op();
        inode_p = 0;

        // Allocate two pages at the next page boundary.
        // Make the first inaccessible.  Use the second as the user stack.
        sz = PGROUNDUP(sz);
        if ((sz = allocuvm(pgdir, sz, sz + 2 * PGSIZE)) == 0)
                goto bad;
        clearpteu(pgdir, (char *)(sz - 2 * PGSIZE));
        sp = sz;

        // Push argument strings, prepare rest of stack in ustack.
        for (argc = 0; argv[argc]; argc++) {
                if (argc >= MAXARG)
                        goto bad;
                sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
                if (copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1)
                    < 0)
                        goto bad;
                ustack[3 + argc] = sp;
        }
        ustack[3 + argc] = 0;

        ustack[0] = 0xffffffff; // fake return PC
        ustack[1] = argc;
        ustack[2] = sp - (argc + 1) * 4;        // argv pointer

        sp -= (3 + argc + 1) * 4;
        if (copyout(pgdir, sp, ustack, (3 + argc + 1) * 4) < 0)
                goto bad;

        // Save program name for debugging.
        for (last = s = path; *s; s++)
                if (*s == '/')
                        last = s + 1;
        safestrcpy(curproc->name, last, sizeof(curproc->name));

        // Commit to the user image.
        oldpgdir = curproc->pgdir;
        curproc->pgdir = pgdir;
        curproc->sz = sz;
        curproc->tf->eip = elf.entry;   // main
        curproc->tf->esp = sp;
        switchuvm(curproc);
        freevm(oldpgdir);
        return 0;

      bad:
        if (pgdir)
                freevm(pgdir);
        if (inode_p) {
                iunlockput(inode_p);
                end_op();
        }
        return -1;
}
