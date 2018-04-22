/**
* @file checkblk.c
* @author Jas Eckard <eckard@clemson.edu>
*
* @section LICENSE
*
* GPL v3
*
* @section DESCRIPTION
*
* This file checks the blocks
*/

#include "debug.h"           // debug(), checkifdebugging()
#include "checkblk.h"

err checkblocks(FILE *xv6_fs_img)
{
        err rc = SUCCESS;

        debug("Begin reading blocks");

        debug("End reading blocks");

        return rc;
}
