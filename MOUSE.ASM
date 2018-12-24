        title   MOUSE.ASM Mouse Event Handler for C Programs
        page    55,132

; MOUSE.ASM  Mouse Event Handler for C Programs
; Copyright (C) 1989 Ziff Davis Communications
; PC Magazine * Ray Duncan
;
; Assemble with:  MASM /Mx MOUSE;

args    equ     4               ; stack offset of arguments,
                                ; C small model

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

mflag   dw      ?               ; offset of mouse event flag
xcoord  dw      ?               ; offset of X variable
ycoord  dw      ?               ; offset of Y variable
buttons dw      ?               ; offset of buttons variable

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

; The procedure 'mregister' is called by the C program with
; the addresses of four variables: an event flag, which is
; set to TRUE by the event handler whenever the mouse changes
; state, and three variables to receive the mouses's X and Y
; coordinates and the state of the mouse buttons.
;
; The C prototype for the function is:
;
;   void mregister(int *flag, int *x, int *y, int *buttons)

        public  _mregister
_mregister proc near
                                ; parameters passed by C program
foffs   equ     [bp+args]       ; offset of event flag variable
xoffs   equ     foffs+2         ; offset of X variable
yoffs   equ     xoffs+2         ; offset of Y variable
boffs   equ     yoffs+2         ; offset of buttons variable

        push    bp              ; set up stack frame
        mov     bp,sp
        push    si              ; protect register variables
        push    di

        mov     ax,boffs        ; save offsets of the   
        mov     buttons,ax      ; C program's variables
        mov     ax,yoffs
        mov     ycoord,ax
        mov     ax,xoffs
        mov     xcoord,ax
        mov     ax,foffs
        mov     mflag,ax

                                ; register mouse event handler
        push    cs
        pop     es              ; ES:DX = handler address
        mov     dx,offset _TEXT:mhandler
        mov     cx,1fh          ; CX = event mask               
        mov     ax,0ch          ; function 0CH = register
        int     33h             ; transfer to mouse driver

        pop     di              ; restore register variables
        pop     si              ; and discard stack frame
        pop     bp
        ret                     ; back to caller

_mregister endp


; The mouse event handler is entered by a far call from the
; mouse driver whenever a mouse event occurs corresponding
; to the event mask passed in the original registration call.
; At entry to the handler, the registers are set up as follows:
;
;       AX      = mouse event flags:    
;                       bit   significance
;                       0     mouse movement
;                       1     left button pressed
;                       2     left button released
;                       3     right button pressed
;                       4     right button released
;                       5-15  reserved
;       BX      = button state          
;                       bit   significance
;                       0     left button is down
;                       1     right button is down
;                       2-15  reserved
;       CX      = X coordinate
;       DX      = Y coordinate
;       SI      = raw vertical mickey count
;       DI      = raw horizontal mickey count
;       DS      = mouse driver data segment

mhandler proc   far
        
        push    ds              ; save mouse driver's data segment
        mov     di,DGROUP       ; make our data segment addressable     
        mov     ds,di
        mov     di,xcoord       ; get offset of X coordinate variable
        mov     [di],cx         ; store X coordinate
        mov     di,ycoord       ; get offset of Y coordinate variable
        mov     [di],dx         ; store Y coordinate
        mov     di,buttons      ; get offset of buttons variable
        mov     [di],bx         ; store button state
        mov     di,mflag        ; get offset of event flag variable
        mov     word ptr [di],1 ; set the variable to TRUE
        pop     ds              ; restore mouse driver's data segment
        ret                     ; and return to driver

mhandler endp


_TEXT   ends

        end

