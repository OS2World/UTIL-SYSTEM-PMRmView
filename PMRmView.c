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
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#define INCL_DOSQUEUES
#define INCL_DOSNMPIPES
#define INCL_DOSERRORS
#include <os2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>

#include "resource.h"
#include "fileutil.h"

/* custom messages, etc. */
#define ID_MLE 13            /* my lucky number! */

#define BUFSIZE 256
#define HF_STDOUT 1

/* local function prototypes */
MRESULT EXPENTRY ClientWndProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY AboutDlgProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY FindDlgProc (HWND, ULONG, MPARAM, MPARAM);

VOID    SetPtrArrow (VOID);
VOID    SetPtrWait (VOID);

/* global variables */
HAB     hab;                 /* anchor block handle */
HWND    hwndMLE;
char    szClientClass[] = "PMRmView";
char    szFind[60];          /* target string for search */
char    szTitle[] = "PMRmView";
char    szFontNameSize[] = "8.Courier";

int     main (void /* int argc, char *argv[] */ )
{
    static ULONG flFrameFlags = FCF_TITLEBAR | FCF_SYSMENU |
        FCF_SIZEBORDER | FCF_MINMAX | FCF_SHELLPOSITION | FCF_TASKLIST | FCF_MENU | FCF_ACCELTABLE | FCF_ICON;
    HMQ     hmq;
    HWND    hwndFrame, hwndClient;
    QMSG    qmsg;

#if defined (__DEBUG_ALLOC__)

    // __DEBUG_ALLOC__ is defined for the debug version of the C Set/2 Memory
    // Management routines. Since the debug version writes to stderr, we
    // send all stderr output to a debuginfo file. Look in MAKEFILE to see how
    // to enable the debug version of those routines.

    freopen ("debug.log", "w", stderr);
#endif

    hab = WinInitialize (0);
    hmq = WinCreateMsgQueue (hab, 0);

    WinRegisterClass (hab,   /* Anchor block handle            */
        szClientClass,       /* Name of class being registered */
        ClientWndProc,       /* Window procedure for class     */
        CS_SIZEREDRAW,       /* Class style                    */
        0);                  /* Extra bytes to reserve         */

    hwndFrame = WinCreateStdWindow (HWND_DESKTOP, /* Parent window handle            */
        WS_VISIBLE,          /* Style of frame window           */
        &flFrameFlags,       /* Pointer to control data         */
        szClientClass,       /* Client window class name        */
        szTitle,             /* Title bar text                  */
        0L,                  /* Style of client window          */
        0,                   /* Module handle for resources     */
        ID_PMRMVIEW,         /* ID of resources                 */
        &hwndClient);        /* Pointer to client window handle */

    while (WinGetMsg (hab, &qmsg, 0, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    WinDestroyWindow (hwndFrame);
    WinDestroyMsgQueue (hmq);
    WinTerminate (hab);

#ifdef __DEBUG_ALLOC__
    _dump_allocated (-1);
#endif

    return 0;
}

void    DoCreate (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    hwndMLE = WinCreateWindow (hwnd, WC_MLE, "", WS_VISIBLE | MLS_HSCROLL | MLS_VSCROLL | MLS_BORDER, 0, 0, 0, 0, /* will set
                                                                                                                     size &
                                                                                                                     position
                                                                                                                     later */
        hwnd, HWND_TOP, ID_MLE, NULL, NULL);

    WinSetPresParam (hwndMLE, PP_FONTNAMESIZE, strlen (szFontNameSize) + 1, szFontNameSize);

    /* set up some MLE parameters */
    WinSendMsg (hwndMLE, MLM_FORMAT, MPFROMSHORT (MLFIE_CFTEXT), NULL);
    WinSendMsg (hwndMLE, MLM_RESETUNDO, NULL, NULL);

    WinSetFocus (HWND_DESKTOP, hwndMLE);

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    DoInitEditMenu (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND    hwndMenu;
    LONG    selmin, selmax;  /* text selected for deletion */
    SHORT   undoable;        /* TRUE if last operation can be undone */

    hwndMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT), FID_MENU);

    /* enable Cut, Copy, or Delete only if text selected */
    selmin = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, (MPARAM) MLFQS_MINSEL, NULL));
    selmax = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYSEL, (MPARAM) MLFQS_MAXSEL, NULL));
    WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT (IDM_CUT, TRUE), MPFROM2SHORT (MIA_DISABLED,
            (selmin == selmax) ? MIA_DISABLED : 0));
    WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT (IDM_COPY, TRUE), MPFROM2SHORT (MIA_DISABLED,
            (selmin == selmax) ? MIA_DISABLED : 0));
    // WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT (IDM_DELETE, TRUE), MPFROM2SHORT (MIA_DISABLED, (selmin == selmax) ?
    // MIA_DISABLED : 0));

    /* enable Undo only if operation may be undone */
    undoable = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_QUERYUNDO, NULL, NULL));
    WinSendMsg (hwndMenu, MM_SETITEMATTR, MPFROM2SHORT (IDM_UNDO, TRUE), MPFROM2SHORT (MIA_DISABLED,
            (undoable) ? 0 : MIA_DISABLED));

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    DoInitCommandsMenu (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND    hwndMenu;

    hwndMenu = WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT), FID_MENU);

    /* 
       nothing at the moment. */

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    DoSaveAs (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    FILEDLG fdg;

    memset (&fdg, 0, sizeof (FILEDLG));
    fdg.cbSize = sizeof (FILEDLG);
    fdg.fl = FDS_SAVEAS_DIALOG | FDS_CENTER;

    if (!WinFileDlg (HWND_DESKTOP, hwnd, &fdg))
        return;

    if (fdg.lReturn == DID_OK)
    {
        ULONG   bufferlen;   /* number of characters read into buffer */
        PCHAR   buffer;      /* buffer for file I/O */
        LONG    selmin, selmax; /* text selected for deletion */
        char    szFileName[260]; /* current filename */

        strcpy (szFileName, fdg.szFullFile);

        /* determine amount of text in MLE */
        bufferlen = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYFORMATTEXTLENGTH, NULL, MPFROMLONG (-1L)));

        /* allocate space for buffer */
        if (NOMEMORY == MakeWriteBuffer (bufferlen, &buffer))
        {
            WinMessageBox (HWND_DESKTOP, hwnd, "Cannot allocate memory.", "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
            WinSendMsg (hwnd, WM_QUIT, NULL, NULL);
        }
        SetPtrWait ();

        /* transfer text from MLE to buffer */
        WinSendMsg (hwndMLE, MLM_SETIMPORTEXPORT, MPFROMP (buffer), MPFROMLONG (bufferlen));
        selmin = 0L;
        selmax = bufferlen;
        WinSendMsg (hwndMLE, MLM_EXPORT, MPFROMP (&selmin), MPFROMP (&selmax));

        /* write to file */
        if (CANTWRITE == WriteFile (szFileName, bufferlen, buffer))
        {
            WinMessageBox (HWND_DESKTOP, hwnd, "Unable to write to file.", "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
        }

        /* deallocate buffer */
        DosFreeMem (buffer);
        WinSendMsg (hwndMLE, MLM_RESETUNDO, NULL, NULL);

        SetPtrArrow ();
    }

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    DoFind (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    if (DID_OK == WinDlgBox (HWND_DESKTOP, hwnd, FindDlgProc, 0, IDD_FIND, NULL))
    {
        MLE_SEARCHDATA SearchData; /* used is MLM_SEARCH message */
        ULONG   res;         /* response from dialog or message box */

        SetPtrWait ();

        SearchData.cb = sizeof (MLE_SEARCHDATA);
        SearchData.pchFind = szFind;
        SearchData.cchFind = strlen (szFind);
        SearchData.iptStart = (-1L);
        SearchData.iptStop = (-1L);

        res = SHORT1FROMMR (WinSendMsg (hwndMLE, MLM_SEARCH, MPFROMLONG (MLFSEARCH_SELECTMATCH), MPFROMP (&SearchData)));

        SetPtrArrow ();

        if (!res)
        {
            WinMessageBox (HWND_DESKTOP, hwnd, "Search string not found.", "Find", 0, MB_OK | MB_ICONASTERISK | MB_MOVEABLE);
        }
    }

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    DoSelectall (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    ULONG   size;

    // WinSendMsg (hwndMLE, MLM_SETSEL, MPFROMLONG(0), MPFROMLONG(WinSendMsg(hwndMLE, MLM_QUERYTEXTLENGTH, 0, 0)));
    // WinPostMsg (hwndMLE, WM_CHAR, MPFROM2SHORT ((KC_CTRL |            KC_VIRTUALKEY), 0), MPFROM2SHORT (0, VK_HOME));
    // WinPostMsg (hwndMLE, WM_CHAR, MPFROM2SHORT ((KC_CTRL | KC_SHIFT | KC_VIRTUALKEY), 0), MPFROM2SHORT (0, VK_END));

    WinEnableWindowUpdate (hwnd, FALSE);

    /* Get length of the output in the MLE */
    size = (ULONG) WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL);

    /* Select all of the output in the MLE */
    WinSendMsg (hwndMLE, MLM_SETSEL, NULL, MPFROMLONG (size));
    WinInvalidateRect (hwnd, NULL, TRUE);

    /* update the window */
    WinEnableWindowUpdate (hwnd, TRUE);

    /* Make MLE so that it can't be undone */
    WinSendMsg (hwndMLE, MLM_RESETUNDO, NULL, NULL);

    msg = msg;
    mp1 = mp1;
    mp2 = mp2;
}

void    fixnl (char *buffer)
{
    char   *read;
    char   *write;

    read = buffer;
    write = buffer;
    while (*read)
    {
        if (*read == '\r' && *(read+1) == '\n')
            read++;
        *write = *read;
        read++;
        write++;
    }
    *write = '\0';
}

void    DoRun (HWND hwnd, const char *params)
{
    APIRET  rc;
    HPIPE   pipeRead = -1;
    HPIPE   pipeWrite = -1;
    HFILE   fileOutSave = -1;
    HFILE   fileOutNew = HF_STDOUT;
    char    buffer[BUFSIZE];
    ULONG   bytesRead = 0;
    STARTDATA StartData;
    ULONG   ulSessID;
    PID     pid;
    char    program[] = "RMVIEW.EXE";

    WinSendMsg (hwnd, WM_COMMAND, (MPARAM) IDM_DELETE, NULL);

    rc = DosDupHandle (HF_STDOUT, &fileOutSave);
    rc = DosCreatePipe (&pipeRead, &pipeWrite, BUFSIZE);
    rc = DosDupHandle (pipeWrite, &fileOutNew);

    /* start session */
    memset (&StartData, 0, sizeof (STARTDATA));

    // Specify the various session start parameters
    StartData.Length = sizeof (STARTDATA);
    StartData.Related = SSF_RELATED_CHILD;
    StartData.FgBg = SSF_FGBG_FORE;
    StartData.TraceOpt = SSF_TRACEOPT_NONE;
    StartData.InheritOpt = SSF_INHERTOPT_PARENT;
    StartData.PgmControl = SSF_CONTROL_VISIBLE;
    StartData.PgmControl |= SSF_CONTROL_MINIMIZE;
    StartData.PgmName = (char *) program;
    StartData.PgmInputs = params;
    StartData.SessionType = SSF_TYPE_WINDOWABLEVIO;

    rc = DosStartSession (&StartData, &ulSessID, &pid);

    rc = DosClose (pipeWrite);
    rc = DosDupHandle (fileOutSave, &fileOutNew);
    rc = DosClose (fileOutSave);

    do
    {
        rc = DosRead (pipeRead, buffer, sizeof (buffer), &bytesRead);
        buffer[bytesRead] = '\0';
        if (bytesRead > 0)
        {
            fixnl (buffer);
            WinSendMsg (hwndMLE, MLM_INSERT, buffer, NULL);
        }
    }
    while (bytesRead);

    DosClose (pipeRead);
}

/* main window procedure for application */
MRESULT EXPENTRY ClientWndProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_CREATE:
        DoCreate (hwnd, msg, mp1, mp2);
        return 0;

    case WM_INITMENU:
        if (SHORT1FROMMP (mp1) == IDM_FILE)
        {
            ULONG   len = LONGFROMMR (WinSendMsg (hwndMLE, MLM_QUERYFORMATTEXTLENGTH, NULL, MPFROMLONG (-1L)));

            WinSendMsg (WinWindowFromID (WinQueryWindow (hwnd, QW_PARENT), FID_MENU),
                MM_SETITEMATTR, MPFROM2SHORT (IDM_SAVEAS, TRUE), MPFROM2SHORT (MIA_DISABLED, (len) ? 0 : MIA_DISABLED));
        }
        else if (SHORT1FROMMP (mp1) == IDM_EDIT)
        {
            DoInitEditMenu (hwnd, msg, mp1, mp2);
        }
        else if (SHORT1FROMMP (mp1) == IDM_COMMANDS)
        {
            DoInitCommandsMenu (hwnd, msg, mp1, mp2);
        }
        return 0;

    case WM_CONTROL:
        switch (SHORT2FROMMP (mp1))
        {
        case MLN_OVERFLOW:
            if (SHORT1FROMMP (mp1) == ID_MLE)
            {
                WinMessageBox (HWND_DESKTOP, hwnd, "File too large.", "Error", 0, MB_OK | MB_ICONHAND | MB_MOVEABLE);
            }
            return 0;

        default:
            break;
        }
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP (mp1))
        {
        case IDM_SAVEAS:
            DoSaveAs (hwnd, msg, mp1, mp2);
            return 0;

        case IDM_EXIT:
            WinSendMsg (hwnd, WM_CLOSE, NULL, NULL);
            return 0;

        case IDM_UNDO:
            WinSendMsg (hwndMLE, MLM_UNDO, NULL, NULL);
            return 0;

        case IDM_CUT:
            WinSendMsg (hwndMLE, MLM_CUT, NULL, NULL);
            return 0;

        case IDM_COPY:
            WinSendMsg (hwndMLE, MLM_COPY, NULL, NULL);
            return 0;

        case IDM_DELETE:
            WinSendMsg (hwndMLE, MLM_DISABLEREFRESH, NULL, NULL);
            WinSendMsg (hwndMLE, MLM_SETSEL, MPFROMSHORT (NULL), (MPARAM) WinSendMsg (hwndMLE, MLM_QUERYTEXTLENGTH, NULL, NULL));
            WinSendMsg (hwndMLE, MLM_CLEAR, NULL, NULL);
            WinSendMsg (hwndMLE, MLM_SETCHANGED, MPFROMSHORT (FALSE), NULL);
            WinSendMsg (hwndMLE, MLM_ENABLEREFRESH, NULL, NULL);
            return 0;

        case IDM_SELECTALL:
            DoSelectall (hwnd, msg, mp1, mp2);
            return 0;

        case IDM_FIND:
            DoFind (hwnd, msg, mp1, mp2);
            return 0;

        case IDM_P:
            DoRun (hwnd, "/P");
            return 0;
        case IDM_P1:
            DoRun (hwnd, "/P1");
            return 0;
        case IDM_D:
            DoRun (hwnd, "/D");
            return 0;
        case IDM_DA:
            DoRun (hwnd, "/DA");
            return 0;
        case IDM_D1:
            DoRun (hwnd, "/D1");
            return 0;
        case IDM_DC:
            DoRun (hwnd, "/DC");
            return 0;
        case IDM_DP:
            DoRun (hwnd, "/DP");
            return 0;
        case IDM_L:
            DoRun (hwnd, "/L");
            return 0;
        case IDM_IRQ:
            DoRun (hwnd, "/IRQ");
            return 0;
        case IDM_IO:
            DoRun (hwnd, "/IO");
            return 0;
        case IDM_IOA:
            DoRun (hwnd, "/IOA");
            return 0;
        case IDM_DMA:
            DoRun (hwnd, "/DMA");
            return 0;
        case IDM_MEM:
            DoRun (hwnd, "/MEM");
            return 0;
        case IDM_HW:
            DoRun (hwnd, "/HW");
            return 0;

        case IDM_ABOUT:
            WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc, 0, IDD_ABOUT, NULL);
            return 0;

        default:
            break;
        }
        break;

    case WM_SIZE:
        {
            USHORT  cyClient, cxClient; /* size of edit window */

            cxClient = SHORT1FROMMP (mp2);
            cyClient = SHORT2FROMMP (mp2);

            WinSetWindowPos (hwndMLE, HWND_TOP, 0, 0, cxClient, cyClient, SWP_MOVE | SWP_SIZE);
            WinSetFocus (HWND_DESKTOP, hwndMLE);
        }
        return 0;

    case WM_DESTROY:
        WinDestroyWindow (hwndMLE);
        return 0;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

