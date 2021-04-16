SECTION .data           ; Section containing initialised data
	HelloMsg: db "Hello world!",10 ; define Hello World String
	HelloLen: equ $-HelloMsg       ; compute length of String

SECTION .bss            ; Section containing uninitialized data

SECTION .text           ; Section containing code

global _start           ; Linker needs this to find the entry point!

_start:
    mov rax, 1 		; Code for sys_write call
    mov rdi, 1		; standard output
    mov rsi, HelloMsg	; pass message
    mov rdx, HelloLen   ; pass length of message
    syscall		; make syscall using defined calls with parameters

    mov rax, 60         ; Code for exit
    mov rdi, 0          ; Return a code of zero
    syscall             ; Make kernel call
