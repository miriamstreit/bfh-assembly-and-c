SECTION .data           ; Section containing initialised data
    Base64Table: db "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    Base64String: db "0000"
    Base64StrLen: equ $-Base64String

SECTION .bss            ; Section containing uninitialized data
    InBufLen: EQU 3     ; initialize a buffer of length 3 for the input string
    InBuf:    resb InBufLen

SECTION .text           ; Section containing code

global _start           ; Linker needs this to find the entry point!

_start:
read:
    mov rax, "0000"         ; the buffer needs to be reset every time
    mov [Base64String], rax

    ; read bytes from stdin
    mov rax, 0		    ; Code for sys_read
    mov rdi, 0		    ; file descriptor: stdin
    mov rsi, InBuf	    ; destination buffer
    mov rdx, InBufLen	; max length of input
    syscall

    ; validate input
    cmp rax, 0		    ; check if there is an input
    je exit		        ; if the input is empty, exit

    ; prepare loop
    mov r10, rax	; rax contains amount of bytes read

process:
    xor r11, r11    ; clear r11. it will be used as index

    mov r8, [InBuf]     ; copy content of InBuf to r8
    call swap_bytes
    and r8, 0xfc0000	; bitmask for the first 6 bits
    shr r8, 18			; shift 18 to right (to have the 6bits at the end)

    call read_and_store_base64_symbol

    inc r11			; increase r11 (going up to 4)

    mov r8, [InBuf]     ; copy content of InBuf to r8
    call swap_bytes
    and r8, 0x03f000		; bitmask for second 6 bits
    shr r8, 12			; shift 12 to the right
    
    call read_and_store_base64_symbol
	
    cmp r10, 1              ; if the input was 1 byte long, it's the moment to exit
    je exit_with_one_byte   ; jumps to exit label which appends two "=" signs

    inc r11 ; increase counter

    mov r8, [InBuf]     ; copy content of InBuf to r8
    call swap_bytes
    and r8, 0x000fc0    ; bitmask for third 6 bits
    shr r8, 6           ; shift 6 to the right

    call read_and_store_base64_symbol

    cmp r10, 2              ; if the input was 1 byte long, it's the moment to exit
    je exit_with_two_bytes  ; jumps to exit label which appends one "=" sign

    inc r11 ; increase counter

    mov r8, [InBuf]     ; copy content of InBuf to r8
    call swap_bytes
    and r8, 0x00003f        ; bitmask for four 6 bits (no more shifting needed)

    call read_and_store_base64_symbol

	call print

    jmp read

read_and_store_base64_symbol:
    xor al, al                      ; clear al
    mov al, byte[Base64Table+r8]    ; read base64 symbol from table
    mov byte[Base64String+r11], al  ; store symbol in output string
    ret                             ; return to continue after its call

swap_bytes:
    bswap r8    ; little endian, bits are stored the wrong way around
    shr r8, 40  ; shift the preceeding zeroes that were added after the value
    ret         ; return to continue after its call

exit_with_one_byte:
    mov bl, 0x3D                    ; 0x3D is an equal sign
    mov byte [Base64String+2], bl   ; appends one equal sign
    jmp exit_with_two_bytes         ; appends the second equal sign and jumps to exit

exit_with_two_bytes:
    mov bl, 0x3D                    ; 0x3D is an equal sign
    mov byte [Base64String+3], bl   ; appends one equal sign
    call print
    jmp exit                        ; jump to exit

print:
    ; print four base64 symbols
    mov rax, 1
    mov rdi, 1
    mov rsi, Base64String
    mov rdx, Base64StrLen
    syscall
    ret

exit:
    mov rax, 60         ; Code for exit
    mov rdi, 0          ; Return a code of zero
    syscall             ; Make kernel call
