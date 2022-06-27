;  Taken from Limine
section .text

global memcpy
memcpy:
    mov rcx, rdx
    mov rax, rdi
    rep movsb
    ret
    
global memcpy16
memcpy16:
    mov rcx, rdx
    mov rax, rdi
    rep movsw
    ret
    
global memcpy32
memcpy32:
    mov rcx, rdx
    mov rax, rdi
    rep movsd
    ret
    
global memcpy64
memcpy64:
    mov rcx, rdx
    mov rax, rdi
    rep movsq
    ret

global memset
memset:
    push rdi
    mov rax, rsi
    mov rcx, rdx
    rep stosb
    pop rax
    ret
    
global memset16
memset16:
    push rdi
    mov rax, rsi
    mov rcx, rdx
    rep stosw
    pop rax
    ret
    
global memset32
memset32:
    push rdi
    mov rax, rsi
    mov rcx, rdx
    rep stosd
    pop rax
    ret
    
global memset64
memset64:
    push rdi
    mov rax, rsi
    mov rcx, rdx
    rep stosq
    pop rax
    ret
