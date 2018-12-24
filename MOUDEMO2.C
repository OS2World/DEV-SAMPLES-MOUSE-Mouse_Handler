 /*
    MOUDEMO2.C --- Simple OS/2 Demo of Mouse Subsystem
    Copyright (C) 1989 Ziff Davis Communications
    PC Magazine * Ray Duncan

    Compile:    CL MOUDEMO2.C

    Usage:      MOUDEMO2  (press both mouse buttons to exit)
*/

#include <stdio.h>

void cls(void);                     /* local function prototypes */
void gotoxy(int, int);
int  mopen(int *);
void mclose(int);
void mread(int, int *, int *, int *);
void mhide(int);
void munhide(int);

#define API unsigned extern far pascal

API MouClose(int);                  /* OS/2 function prototypes */
API MouDrawPtr(int);
API MouOpen(void far *, int far *);
API MouReadEventQue(void far *, int far *, int);
API MouRemovePtr(void far *, int);

struct _MouEvent {  unsigned Flags;
                    long Timestamp;
                    int Row;
                    int Col;
                 }  ;

main()
{
    int x,y,buttons;                /* mouse state variables */
    int mhandle;                    /* mouse handle from mopen */

    if(! mopen(&mhandle))           /* exit if no mouse */
    {   
        printf("\nMouse not available\n");
        exit(1);
    }

    cls();                          /* clear the screen */  
    gotoxy(45,0);                   /* and show help info */
    puts("Press Both Mouse Buttons To Exit");

    munhide(mhandle);               /* make mouse cursor visible */
   
    do {                            /* read mouse position & buttons */
        mread(mhandle, &x, &y, &buttons);
        gotoxy(0,0);                /* display mouse status */
        printf("X = %3d  Y = %3d", x, y);
    }   while(buttons != 3);        /* exit if both buttons down */

    mhide(mhandle);                 /* hide mouse cursor */
    mclose(mhandle);                /* release mouse driver */
    cls();                          /* clear screen and exit */
}

/*
    Clear the screen and position cursor at (x,y) = (0,0)
*/
void cls(void)
{
    printf("\x01b[2J");
}

/*
    Position cursor to (x,y); ANSI driver uses 1-based coordinates.
*/
void gotoxy(int x, int y)
{
    printf("\x01b[%d;%dH",y+1,x+1);
}

/*
    Initialize mouse driver, return mouse status and handle.
    Status is true if mouse available, false if not.  
*/
int mopen(int *handle)
{
    return(! MouOpen(0L, handle));  /* open mouse, save handle, */
}                                   /* and return status */                 

/*
    Make mouse pointer visible. 
*/
void munhide(handle)
{
    MouDrawPtr(handle);
}

/*
    Hide mouse pointer.  For the purposes of this demo, we 
    assume an 80-by-25 text mode.
*/
void mhide(handle)
{
    static int HideArea[4] = { 0, 0, 24, 79 } ;
    
    MouRemovePtr(HideArea, handle);
}

/*
    Read mouse position and button status.  
*/
void mread(int handle, int *x, int *y, int *buttons)
{
    int WaitOption = 1 ;            /* wait for a mouse event */
    struct _MouEvent MouEvent;      /* receives mouse information */

    MouReadEventQue(&MouEvent, &WaitOption, handle);

    *x = MouEvent.Col;              /* return mouse coordinates */
    *y = MouEvent.Row;              /* and button status */
    *buttons = ((MouEvent.Flags&4)>>2) | ((MouEvent.Flags&0x10)>>3) & 3;
}

/*
    Release mouse driver.  
*/
void mclose(handle)
{
    MouClose(handle);
}

