/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "sched.h"
#include "i386.h"
#include "screen.h"

sched_t scheduler;

void sched_inicializar(){
	for(int i = 0; i < MAX_CANT_TAREAS_VIVAS ; i++){
		scheduler.tasks[i].gdt_index = 0;
		scheduler.tasks[i].perro = 0;
	}

	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].gdt_index = 0x70;
	scheduler.tasks[MAX_CANT_TAREAS_VIVAS].perro = 0;
	scheduler.current = MAX_CANT_TAREAS_VIVAS;
}


int sched_buscar_indice_tarea(uint gdt_index) {
	int indice_tarea = -1;

	for(int i = 0; i < MAX_CANT_TAREAS_VIVAS; i++){
		if(scheduler.tasks[i].gdt_index == gdt_index){
			return i;
		}
	}
    return indice_tarea;
}


int sched_buscar_tarea_libre(uint jugador_index){
	for(int i = jugador_index; i < MAX_CANT_PERROS_VIVOS; i++){
		if(scheduler.tasks[2*i + jugador_index].gdt_index == NULL){
			return i;
		}
	}
    return -1;
}



perro_t* sched_tarea_actual(){
    return scheduler.tasks[scheduler.current].perro;
}

void sched_agregar_tarea(perro_t *perro){
	int indice_slot_libre = sched_buscar_tarea_libre(perro->jugador->index);

	if(indice_slot_libre != -1){
		scheduler.tasks[indice_slot_libre].gdt_index = perro->id;
		scheduler.tasks[indice_slot_libre].perro = perro;
	}
}

void sched_remover_tarea(unsigned int gdt_index){
	int indice_tarea_remover = sched_buscar_indice_tarea(gdt_index);

	if(indice_tarea_remover != -1){
		game_perro_termino(scheduler.tasks[indice_tarea_remover].perro);
		scheduler.tasks[indice_tarea_remover].gdt_index = NULL;
		scheduler.tasks[indice_tarea_remover].perro = NULL;
	}
}

uint sched_proxima_a_ejecutar(){
	uchar encontre_proximo = FALSE;
	uint i = scheduler.current;
	sched_task_t tarea_actual = scheduler.tasks[scheduler.current];
	sched_task_t tarea_siguiente;
	while(!encontre_proximo){
		i = (i + 1) % (MAX_CANT_TAREAS_VIVAS + 1);
		tarea_siguiente = scheduler.tasks[i];

		if(i == scheduler.current){
			if(tarea_actual.gdt_index == NULL){
				i = MAX_CANT_TAREAS_VIVAS;
			}
			encontre_proximo = TRUE;
		}
		else if(tarea_siguiente.gdt_index != NULL && tarea_siguiente.perro != NULL){
			if(tarea_actual.perro == NULL){
				encontre_proximo = TRUE;
			}
			else if(tarea_siguiente.perro->jugador->index != tarea_actual.perro->jugador->index){
				encontre_proximo = TRUE;
			}
		}
	}
    return i;
}


ushort sched_atender_tick(){
	game_atender_tick(game_perro_actual);
	scheduler.current = sched_proxima_a_ejecutar();
	game_perro_actual = scheduler.tasks[scheduler.current].perro;
    return scheduler.tasks[scheduler.current].gdt_index;
}


