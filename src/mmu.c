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
void mmu_inicializar(){
	contador = CONTADOR_BASE;
} 

uint mmu_proxima_pagina_fisica_libre(){
	contador += 0x1000;
	return (contador - 0x1000);
}

void mmu_inicializar_pagina(uint * pagina){
	for(uint i = 0; i < 1024; i++){
		pagina[i] = 0;
	}
}

void mmu_copiar_pagina(uint src, uint dst){
	uint* ptr_src = (uint*)(src);
	uint* ptr_dst = (uint*)(dst);

	for(uint i = 0; i < 1024; i++){
		ptr_dst[i] = ptr_src[i];
	}
}

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

uint mmu_xy2fisica(uint x, uint y){
	return MAPA_BASE_FISICA + 4096*(80*y + x);
}

uint mmu_xy2virtual(uint x, uint y){
	return MAPA_BASE_VIRTUAL + 4096*(80*y + x);
}

uint mmu_inicializar_memoria_perro(perro_t *perro, int index_jugador, int index_tipo){
	uint base_directorio_tablas_perro = mmu_proxima_pagina_fisica_libre();
	uint* ptr_pd_perro = (uint*)(base_directorio_tablas_perro);

	uint base_directorio_paginas_perro = mmu_proxima_pagina_fisica_libre();
	uint* ptr_pt_perro = (uint*)(base_directorio_paginas_perro);

	mmu_inicializar_pagina(ptr_pd_perro);
	mmu_inicializar_pagina(ptr_pt_perro);

	// Inicializo PD
	*ptr_pd_perro = *ptr_pd_perro | 0x3; // Atributos
	*ptr_pd_perro = *ptr_pd_perro | base_directorio_paginas_perro;

	// Identity mapping
	uint direccion_pagina;
	for(uint i = 0; i < 1024; i++){
		direccion_pagina = i << 12;
		mmu_mapear_pagina(direccion_pagina, base_directorio_tablas_perro, direccion_pagina, 0x3);
	}

	// Código tarea
	uint base_dir_virtual_codigo = mmu_xy2virtual(perro->x, perro->y);
	uint base_dir_fisica_codigo = mmu_xy2fisica(perro->x, perro->y);
	mmu_mapear_pagina_base(base_dir_virtual_codigo, base_directorio_tablas_perro, base_dir_fisica_codigo, 0x5, 0x7);
	mmu_mapear_pagina_base(CODIGO_BASE, base_directorio_tablas_perro, base_dir_fisica_codigo, 0x7, 0x7);

	// Mapeamos memoria compartida
	uint direccion_compartida = (index_jugador == 0) ? COMPARTIDA_A_BASE : COMPARTIDA_B_BASE;
	mmu_mapear_pagina_base(COMPARTIDA_BASE, base_directorio_tablas_perro, direccion_compartida, 0x7, 0x7);

	// Mapeamos en kernel y copiamos código
	mmu_mapear_pagina(base_dir_virtual_codigo, BASE_DIR_TABLAS, base_dir_fisica_codigo, 0x3);
	uint direccion_codigo = (index_jugador == 0) ? 0x10000 + 0x1000*index_tipo : 0x12000 + 0x1000*index_tipo;
	mmu_copiar_pagina(direccion_codigo, base_dir_virtual_codigo);
	mmu_unmapear_pagina(base_dir_virtual_codigo, BASE_DIR_TABLAS);

	return base_directorio_tablas_perro;
}

void mmu_mapear_pagina(uint virtual, uint cr3, uint fisica, uint attrs_pte){
	mmu_mapear_pagina_base(virtual, cr3, fisica, attrs_pte, 0x3);
}

void mmu_mapear_pagina_base(uint virtual, uint cr3, uint fisica, uint attrs_pte, uint attrs_pde){
	uint base_directorio_tablas = 0xFFFFF000 & cr3;
	uint offset_directorio_tablas = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde = (uint*)(base_directorio_tablas + offset_directorio_tablas);
	uint pde = *ptr_pde;

	// Si no esta presente la tabla de páginas que buscamos
	if ((pde & 0x1) == 0){
		uint pt = mmu_proxima_pagina_fisica_libre();
		mmu_inicializar_pagina((uint*)pt);
		*ptr_pde = pt;	// Actualizamos el pde con la nueva dirección de la tabla de páginas
	}

	*ptr_pde = attrs_pde | *ptr_pde; // Seteamos el present y el write en el pde, para poder modificar el pte
	pde = *ptr_pde;
	uint base_directorio_paginas = 0xFFFFF000 & pde;

	uint offset_directorio_paginas = 4*((0x003FF000 & virtual) >> 12);
	uint* ptr_pte = (uint*)(base_directorio_paginas + offset_directorio_paginas);
	*ptr_pte = (0xFFFFF000 & fisica) | attrs_pte;
	tlbflush();	// Vaciamos la TLB para que se reflejen los cambios
}

uint mmu_unmapear_pagina(uint virtual, uint cr3){
	uint base_directorio_tablas = 0xFFFFF000 & cr3;
	uint offset_directorio_tablas = 4*((0xFFC00000 & virtual) >> 22);
	uint* ptr_pde = (uint*)(base_directorio_tablas + offset_directorio_tablas);
	uint pde = *ptr_pde;

	// Si está presente la tabla de páginas asociada a la dirección procedemos a
	// deshabilitar la página correspondiente
	if ((pde & 0x1) == 1){
		uint base_directorio_paginas = 0xFFFFF000 & pde;
		uint offset_directorio_paginas = 4*((0x003FF000 & virtual) >> 12);
		uint* ptr_pte = (uint*)(base_directorio_paginas + offset_directorio_paginas);

		*ptr_pte = *ptr_pte & 0xFFFFFFFE;
		tlbflush();
	}

	return 0;
}
