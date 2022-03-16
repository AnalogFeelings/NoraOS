global HalLoadGDT

HalLoadGDT:
    lgdt [rdi]
	push 8
	lea rax, [rel .flush]
	push rax
	retfq
.flush:
	mov eax, 0x10
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	ret
