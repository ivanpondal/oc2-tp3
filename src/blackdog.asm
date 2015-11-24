mov ORG 0x00401000

BITS 32

main:
	jmp .copiar_a_compartida
	times 1024 nop
	jmp $
	.copiar_a_compartida:
	mov esi, [esp]
	mov edi, [esp + 4]
	xor ecx, ecx
	mov cx, 1024
	.ciclo_copia:
		mov eax, [4*ecx + 0x400ffc]
		mov dword [4*ecx + 0x3ffffc], eax
		loop .ciclo_copia
	jmp 0x400700
	times 1024 nop
	.idle:
		jmp .idle
