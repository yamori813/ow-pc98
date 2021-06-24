;--- this is a 32bit DPMI application.

    .386
    .model small

    .dosseg    ;ensures that segments are ordered: code

    .code
    PUBLIC start
    PUBLIC printstring_
    EXTERN hello_:BYTE

start:
    call near ptr hello_
    mov ax, 4C00h   ;normal client exit
    int 21h

;--- print a string in protected-mode with simple
;--- DOS commands not using pointers.

printstring_:
    mov esi, eax
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
