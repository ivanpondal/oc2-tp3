;----------
; BLACKDOG
;----------
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
	cmp byte [0x401FF4], 1
	je .matar
	jmp $	; congelar tarea
	.matar:
	xor eax, eax
	div eax
	.copiar_a_compartida:
	mov byte [0x401FF4], 0	; flag para matar o congelar
	; guardo posición inicial cucha y perro
	mov si, [esp + 8]
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
	jmp .blackdog
	.blackdog:
		mov eax, 4
		int 0x46
		mov ebx, eax
		and ebx, 0xFFFF0000
		shr ebx, 16	; ebx = número de orden

		cmp ebx, 1
		jne .switch_dos_tres
		; 1 - mover perro hacia jugador
		xor ebx, ebx
		mov bx, ax
		shl ebx, 8
		and ax, 0x00FF
		mov bx, ax
		; ebx = jugador_y:jugador_x
		.mover_x:
			cmp bx, di	; jugador_x ? perro_x
			je .mover_y
			jb .mover_izquierda
			.mover_derecha:
			; jugador_x > perro_x
			mov eax, 1
			mov ecx, 0xA
			int 0x46
			inc di	; perro_x++
			jmp .mover_x
			.mover_izquierda:
			; jugador_x < perro_x
			mov eax, 1
			mov ecx, 0xD
			int 0x46
			dec di	; perro_x--
			jmp .mover_x
			.mover_y:
				; jugador_x == perro_x
				cmp ebx, edi	; jugador_y:jugador_x ? perro_y:perro_x
				je .blackdog
				jb .mover_arriba
				.mover_abajo:
				; jugador_y:jugador_x > perro_y:perro_x
				mov eax, 1
				mov ecx, 0x7
				int 0x46
				add edi, 0x10000
				jmp .mover_y
				.mover_arriba:
				; jugador_y:jugador_x < perro_y:perro_x
				mov eax, 1
				mov ecx, 0x4
				int 0x46
				sub edi, 0x10000
				jmp .mover_y
		.switch_dos_tres:
		cmp ebx, 2
		jne .switch_tres
		; 2 - atacar
		.atacar:
			mov eax, 1
			mov ecx, 0x4
			int 0x46
			mov eax, 1
			mov ecx, 0x7
			int 0x46
		.switch_tres:
		cmp ebx, 3
		jne .blackdog
		; 3 - matar
		mov byte [0x401FF4], 1
		jmp .blackdog
	.heybulldog:
		mov eax, 3
		int 0x46
		cmp eax, 0x4
		je .ir_arriba
		cmp eax, 0x7
		je .ir_abajo
		cmp eax, 0xA
		je .ir_derecha
		cmp eax, 0xD
		je .ir_izquierda
		jmp .ir_cucha
		.ir_arriba:
			sub edi, 0x10000
			jmp .moverse
		.ir_abajo:
			add edi, 0x10000
			jmp .moverse
		.ir_derecha:
			inc di
			jmp .moverse
		.ir_izquierda:
			dec di
			jmp .moverse
		.moverse:
			mov ecx, eax
			mov eax, 1
			int 0x46
			jmp .heybulldog
		.ir_cucha:
			mov eax, 2
			int 0x46
			cmp eax, 0
			jne .ir_cucha
			.volver_x:
				cmp si, di	; cucha_x ? perro_x
				je .volver_y
				jb .volver_izquierda
				.volver_derecha:
				; cucha_x > perro_x
				mov eax, 1
				mov ecx, 0xA
				int 0x46
				inc di	; perro_x++
				jmp .volver_x
				.volver_izquierda:
				; cucha_x < perro_x
				mov eax, 1
				mov ecx, 0xD
				int 0x46
				dec di	; perro_x--
				jmp .volver_x
				.volver_y:
					; cucha_x == perro_x
					cmp esi, edi	; cucha_y:cucha_x ? perro_y:perro_x
					je .fin
					jb .volver_arriba
					.volver_abajo:
					; cucha_y:cucha_x > perro_y:perro_x
					mov eax, 1
					mov ecx, 0x7
					int 0x46
					add edi, 0x10000
					jmp .volver_y
					.volver_arriba:
					; cucha_y:cucha_x < perro_y:perro_x
					mov eax, 1
					mov ecx, 0x4
					int 0x46
					sub edi, 0x10000
					jmp .volver_y
			.fin:
			jmp $
