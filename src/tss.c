/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"


tss tss_inicial;
tss tss_idle;

tss tss_jugadorA[MAX_CANT_PERROS_VIVOS];
tss tss_jugadorB[MAX_CANT_PERROS_VIVOS];

void tss_inicializar() {
	gdt[GDT_ID13_TSS_INICIAL_DESC].base_0_15 = (unsigned short)((uint)&tss_inicial & 0xFFFF);
	gdt[GDT_ID13_TSS_INICIAL_DESC].base_23_16 = (unsigned char)(((uint)&tss_inicial >> 16) & 0xFF);
	gdt[GDT_ID13_TSS_INICIAL_DESC].base_31_24 = (unsigned char)(((uint)&tss_inicial >> 24) & 0xFF);

	gdt[GDT_ID14_TSS_IDLE_DESC].base_0_15 = (unsigned short)((uint)&tss_idle & 0xFFFF);
	gdt[GDT_ID14_TSS_IDLE_DESC].base_23_16 = (unsigned char)(((uint)&tss_idle >> 16) & 0xFF);
	gdt[GDT_ID14_TSS_IDLE_DESC].base_31_24 = (unsigned char)(((uint)&tss_idle >> 24) & 0xFF);

	TSS_PERRO_ENTRY(15, A, 0);
	TSS_PERRO_ENTRY(16, A, 1);
	TSS_PERRO_ENTRY(17, A, 2);
	TSS_PERRO_ENTRY(18, A, 3);
	TSS_PERRO_ENTRY(19, A, 4);
	TSS_PERRO_ENTRY(20, A, 5);
	TSS_PERRO_ENTRY(21, A, 6);
	TSS_PERRO_ENTRY(22, A, 7);

	TSS_PERRO_ENTRY(23, B, 0);
	TSS_PERRO_ENTRY(24, B, 1);
	TSS_PERRO_ENTRY(25, B, 2);
	TSS_PERRO_ENTRY(26, B, 3);
	TSS_PERRO_ENTRY(27, B, 4);
	TSS_PERRO_ENTRY(28, B, 5);
	TSS_PERRO_ENTRY(29, B, 6);
	TSS_PERRO_ENTRY(30, B, 7);

}

void tss_inicializar_idle() {
	tss_idle.ptl = 0;
    tss_idle.unused0 = 0;
    tss_idle.esp0 = 0x27000;
    tss_idle.ss0 = 0x50;
    tss_idle.unused1 = 0;
    tss_idle.esp1 = 0;
    tss_idle.ss1 = 0;
    tss_idle.unused2 = 0;
    tss_idle.esp2 = 0;
    tss_idle.ss2 = 0;
    tss_idle.unused3 = 0;
    tss_idle.cr3 = 0x27000;
    tss_idle.eip = 0x16000;
    tss_idle.eflags = 0x00000202;
    tss_idle.eax = 0;
    tss_idle.ecx = 0;
    tss_idle.edx = 0;
    tss_idle.ebx = 0;
    tss_idle.esp = 0x27000;
    tss_idle.ebp = 0x27000;
    tss_idle.esi = 0;
    tss_idle.edi = 0;
    tss_idle.es = 0x50;
    tss_idle.unused4 = 0;
    tss_idle.cs = 0x40;
    tss_idle.unused5 = 0;
    tss_idle.ss = 0x50;
    tss_idle.unused6 = 0;
    tss_idle.ds = 0x50;
    tss_idle.unused7 = 0;
    tss_idle.fs = 0x50;
    tss_idle.unused8 = 0;
    tss_idle.gs = 0x50;
    tss_idle.unused9 = 0;
    tss_idle.ldt = 0;
    tss_idle.unused10 = 0;
    tss_idle.dtrap = 0;
    tss_idle.iomap = 0;
}

void tss_construir_tarea(perro_t *perro, int index_jugador, int index_tipo){
	uint cr3 = mmu_inicializar_memoria_perro(perro, index_jugador, index_jugador);
	uint direccion_codigo = (index_jugador == JUGADOR_A) ? CODIGO_A_BASE + PAGE_SIZE*index_tipo : CODIGO_B_BASE + PAGE_SIZE*index_tipo;
	uint ebp = direccion_codigo + 0xFFF;
	uint esp = ebp - 12;

	tss* ptr_tss = (index_jugador == JUGADOR_A) ? &tss_jugadorA[perro->id] : &tss_jugadorB[perro->id];

	ptr_tss->ptl = 0;
	ptr_tss->unused0 = 0;
	ptr_tss->esp0 = 0x27000;
	ptr_tss->ss0 = 0x50;
	ptr_tss->unused1 = 0;
	ptr_tss->esp1 = 0;
	ptr_tss->ss1 = 0;
	ptr_tss->unused2 = 0;
	ptr_tss->esp2 = 0;
	ptr_tss->ss2 = 0;
	ptr_tss->unused3 = 0;
	ptr_tss->cr3 = cr3;
	ptr_tss->eip = CODIGO_BASE;
	ptr_tss->eflags = 0x00000202;
	ptr_tss->eax = 0;
	ptr_tss->ecx = 0;
	ptr_tss->edx = 0;
	ptr_tss->ebx = 0;
	ptr_tss->esp = esp;
	ptr_tss->ebp = ebp;
	ptr_tss->esi = 0;
	ptr_tss->edi = 0;
	ptr_tss->es = 0x58;
	ptr_tss->unused4 = 0;
	ptr_tss->cs = 0x48;
	ptr_tss->unused5 = 0;
	ptr_tss->ss = 0x58;
	ptr_tss->unused6 = 0;
	ptr_tss->ds = 0x58;
	ptr_tss->unused7 = 0;
	ptr_tss->fs = 0x58;
	ptr_tss->unused8 = 0;
	ptr_tss->gs = 0x58;
	ptr_tss->unused9 = 0;
	ptr_tss->ldt = 0;
	ptr_tss->unused10 = 0;
	ptr_tss->dtrap = 0;
	ptr_tss->iomap = 0;
}

