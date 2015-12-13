ORG 0x00401000

BITS 32

; eax: reservado para sumas, multiplicaciones, etc
; ebx
; ecx: reservado para ciclos
; edx
; esi: cucha_y:cucha_x
; edi: perro_y:perro_x
; ebp
; esp

main:
	jmp .copiar_a_compartida
	times 1024 nop
	jmp $
	.copiar_a_compartida:
	; guardo posición inicial cucha y perro
	mov si, [esp]
	shl esi, 16
	mov si, [esp + 4]	; esi = cucha_y:cucha_x
	mov edi, esi		; edi = perro_y:perro_x

	xor ecx, ecx
	mov cx, 1024
	.ciclo_copia:
		mov eax, [4*ecx + 0x400ffc]
		mov dword [4*ecx + 0x3ffffc], eax
		loop .ciclo_copia
	jmp 0x400700
	times 1024 nop
	.dogloop:
		mov word eax, 4
		int 0x46
		mov ebx, eax
		and ebx, 0xFFFF0000
		shr ebx, 16	; ebx = número de orden

		cmp ebx, 1
		jne .dogloop
		; 1 - mover perro hacia jugador
		xor ebx, ebx
		mov bx, ax
		shl ebx, 8
		and ax, 0x00FF
		mov bx, ax
		; ebx = jugador_y:jugador_x
		.move_x:
			cmp bx, di	; jugador_x ? perro_x
			je .move_y
			jb .move_left
			.move_right:
			; jugador_x > perro_x
			mov word eax, 1
			mov word ecx, 0xA
			int 0x46
			inc di	; perro_x++
			jmp .move_x
			.move_left:
			; jugador_x < perro_x
			mov word eax, 1
			mov word ecx, 0xD
			int 0x46
			dec di	; perro_x--
			jmp .move_x
			.move_y:
				; jugador_x == perro_x
				cmp ebx, edi	; jugador_y:jugador_x ? perro_y:perro_x
				je .dogloop
				jb .move_up
				.move_down
				; jugador_y:jugador_x > perro_y:perro_x
				mov word eax, 1
				mov word ecx, 0x7
				int 0x46
				add edi, 0x10000
				jmp .move_y
				.move_up:
				; jugador_y:jugador_x < perro_y:perro_x
				mov word eax, 1
				mov word ecx, 0x4
				int 0x46
				sub edi, 0x10000
				jmp .move_y
		jmp .dogloop
