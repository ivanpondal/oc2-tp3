
#include "game.h"
#include "mmu.h"
#include "screen.h"
#include "tss.h"


// realiza inicialización básica de un perro. El perro aun no está vivo ni por lanzarse. Setea jugador, indice, etc
void game_perro_inicializar(perro_t *perro, jugador_t *j, uint index, uint id)
{
	perro->id   = id;
    perro->index = index;
    perro->jugador = j;
	perro->libre = TRUE;

//	~~~ completar si es necesario ~~~

}

// toma un perro ya existente libre y lo recicla seteando x e y a la cucha.
// luego inicializa su mapeo de memoria, tss, lo agrega al sched y pinta la pantalla
void game_perro_reciclar_y_lanzar(perro_t *perro, uint tipo)
{
	jugador_t *j = perro->jugador;

	perro->x = j->x_cucha;
	perro->y = j->y_cucha;
	perro->huesos = 0;
	perro->tipo = tipo;
	perro->libre = FALSE;

	// ahora debo llamar a rutinas que inicialicen un nuevo mapa de
	// memoria para el nuevo perro, que carguen su tss correspondiente,
	// lo scheduleen y finalmente lo pinten en pantalla

	tss_construir_tarea(perro, j->index, tipo);
	screen_pintar_perro(perro);
}

// el perro descargó sus huesos o realizó una acción no válida y caputó, hay que sacarlo del sistema.
void game_perro_termino(perro_t *perro)
{
//	~~~ completar ~~~
}

// transforma código de dirección en valores x e y 
uint game_dir2xy(/* in */ direccion dir, /* out */ int *x, /* out */ int *y)
{
	switch (dir)
	{
		case IZQ: *x = -1; *y =  0; break;
		case DER: *x =  1; *y =  0; break;
		case ABA: *x =  0; *y =  1; break;
		case ARR: *x =  0; *y = -1; break;
    	default: return -1;
	}

	return 0;
}

// recibe una direccion y un perro, al cual debe mover en esa dirección
// *** viene del syscall mover ***
uint game_perro_mover(perro_t *perro, direccion dir)
{
	int x_dest = perro->x;
	int y_dest = perro->y;

	switch(dir){
		case ARR:
			y_dest--;
			break;
		case ABA:
			y_dest++;
			break;
		case DER:
			x_dest++;
			break;
		case IZQ:
			x_dest--;
			break;
		case AQUI:
			break;
	}

	if(dir != AQUI && game_es_posicion_valida(x_dest, y_dest)){
		perro_t* perro_en_posicion = game_perro_en_posicion(x_dest, y_dest);
		if(perro_en_posicion != NULL && perro_en_posicion->jugador->index != perro->jugador->index){
			// muevo al perro
			perro->x = x_dest;
			perro->y = y_dest;
			uint direccion_nueva_virtual = mmu_xy2virtual(x_dest, y_dest);
			uint direccion_nueva_fisica = mmu_xy2fisica(x_dest, y_dest);
			uint cr3 = rcr3(); 
			mmu_mapear_pagina_base(direccion_nueva_virtual, cr3, direccion_nueva_fisica, 0x5, 0x7);
			mmu_copiar_pagina(CODIGO_BASE, direccion_nueva_virtual);
			mmu_mapear_pagina_base(CODIGO_BASE, cr3, direccion_nueva_fisica, 0x7, 0x7);
		}
	}
	return 0;
}

// recibe un perro, el cual debe cavar en su posición
// *** viene del syscall cavar ***
uint game_perro_cavar(perro_t *perro)
{
	if(perro->huesos < 10 && game_huesos_en_posicion(perro->x, perro->y) > 0){
		perro->huesos++;	
		for (int i = 0; i < ESCONDITES_CANTIDAD; i++){
			if (escondites[i][0] == perro->x && escondites[i][1] == perro->y){
				escondites[i][2]--;
				return 0;
			}
		}
	}
	return 0;
}

// recibe un perro, devueve la dirección del hueso más cercano
// *** viene del syscall olfatear ***
uint game_perro_olfatear(perro_t *perro)
{
	int x_actual_diff = 1000, y_actual_diff = 1000;


	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
		if (escondites[i][2] == 0) continue;

		int diff_x = escondites[i][0] - (int)perro->x;
		int diff_y = escondites[i][1] - (int)perro->y;

		if (x_actual_diff * x_actual_diff + y_actual_diff * y_actual_diff > diff_x * diff_x + diff_y * diff_y)
		{
			x_actual_diff = diff_x;
			y_actual_diff = diff_y;
		}
   	}

	if (x_actual_diff == 0 && y_actual_diff == 0)
		return AQUI;

	if (x_actual_diff * x_actual_diff > y_actual_diff * y_actual_diff)
	{
		return x_actual_diff > 0 ? DER : IZQ;
	}
	else 
	{
		return y_actual_diff > 0 ? ABA : ARR;
	}

    return 0;
}

uint game_perro_recibir_orden(perro_t *perro){
	return ultima_orden[perro->jugador->index];
}

// chequea si el perro está en la cucha y suma punto al jugador o lo manda a dormir
void game_perro_ver_si_en_cucha(perro_t *perro)
{
	if (perro->x != perro->jugador->x_cucha || perro->y != perro->jugador->y_cucha)
		return;

	if (perro->huesos == 0)
		return;

	game_jugador_anotar_punto(perro->jugador);
	perro->huesos--;
	if (perro->huesos == 0)
		game_perro_termino(perro);
}

