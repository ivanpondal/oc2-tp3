/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
/* Atributos paginas */
/* -------------------------------------------------------------------------- */


/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */

uint mmu_inicializar_dir_kernel(){

	// Inicializo PDE
	uint* ptr_pde = (uint*)(BASE_DIR_TABLAS);
	*ptr_pde = 0;
	*ptr_pde = *ptr_pde | 0x3; // Atributos
	*ptr_pde = *ptr_pde | BASE_DIR_PAGINAS_0;

	uint direccion_pagina;
	for(uint i = 0; i < 1024; i++){
		direccion_pagina = i << 12;
		mmu_mapear_pagina(direccion_pagina, BASE_DIR_TABLAS, direccion_pagina, 0x3);
	}

	return 0;
}

void mmu_mapear_pagina(uint virtual, uint cr3, uint fisica, uint attrs){
	uint base_directorio_tablas = 0xFFFFF000 & cr3;
	uint offset_directorio_tablas = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde = (uint*)(base_directorio_tablas + offset_directorio_tablas);
	uint pde = *ptr_pde;

	uint base_directorio_paginas = 0xFFFFF000 & pde;
	uint offset_directorio_paginas = 4*((0x003FF000 & virtual) >> 12);
	uint* ptr_pte = (uint*)(base_directorio_paginas + offset_directorio_paginas);

	*ptr_pte = (0xFFFFF000 & fisica) | attrs;
}

uint mmu_unmapear_pagina(uint virtual, uint cr3){
	uint base_directorio_tablas = 0xFFFFF000 & cr3;
	uint offset_directorio_tablas = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde = (uint*)(base_directorio_tablas + offset_directorio_tablas);
	uint pde = *ptr_pde;

	uint base_directorio_paginas = 0xFFFFF000 & pde;
	uint offset_directorio_paginas = 4*((0x003FF000 & virtual) >> 12);
	uint* ptr_pte = (uint*)(base_directorio_paginas + offset_directorio_paginas);

	*ptr_pte = *ptr_pte & 0xFFFFFFFE;

	return 0;
}
