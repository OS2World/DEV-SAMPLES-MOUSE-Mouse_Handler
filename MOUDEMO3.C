 /*
    MOUDEMO3.C --- Demonstrates use of MOUSE.ASM Event Handler
    Copyright (C) 1989 Ziff Davis Communications
    PC Magazine * Ray Duncan

    Compile:    MASM /Mx MOUSE;
                CL MOUDEMO3.C MOUSE

    Usage:      MOUDEMO3  (press both mouse buttons to exit)
*/

#include <stdio.h>
#include <dos.h>

union REGS regs;

void cls(void);                     /* local function prototypes */
void gotoxy(int, int);
int  mopen(int *);
void mclose(int);
void mhide(int);
void munhide(int);

                                    /* prototype for MOUSE.ASM */
void extern mregister(int *, int *, int *, int *);

main()
{
    int x,y,buttons;                /* mouse state variables */
    int mhandle;                    /* mouse handle from mopen */
    int mflag = 0;                  /* mouse event flag */

    if(! mopen(&mhandle))           /* exit if no mouse */
    {   
        printf("\nMouse not available\n");
        exit(1);
    }

                                    /* register mouse event handler */  
    mregister(&mflag, &x, &y, &buttons);

    cls();                          /* clear the screen */  
    gotoxy(45,0);                   /* and show help info */
    puts("Press Both Mouse Buttons To Exit");

    munhide(mhandle);               /* make mouse cursor visible */
   
    while(1)                        /* loop practically forever */
    {
        if(mflag)                   /* if mouse event detected */
        {
            gotoxy(0,0);            /* display mouse status */
            printf("X = %3d  Y = %3d", x, y);
            mflag = 0;              /* reset mouse event flag */    

            if(buttons == 3) break; /* exit if both buttons down */
        }
    } 

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
    Status is true if mouse available, false if not.  Handle is
    dummy value in MS-DOS version.
*/
int mopen(int *handle)
{
    regs.x.ax = 0;                  /* function 0 = reset driver */
    int86(0x33, &regs, &regs);
    *handle = 0;                    /* set dummy mouse handle */
    return(regs.x.ax);              /* and return status */
}

/*
    Make mouse pointer visible.  Mouse handle is dummy in MS-DOS version.
*/
void munhide(handle)
{
    regs.x.ax = 1;                  /* function 1 = display pointer */
    int86(0x33, &regs, &regs);
}

/*
    Hide mouse pointer.  Mouse handle is dummy in MS-DOS version.
*/
void mhide(handle)
{
    regs.x.ax = 2;                  /* function 2 = hide pointer */
    int86(0x33, &regs, &regs);
}

/*
    Release mouse driver.  Mouse handle is dummy in MS-DOS version.
    This incidentally disables mouse event handler calls from
    mouse driver.
*/
void mclose(handle)
{
    regs.x.ax = 0;                  /* function 0 = reset driver */
    int86(0x33, &regs, &regs);
}

