; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; Debugger
extern debug_info
extern debug_screen_on
extern debug_on

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_atender_tick
extern sched_tarea_actual
extern sched_remover_tarea
extern sched_debug_interrupcion


;; Game 
extern game_atender_teclado
extern game_syscall_manejar

;; Clock
extern esta_pantalla_debug_activada

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
	str ax	;COMENTAR ESTAS LINEAS SI SE QUIERE  
	push ax	;SACAR EL PANIC DE LAS EXCEPCIONES


    	mov dword [debug_info + 00], eax
    	mov dword [debug_info + 04], ebx
    	mov dword [debug_info + 08], ecx
    	mov dword [debug_info + 12], edx
    	mov dword [debug_info + 16], esi
    	mov dword [debug_info + 20], edi
    	mov dword [debug_info + 24], ebp
   	mov dword [debug_info + 28], esp
 
	mov eax, [esp+12] ; eip
	mov dword [debug_info + 32], eax ;eip
		
	mov ax, cs
	mov word [debug_info + 36], ax
	mov ax, ds
	mov word [debug_info + 38], ax
	mov ax, es
	mov word [debug_info + 40], ax
	mov ax, fs
	mov word [debug_info + 42], ax
	mov ax, gs
	mov word [debug_info + 44], ax
	mov ax, ss
	mov word [debug_info + 46], ax
    
    	xor eax, eax
    	pushf    ; obtenemos el registro
    	pop ax   ; eflags
    	mov dword [debug_info + 48], eax ; guardo flags
    
   	mov eax, cr0
  	mov dword [debug_info + 52], eax
  	mov eax, cr2
  	mov dword [debug_info + 56], eax
  	mov eax, cr3
  	mov dword [debug_info + 60], eax
  	mov eax, cr4
  	mov dword [debug_info + 64], eax
  	
  	mov eax, [esp]
  	mov dword [debug_info + 68], eax
  	mov eax, [esp+4]
  	mov dword [debug_info + 72], eax
  	mov eax, [esp+8]
  	mov dword [debug_info + 76], eax
  	mov eax, [esp+12]
  	mov dword [debug_info + 80], eax
    	mov eax, [esp+16]
  	mov dword [debug_info + 84], eax

	sti

	call sched_remover_tarea
	xor eax,eax
	mov eax,1
	cmp [debug_on],eax
	jne .nodebuguear
	call sched_debug_interrupcion
	.nodebuguear:
	;Salto a la tarea idle
	jmp 0x70:0




%endmacro

;;
;; Datos
;; -------------------------------------------------------------------------- ;;
; Scheduler

;;
;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0x00
ISR 0x01
ISR 0x02
ISR 0x03
ISR 0x04
ISR 0x05
ISR 0x06
ISR 0x07
ISR 0x08
ISR 0x09
ISR 0x0A
ISR 0x0B
ISR 0x0C
ISR 0x0D
ISR 0x0E
ISR 0x0F
ISR 0x10
ISR 0x11
ISR 0x12
ISR 0x13


;;
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr0x20
_isr0x20:
	pushad

	call fin_intr_pic1
	

	xor eax,eax
	mov eax,1
	cmp [debug_screen_on],eax
	je .fin
	xor eax, eax	
	
	call sched_atender_tick

	str cx
	cmp ax, cx
	je .fin
		mov [sched_tarea_selector], ax
		jmp far [sched_tarea_offset]
	.fin:
	popad
	iret

;;
;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr0x21
_isr0x21:
	pushad

	call fin_intr_pic1
	xor eax, eax
	in al, 0x60
	push eax
	call game_atender_teclado
	pop eax

	popad
	iret
;;
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;

global _isr0x46
_isr0x46:
	push eax
	pushad

	push ecx
	push eax
	call game_syscall_manejar
	add esp, 8
	mov [esp + 32], eax

	; Salto a la tarea idle
	jmp 0x70:0

	popad
	pop eax
	iret

