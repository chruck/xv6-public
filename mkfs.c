#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat           // avoid clash with host struct stat
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "param.h"

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

int nbitmap = FSSIZE / (BSIZE * 8) + 1;
int ninodeblocks = NINODES / IPB + 1;
int nlog = LOGSIZE;
int nmeta;                      // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks;                    // Number of data blocks

int fsfd;
struct superblock sb;
char zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

void balloc(int);
void wsect(uint, void *);
void winode(uint, struct dinode *);
void rinode(uint inodenum, struct dinode *in_inode_p);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inodenum, void *p, int n);

// convert to intel byte order
ushort xshort(ushort x)
{
        ushort y;
        uchar *a = (uchar *) & y;

        a[0] = x;
        a[1] = x >> 8;
        return y;
}

uint xint(uint x)
{
        uint y;
        uchar *a = (uchar *) & y;

        a[0] = x;
        a[1] = x >> 8;
        a[2] = x >> 16;
        a[3] = x >> 24;
        return y;
}

int main(int argc, char *argv[])
{
        int i, bytesread, fd;
        uint rootino, inodenum, off;
        struct dirent direntry;
        char buf[BSIZE];
        struct dinode inode;

        static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

        if (argc < 2) {
                fprintf(stderr, "Usage: mkfs fs.img files...\n");
                exit(1);
        }

        assert((BSIZE % sizeof(struct dinode)) == 0);
        assert((BSIZE % sizeof(struct dirent)) == 0);

        fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
        if (fsfd < 0) {
                perror(argv[1]);
                exit(1);
        }
        // 1 fs block = 1 disk sector
        nmeta = 2 + nlog + ninodeblocks + nbitmap;
        nblocks = FSSIZE - nmeta;

        sb.size = xint(FSSIZE);
        sb.nblocks = xint(nblocks);
        sb.ninodes = xint(NINODES);
        sb.nlog = xint(nlog);
        sb.logstart = xint(2);
        sb.inodestart = xint(2 + nlog);
        sb.bmapstart = xint(2 + nlog + ninodeblocks);

        printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n", nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

        freeblock = nmeta;      // the first free block that we can allocate

        // initialize the whole filesystem
        for (i = 0; i < FSSIZE; i++)
                wsect(i, zeroes);

        // write the superblock
        memset(buf, 0, sizeof(buf));
        memmove(buf, &sb, sizeof(sb));
        wsect(1, buf);

        // create inode for '/'
        rootino = ialloc(T_DIR);
        assert(rootino == ROOTINO);

        // ... with '.'
        bzero(&direntry, sizeof(direntry));
        direntry.inum = xshort(rootino);
        strcpy(direntry.name, ".");
        iappend(rootino, &direntry, sizeof(direntry));

        // ... and '..'
        bzero(&direntry, sizeof(direntry));
        direntry.inum = xshort(rootino);
        strcpy(direntry.name, "..");
        iappend(rootino, &direntry, sizeof(direntry));

        for (i = 2; i < argc; i++) {
                assert(index(argv[i], '/') == 0);

                if ((fd = open(argv[i], 0)) < 0) {
                        perror(argv[i]);
                        exit(1);
                }
                // Skip leading _ in name when writing to file system.
                // The binaries are named _rm, _cat, etc. to keep the
                // build operating system from trying to execute them
                // in place of system binaries like rm and cat.
                if (argv[i][0] == '_')
                        ++argv[i];

                inodenum = ialloc(T_FILE);

                bzero(&direntry, sizeof(direntry));
                direntry.inum = xshort(inodenum);
                strncpy(direntry.name, argv[i], DIRSIZ);
                iappend(rootino, &direntry, sizeof(direntry));

                while ((bytesread = read(fd, buf, sizeof(buf))) > 0)
                        iappend(inodenum, buf, bytesread);

                close(fd);
        }

        // fix size of root inode dir
        rinode(rootino, &inode);
        off = xint(inode.size);
        off = ((off / BSIZE) + 1) * BSIZE;
        inode.size = xint(off);
        winode(rootino, &inode);

        balloc(freeblock);

        exit(0);
}

void wsect(uint sec, void *buf)
{
        if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE) {
                perror("lseek");
                exit(1);
        }
        if (write(fsfd, buf, BSIZE) != BSIZE) {
                perror("write");
                exit(1);
        }
}

void winode(uint inodenum, struct dinode *in_inode_p)
{
        char buf[BSIZE];
        uint blocknum;
        struct dinode *inode_p;

        blocknum = IBLOCK(inodenum, sb);
        rsect(blocknum, buf);
        inode_p = ((struct dinode *)buf) + (inodenum % IPB);
        *inode_p = *in_inode_p;
        wsect(blocknum, buf);
}

void rinode(uint inodenum, struct dinode *in_inode_p)
{
        char buf[BSIZE];
        uint blocknum;
        struct dinode *inode_p;

        blocknum = IBLOCK(inodenum, sb);
        rsect(blocknum, buf);
        inode_p = ((struct dinode *)buf) + (inodenum % IPB);
        *in_inode_p = *inode_p;
}

void rsect(uint sec, void *buf)
{
        if (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE) {
                perror("lseek");
                exit(1);
        }
        if (read(fsfd, buf, BSIZE) != BSIZE) {
                perror("read");
                exit(1);
        }
}

uint ialloc(ushort type)
{
        uint inodenum = freeinode++;
        struct dinode inode;

        bzero(&inode, sizeof(inode));
        inode.type = xshort(type);
        inode.nlink = xshort(1);
        inode.size = xint(0);
        winode(inodenum, &inode);

        return inodenum;
}

void balloc(int used)
{
        uchar buf[BSIZE];
        int i;

        printf("balloc: first %d blocks have been allocated\n", used);
        assert(used < BSIZE * 8);
        bzero(buf, BSIZE);
        for (i = 0; i < used; i++) {
                buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
        }
        printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
        wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void iappend(uint inodenum, void *xp, int n)
{
        char *inbuf = (char *)xp;
        uint blocknum, off, n1;
        struct dinode inode;
        char buf[BSIZE];
        uint indirect[NINDIRECT];
        uint sectnum;

        rinode(inodenum, &inode);
        off = xint(inode.size);
        // printf("append inodenum %d at off %d sz %d\n", inodenum, off, n);
        while (n > 0) {
                blocknum = off / BSIZE;
                assert(blocknum < MAXFILE);

                if (blocknum < NDIRECT) {  // direct blocks
                        if (xint(inode.addrs[blocknum]) == 0) {
                                inode.addrs[blocknum] = xint(freeblock++);
                        }
                        sectnum = xint(inode.addrs[blocknum]);

                } else {  // indirect blocks
                        if (xint(inode.addrs[NDIRECT]) == 0) {
                                inode.addrs[NDIRECT] = xint(freeblock++);
                        }
                        rsect(xint(inode.addrs[NDIRECT]), (char *)indirect);
                        if (indirect[blocknum - NDIRECT] == 0) {
                                indirect[blocknum - NDIRECT] =
                                        xint(freeblock++);
                                wsect(xint(inode.addrs[NDIRECT]),
                                      (char *)indirect);
                        }
                        sectnum = xint(indirect[blocknum - NDIRECT]);
                }

                n1 = min(n, (blocknum + 1) * BSIZE - off);
                rsect(sectnum, buf);
                bcopy(inbuf, buf + off - (blocknum * BSIZE), n1);
                wsect(sectnum, buf);
                n -= n1;
                off += n1;
                inbuf += n1;
        }
        inode.size = xint(off);
        winode(inodenum, &inode);
}
