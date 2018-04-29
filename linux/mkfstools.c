/**
* @file mkfstools.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* Tools from xv6's mkfs.c, broken into their own functions.
*/

#include <stdio.h>      // fread(3), fwrite(3)
#include <unistd.h>     // lseek(2)
#include <stdlib.h>     // exit(3)
#include <string.h>     // bzero(3)
#include <assert.h>     // assert(3)

#define stat xv6_stat           // avoid clash with host struct stat
#include "../param.h"   // FSSIZE
#include "debug.h"      // debug(), checkifdebugging()
#include "error.h"      // BAD_FS_FILE_*
#include "mkfstools.h"

uint freeinode = 1;
uint freeblock;

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

err rinode(FILE *xv6_fs_img, uint inum, struct dinode *ip, uint inodestart)
{
        char buf[BSIZE] = "";
        uint bn = 0;
        struct dinode *dip = NULL;
        err rc = SUCCESS;

        // from IBLOCK
        bn = inum / IPB + inodestart;
        rc = rsect(xv6_fs_img, bn, buf);
        dip = ((struct dinode *)buf) + (inum % IPB);
        *ip = *dip;

        return rc;
}

err winode(FILE *xv6_fs_img, uint inum, struct dinode *ip, uint inodestart)
{
        char buf[BSIZE] = "";
        uint bn = 0;
        struct dinode *dip = NULL;

        // from IBLOCK
        bn = inum / IPB + inodestart;
        rsect(xv6_fs_img, bn, buf);
        dip = ((struct dinode *)buf) + (inum % IPB);
        *dip = *ip;

        return wsect(xv6_fs_img, bn, buf);
}

err rsect(FILE *xv6_fs_img, uint sec, void *buf)
{
        if (0 != fseek(xv6_fs_img, sec * BSIZE, SEEK_SET)) {
                debug("error with fseek(3)");
                return BAD_FS_FILE_SEEK;
        }

        if (BSIZE != fread(buf, 1, BSIZE, xv6_fs_img)) {
                debug("error with fread(3)");
                return BAD_FS_FILE_READ;
        }

        return SUCCESS;
}

err wsect(FILE *xv6_fs_img, uint sec, void *buf)
{
        if (0 != fseek(xv6_fs_img, sec * BSIZE, SEEK_SET)) {
                debug("error with fseek(3)");
                return BAD_FS_FILE_SEEK;
        }

        if (BSIZE != fwrite(buf, 1, BSIZE, xv6_fs_img)) {
                debug("error with fwrite(3)");
                return BAD_FS_FILE_WRITE;
        }

        return SUCCESS;
}

uint ialloc(FILE *xv6_fs_img, ushort type, uint inodestart)
{
        uint inum = freeinode++;
        struct dinode din;

        bzero(&din, sizeof(din));
        din.type = xshort(type);
        din.nlink = xshort(1);
        din.size = xint(0);
        winode(xv6_fs_img, inum, &din, inodestart);

        return inum;
}

err balloc(FILE *xv6_fs_img, int used, uint bmapstart)
{
        uchar buf[BSIZE];
        int i;

        printf("balloc: first %d blocks have been allocated\n", used);
        assert(used < BSIZE * 8);
        bzero(buf, BSIZE);
        for (i = 0; i < used; i++) {
                buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
        }
        printf("balloc: write bitmap block at sector %d\n", bmapstart);

        return wsect(xv6_fs_img, bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

err iappend(FILE *xv6_fs_img, uint inum, void *xp, int n, uint inodestart)
{
        char *p = (char *)xp;
        uint fbn, off, n1;
        struct dinode din;
        char buf[BSIZE];
        uint indirect[NINDIRECT];
        uint x;

        rinode(xv6_fs_img, inum, &din, inodestart);
        off = xint(din.size);
        // printf("append inum %d at off %d sz %d\n", inum, off, n);
        while (n > 0) {
                fbn = off / BSIZE;
                assert(fbn < MAXFILE);
                if (fbn < NDIRECT) {
                        if (xint(din.addrs[fbn]) == 0) {
                                din.addrs[fbn] = xint(freeblock++);
                        }
                        x = xint(din.addrs[fbn]);
                } else {
                        if (xint(din.addrs[NDIRECT]) == 0) {
                                din.addrs[NDIRECT] = xint(freeblock++);
                        }
                        rsect(xv6_fs_img, xint(din.addrs[NDIRECT]), (char *)indirect);
                        if (indirect[fbn - NDIRECT] == 0) {
                                indirect[fbn - NDIRECT] =
                                        xint(freeblock++);
                                wsect(xv6_fs_img, xint(din.addrs[NDIRECT]),
                                      (char *)indirect);
                        }
                        x = xint(indirect[fbn - NDIRECT]);
                }
                n1 = min(n, (fbn + 1) * BSIZE - off);
                rsect(xv6_fs_img, x, buf);
                bcopy(p, buf + off - (fbn * BSIZE), n1);
                wsect(xv6_fs_img, x, buf);
                n -= n1;
                off += n1;
                p += n1;
        }
        din.size = xint(off);

        return winode(xv6_fs_img, inum, &din, inodestart);
}