VOID    SetPtrArrow (VOID)
{
    if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
        WinShowPointer (HWND_DESKTOP, FALSE);

    WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, 0));
}

VOID    SetPtrWait (VOID)
{
    WinSetPointer (HWND_DESKTOP, WinQuerySysPointer (HWND_DESKTOP, SPTR_WAIT, 0));

    if (!WinQuerySysValue (HWND_DESKTOP, SV_MOUSEPRESENT))
        WinShowPointer (HWND_DESKTOP, TRUE);
}

/* window procedure for about box */
MRESULT EXPENTRY AboutDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (SHORT1FROMMP (mp1))
        {
        case DID_OK:
        case DID_CANCEL:
            WinDismissDlg (hwnd, TRUE);
            return 0;
        default:
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* window procedure for find (text search) dialog box */
MRESULT EXPENTRY FindDlgProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        WinSendDlgItemMsg (hwnd, DID_FINDTEXT, EM_SETTEXTLIMIT, MPFROM2SHORT (50, 0), NULL);
        WinSetDlgItemText (hwnd, DID_FINDTEXT, szFind);
        WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_FINDTEXT));
        return (MRESULT) TRUE;

    case WM_COMMAND:
        switch (SHORT1FROMMP (mp1))
        {
        case DID_OK:
            if (WinQueryDlgItemText (hwnd, DID_FINDTEXT, 60, szFind))
                WinDismissDlg (hwnd, DID_OK);
            else
                WinDismissDlg (hwnd, DID_CANCEL);
            return 0;

        case DID_CANCEL:
            WinDismissDlg (hwnd, DID_CANCEL);
            return 0;

        default:
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
