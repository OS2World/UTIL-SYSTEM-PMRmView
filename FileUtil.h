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

/* return types for local file I/O functions */
#define CANTREAD (-1L)
#define CANTWRITE (-2L)
#define TOOLONG (-3L)
#define NOMEMORY (-4L)

ULONG   ReadFile (const char *fn, char **b);
ULONG   MakeWriteBuffer (ULONG cb, char **b);
ULONG   WriteFile (const char *fn, ULONG cb, char *b);
