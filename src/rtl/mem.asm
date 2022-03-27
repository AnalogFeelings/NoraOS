;  Taken from Limine
section .text

global memcpy
memcpy:
    mov rcx, rdx
    mov rax, rdi
    rep movsb
    ret

global memset
memset:
    push rdi
    mov rax, rsi
    mov rcx, rdx
    rep stosb
    pop rax
    ret
