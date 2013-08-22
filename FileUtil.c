/*
 * PMRmView - PM frontend for RMVIEW.
 *
 * ver 1.0, 25 Feb 2000
 *
 * Public domain by:
 *   Jari Laaksonen
 *   Arkkitehdinkatu 30 A 2
 *   FIN-33720 Tampere
 *   FINLAND
 *
 *   email: jari.j.laaksonen@nokia.com
 */

#define INCL_WIN
#define INCL_GPI
#define INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fileutil.h"

/* open a file, read file into dynamically allocated buffer
   b is a pointer to the beginning of the buffer
   On success, return file size (in bytes)
   On failure, return:
                 CANTREAD if unable to open file
                 TOOLONG if file too big (> 64K)
                 NOMEMORY if unable to allocate memory      */
ULONG   ReadFile (const char *fn, char **b)
{
    HFILE   file;            /* handle */
    ULONG   ac;              /* action code (from DosOpen) */
    ULONG   rc;              /* return code (from API functions) */
    ULONG   rd;              /* bytes actually read */
    ULONG   cbSize;          /* file size in bytes */
    ULONG   dummy;           /* used when seeking back to beginning of file */

    /* try to open existing file --  access */
    rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0001, 0x20C0, 0L);
    if (rc != 0 || ac != 0x0001) /* failed */
        return CANTREAD;

    DosChgFilePtr (file, 0L, 2, &cbSize); /* determine file size */
    // if (cbSize > 65535L)
    // return TOOLONG;

    DosChgFilePtr (file, 0L, 0, &dummy); /* reset pointer to beginning */

    rc = DosAllocMem ((PPVOID) b, cbSize, PAG_READ | PAG_WRITE | PAG_COMMIT);
    if (0 == rc)
    {
        /* read entire file into buffer */
        DosRead (file, *b, cbSize, &rd);
        DosClose (file);
        return (LONG) cbSize;
    }
    return NOMEMORY;
}

/* create a dynamically allocated buffer to use for writing to file
   on success, return 0;  on failure return NOMEMORY                */
ULONG   MakeWriteBuffer (ULONG cb, char **b)
{
    ULONG   rc;              /* return code (from API functions) */

    rc = DosAllocMem ((PPVOID) b, cb, PAG_READ | PAG_WRITE | PAG_COMMIT);
    if (rc != 0)
        return NOMEMORY;

    return 0;
}

/* write buffer to file (cb is number of bytes in buffer)
   on success return 0;  on failure return CANTWRITE      */
ULONG   WriteFile (const char *fn, ULONG cb, char *b)
{
    HFILE   file;            /* handle */
    ULONG   ac;              /* action code (from DosOpen) */
    ULONG   rc;              /* return code (from API functions) */
    ULONG   wr;              /* bytes actually written */

    /* create file, overwriting any existing file */
    rc = DosOpen (fn, &file, &ac, 0L, 0, 0x0012, 0x20C1, 0L);
    if (rc == 0 && (ac == 0x0002 || ac == 0x0003))
    {                        /* ok */
        /* write entire buffer to file */
        rc = DosWrite (file, b, cb, &wr);
        if (rc != 0 || wr != cb)
            return CANTWRITE;

        /* close file */
        rc = DosClose (file);
        if (rc != 0)
            return CANTWRITE;

        return 0;
    }
    else
        return CANTWRITE;
}
