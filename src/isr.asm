; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "imprimir.mac"

BITS 32

sched_tarea_offset:     dd 0x00
sched_tarea_selector:   dw 0x00

;; PIC
extern fin_intr_pic1

;; Sched
extern sched_atender_tick
extern sched_tarea_actual
extern sched_remover_tarea

;; Game 
extern game_atender_teclado
extern game_syscall_manejar
;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
	str ax
	push ax
	call sched_remover_tarea
	; Salto a la tarea idle
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
	pushad

	push ecx
	push eax
	call game_syscall_manejar
	add esp, 8

	; Salto a la tarea idle
	jmp 0x70:0

	popad
	iret
