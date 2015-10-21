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
	uint base_directorio = 0xFFFFF000 & cr3;
	uint offset_directorio_virtual = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde_virtual = (uint*)(base_directorio + offset_directorio_virtual);
	uint pde_virtual = *ptr_pde_virtual;

	uint base_directorio_tabla = 0xFFFFF000 & pde_virtual;
	uint offset_directorio_tabla = 4*((0x003FF000 & virtual) >> 12);
	uint* ptr_pte_virtual = (uint*)(base_directorio_tabla + offset_directorio_tabla);

	*ptr_pte_virtual = (0x00000FFF & virtual) | (0xFFFFF000 & fisica) | attrs;
}

uint mmu_unmapear_pagina(uint virtual, uint cr3){
	uint base_directorio = 0xFFFFF000 & cr3;
	uint offset_directorio_virtual = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde_virtual = (uint*)(base_directorio + offset_directorio_virtual);
	uint pde_virtual = *ptr_pde_virtual;

	uint base_directorio_tabla = 0xFFFFF000 & pde_virtual;
	uint offset_directorio_tabla = 4*((0x003FF000 & virtual) >> 12);
	uint* ptr_pte_virtual = (uint*)(base_directorio_tabla + offset_directorio_tabla);

	*ptr_pte_virtual = *ptr_pte_virtual & 0xFFFFFFFE;

	return 0;
}
