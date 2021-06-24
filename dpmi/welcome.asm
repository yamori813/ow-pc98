;--- this is a 32bit DPMI application.
;--- assemble+link: JWasm -mz Dos3.asm

    .386
    .model small

    .dosseg    ;ensures that segments are ordered: code, data

    .stack 1024

LF  equ 10
CR  equ 13

    .data

szWelcome db "welcome in protected-mode",CR,LF,0

    .code
    PUBLIC start

start:
    mov esi, offset szWelcome
    call printstring
    mov ax, 4C00h   ;normal client exit
    int 21h

;--- print a string in protected-mode with simple
;--- DOS commands not using pointers.

printstring:
    lodsb
    and al,al
    jz stringdone
    mov dl,al
    mov ah,2
    int 21h
    jmp printstring
stringdone:
    ret

    end
