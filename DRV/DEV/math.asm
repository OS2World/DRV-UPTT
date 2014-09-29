;/************************************************************************\
;**                                                                      **
;**               OS/2(r) Physical Device Driver Libraries               **
;**                         for Watcom C/C++ 10                          **
;**                                                                      **
;**  COPYRIGHT:                                                          **
;**                                                                      **
;**    (C) Copyright Advanced Gravis Computer Technology Ltd 1994.       **
;**        All Rights Reserved.                                          **
;**                                                                      **
;**  DISCLAIMER OF WARRANTIES:                                           **
;**                                                                      **
;**    The following [enclosed] code is provided to you "AS IS",         **
;**    without warranty of any kind.  You have a royalty-free right to   **
;**    use, modify, reproduce and distribute the following code (and/or  **
;**    any modified version) provided that you agree that Advanced       **
;**    Gravis has no warranty obligations and shall not be liable for    **
;**    any damages arising out of your use of this code, even if they    **
;**    have been advised of the possibility of such damages.  This       **
;**    Copyright statement and Disclaimer of Warranties may not be       **
;**    removed.                                                          **
;**                                                                      **
;\************************************************************************/


;; Math.asm
;;
;; Run-time library replacement functions (long arithmetic, etc...)
;;
;; History:
;;
;; Sep 30, 94  David Bollo    Initial version



;; Allow 386 instructions (OS/2 2.0 or better)

              .386p


_TEXT         segment word public use16 'CODE'
              assume cs:_TEXT, ds:DGROUP, es:NOTHING, ss:NOTHING

              public __U4M
              public __I4M
              public __U4D
              public __I4D


;; Long multiply routine
;;
;; Arguments (from reverse engineering calls generated by the compiler):
;;    DX:AX * CX:BX
;; Returns
;;    DX:AX = product
;; Notes
;;    Trashes high words of 32-bit registers EAX and EDX
__U4M         proc near
__I4M:        shl      edx,10h            ;; Load dx:ax into eax
              mov      dx,ax
              mov      eax,edx
              mov      dx,cx              ;; Load cx:bx into edx
              shl      edx,10h
              mov      dx,bx
              mul      edx                ;; Multiply eax*edx into edx:eax
              mov      edx,eax            ;; Load eax into dx:ax
              shr      edx,10h
              ret
__U4M         endp


;; Long unsigned divide routine
;;
;; Arguments (from reverse engineering calls generated by the compiler):
;;    DX:AX / CX:BX
;; Returns
;;    DX:AX = quotient
;;    CX:BX = remainder
;; Notes
;;    Trashes high words of 32-bit registers EAX, ECX and EDX
__U4D         proc near
              shl      edx,10h            ;; Load dx:ax into eax
              mov      dx,ax
              mov      eax,edx
              cdq                         ;; Sign extend eax into edx
              shl      ecx,10h            ;; Load cx:bx into ecx
              mov      cx,bx
              div      ecx                ;; Divide eax/ecx into eax
              mov      ecx,edx            ;; Load edx into cx:bx
              shr      ecx,10h
              mov      bx,dx
              mov      edx,eax            ;; Load eax into dx:ax
              shr      edx,10h
              ret
__U4D         endp


;; Long signed divide routine
;;
;; Arguments (from reverse engineering calls generated by the compiler):
;;    DX:AX / CX:BX
;; Returns
;;    DX:AX = quotient
;;    CX:BX = remainder
;; Notes
;;    Trashes high words of 32-bit registers EAX, ECX and EDX
__I4D         proc near
              shl      edx,10h            ;; Load dx:ax into eax
              mov      dx,ax
              mov      eax,edx
              cdq                         ;; Sign extend eax into edx
              shl      ecx,10h            ;; Load cx:bx into ecx
              mov      cx,bx
              idiv     ecx                ;; Divide eax/ecx into eax
              mov      ecx,edx            ;; Load edx into cx:bx
              shr      ecx,10h
              mov      bx,dx
              mov      edx,eax            ;; Load eax into dx:ax
              shr      edx,10h
              ret
__I4D         endp


_TEXT         ends



              end
