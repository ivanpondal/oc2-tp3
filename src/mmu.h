/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "defines.h"
#include "game.h"

#define COMPARTIDA_BASE   0x400000
#define CODIGO_BASE       0x401000

#define COMPARTIDA_A_BASE 0x100000
#define COMPARTIDA_B_BASE 0x101000
#define CODIGO_A_BASE     0x10000
#define CODIGO_B_BASE     0x12000

#define CONTADOR_BASE     0x102000

#define MAPA_BASE_FISICA  0x500000
#define MAPA_BASE_VIRTUAL 0x800000

#define BASE_DIR_TABLAS		0x27000
#define BASE_DIR_PAGINAS_0	0x28000

uint contador;

void mmu_inicializar();


// devuelve la proxima pagina libre del area libre del kernel
uint mmu_proxima_pagina_fisica_libre();

// setea en cero todos los bytes
void mmu_inicializar_pagina(uint * pagina);

// copia los bytes
void mmu_copiar_pagina(uint src, uint dst);

// pide una pagina para usar de directorio. Luego inicializa las entradas que iran con identity mapping.
uint mmu_inicializar_dir_kernel();

// transforma coordenadas (x,y) en direcciones fisicas
uint mmu_xy2fisica(uint x, uint y);

// transforma coordenadas (x,y) en direcciones virtuales
uint mmu_xy2virtual(uint x, uint y);

// crea el directorio, las paginas, copia el codigo e inicializa el stack
uint mmu_inicializar_memoria_perro(perro_t *perro, int index_jugador, int index_tipo);

// debe remapear y copiar el codigo
void mmu_mover_perro(perro_t *perro, int viejo_x, int viejo_y);


void mmu_mapear_pagina		(uint virtual, uint cr3, uint fisica, uint attrs_pte);
void mmu_mapear_pagina_base	(uint virtual, uint cr3, uint fisica, uint attrs_pte, uint attrs_pde);
uint mmu_unmapear_pagina(uint virtual, uint cr3);


#endif	/* !__MMU_H__ */
