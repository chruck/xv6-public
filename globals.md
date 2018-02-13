Global variables
===
`_binary_fs_img_size`:  memide.c, uchar array
`_binary_fs_img_start`:  memide.c, uchar array
`base`:  umalloc.c, a header?
`bcache`:  bio.c, disk block cache
`cons`:  console.c, console lock
`cpus`:  mp.c, array of cpu structs
`data`:  vm.c, "string" from kernel.ld
`devsw`:  file.c, array of devsw?
`disksize`:  memide.c, size of fake ide disk
`end`:  kalloc.c/main.c, first addr after kernel
`entrypgdir`:  main.c, an array of pde_t?  entry.S's page directory?
`freep`:  umalloc.c, a header pointer?
`ftable`:  file.c, array of files
`havedisk1`:  ide.c, int
`idelock`:  ide.c, lock on ide?
`idequeue`:  ide.c, pointer to a buffer
`idt`:  trap.c, interrupt descriptor table
`initproc`:  proc.c, initial process?
`ioapic`:  ioapic.c, pointer to ioapic struct
`ioapicid`:  mp.c, id of i/o?
`kmem`:  kalloc.c, kernel memory?
`kmap`:  vm.c, table of kernel mappings for every proc's page table
`kpgdir`:  main.c/vm.c, a pde_t?  kernel page directory? for scheduler()
`lapic`:  lapic.c, pointer to uint (i/o interrupts?)
`log`:  log.c, simple log
`memdisk`:  memide.c, pointer to uchar, the fake disk?
`ncpu`:  mp.c, number of cpus?
`nextpid`:  proc.c, next pid
`panicked`:  console.c, ?
`ptable`:  proc.c, table of processes
`sb`:  fs.c, superblock
`ticks`:  trap.c, uint?
`tickslock`:  trap.c, a spinlock
`uart`:  uart.c, "bool" if uart exists?
`vectors`:  trap.c, entry pointers
