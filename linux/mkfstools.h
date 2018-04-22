/**
* @file mkfstools.h
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* Header file for mkfstools.c
*/

#ifndef mkfstools_h
#define mkfstools_h

#include <stdio.h>      // FILE
#include "../types.h"   // ushort, unint
#include "../fs.h"      // dinode
#include "error.h"      // err

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define NINODES 200

ushort xshort(ushort x);
uint xint(uint x);
err balloc(FILE *xv6_fs_img, int used, uint bmapstart);
err rinode(FILE *xv6_fs_img, uint inum, struct dinode *ip, uint inodestart);
err winode(FILE *xv6_fs_img, uint inum, struct dinode *ip, uint inodestart);
err rsect(FILE *xv6_fs_img, uint sect, void *buf);
err wsect(FILE *xv6_fs_img, uint sect, void *buf);
uint ialloc(FILE *xv6_fs_img, ushort type, uint inodestart);
err iappend(FILE *xv6_fs_img, uint inum, void *p, int n, uint inodestart);

#endif  // mkfstools_h
