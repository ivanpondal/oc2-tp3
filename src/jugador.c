
#include "game.h"
#include "mmu.h"
#include "screen.h"

void game_jugador_inicializar(jugador_t *j)
{
	static int index = 0;

	j->index = index++;

	if (j->index == JUGADOR_A) {
		j->x = POS_INIT_A_X;
		j->y = POS_INIT_A_Y;

	} else {
		j->x = POS_INIT_B_X;
		j->y = POS_INIT_B_Y;
	}

	j->puntos = 0;
	j->x_cucha = j->x;
	j->y_cucha = j->y;


	uint gdt_index = j->index == JUGADOR_A ? 0x7B : 0xBB;
	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		game_perro_inicializar(&j->perros[i], j, i, gdt_index + i*8);
	}

	screen_pintar_jugador(j);
}


// debe devolver el proximo perro del arreglo que no esté siendo usado actualmente
perro_t* game_jugador_dame_perro_libre(jugador_t *j)
{
	int i = 0;

	for (; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		if (j->perros[i].libre == TRUE)
			return &j->perros[i];
	}

	return NULL;
}


// debe encargarse de buscar un perro libre, configurarlo, y inicializar su mapeo de memoria, tss, y lugar en el sched
void game_jugador_lanzar_perro(jugador_t *j, uint tipo, int x, int y)
{
	if (game_perro_en_posicion(x,y) != NULL)
		return;

	perro_t *perro = game_jugador_dame_perro_libre(j);
	if (perro == NULL)
		return;

	game_perro_reciclar_y_lanzar(perro, tipo);
}

// recibe un par (x, y) y un jugador, al cual debe mover en esa dirección
uint game_jugador_moverse(jugador_t *j, int x, int y)
{
	int nuevo_x = j->x + x;
	int nuevo_y = j->y + y;

	if(game_es_posicion_valida(nuevo_x, nuevo_y)){
		screen_borrar_jugador(j);
		j->x = nuevo_x;
		j->y = nuevo_y;

		screen_pintar_jugador(j);
	}
    return 0;
}

// descarga 1 hueso en la cucha y actualiza el screen
void game_jugador_anotar_punto(jugador_t *j)
{
    ultimo_cambio = 0;

	j->puntos++;

    screen_pintar_puntajes();

	if(j->puntos==999)
		screen_stop_game_show_winner(j);
}


// guarda la orden en el jugador para que los perros puedan preguntarla luego (mediante un syscall)
void game_jugador_dar_orden(jugador_t *jugador, int orden){
	if(jugador->index == 0){
		ultima_orden[0][0] = jugador->x;
		ultima_orden[0][1] = jugador->y;
		ultima_orden[0][2] = orden;
	}else{
		ultima_orden[1][0] = jugador->x;
		ultima_orden[1][1] = jugador->y;
		ultima_orden[1][2] = orden;
	}
}



