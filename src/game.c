/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "sched.h"

#include <stdarg.h>


int debug_on = FALSE;

int escondites[ESCONDITES_CANTIDAD][3] = { // TRIPLAS DE LA FORMA (X, Y, HUESOS)
                                        {76,  25, 50}, {12, 15, 50}, {9, 10, 100}, {47, 21, 100} ,
                                        {34,  11, 50}, {75, 38, 50}, {40, 21, 100}, {72, 17, 100}
                                    };

jugador_t jugadorA;
jugador_t jugadorB;

perro_t *game_perro_actual = NULL;
int ultimo_cambio = 0;
int ultima_orden[2][3] = {{0, 0, 0}, {0, 0, 0}};

void ASSERT_OR_ERROR(uint value, char* error_msg)
{
	if (!value) {
		print(error_msg, 5, 5, C_BG_LIGHT_GREY | C_FG_BLACK);
		breakpoint();
	}
}

void* error()
{
	__asm__ ("int3");
	return 0;
}

uint game_xy2lineal (uint x, uint y) {
	return (y * MAPA_ANCHO + x);
}

uint game_es_posicion_valida(int x, int y) {
	return (x >= 0 && y >= 0 && x < MAPA_ANCHO && y < MAPA_ALTO);
}

void game_inicializar()
{
	game_jugador_inicializar(&jugadorA);
	game_jugador_inicializar(&jugadorB);
	debug_screen_on = 0;
	debug_on = 0;

    screen_pintar_puntajes();
}


// devuelve la cantidad de huesos que hay en la posición pasada como parametro
uint game_huesos_en_posicion(uint x, uint y)
{
	int i;
	for (i = 0; i < ESCONDITES_CANTIDAD; i++)
	{
		if (escondites[i][0] == x && escondites[i][1] == y)
			return escondites[i][2];
	}
	return 0;
}




// devuelve algun perro que esté en la posicion pasada (hay max 2, uno por jugador)
perro_t* game_perro_en_posicion(uint x, uint y)
{
	int i;
	for (i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
	{
		if (!jugadorA.perros[i].libre && jugadorA.perros[i].x == x && jugadorA.perros[i].y == y)
			return &jugadorA.perros[i];
		if (!jugadorB.perros[i].libre && jugadorB.perros[i].x == x && jugadorB.perros[i].y == y)
			return &jugadorB.perros[i];
	}
	return NULL;
}



// termina si se agotaron los huesos o si hace tiempo que no hay ningun cambio
void game_terminar_si_es_hora(){
	int suma = 0;
	int i;	
	for (i = 0; i < ESCONDITES_CANTIDAD; i++){
		suma += escondites[i][2];
	}

	jugador_t* ganador = NULL;
	if(jugadorA.puntos > jugadorB.puntos) ganador = &jugadorA;
	else if(jugadorA.puntos < jugadorB.puntos) ganador = &jugadorB;

	if(suma == 0 || ultimo_cambio == MAX_SIN_CAMBIOS) screen_stop_game_show_winner(ganador);
}

void game_jugador_debug(){
		if(debug_screen_on == TRUE){
			debug_screen_on = FALSE;
			screen_restore_backup();
		}
		else{
			if(debug_on == FALSE){
				debug_on = TRUE;
				print("MODO DEBUG ACTIVADO", 55, 0, 0x0f0f);
			}
			else{
				debug_on = FALSE;
				screen_pintar_rect(0, 0x00, 0, 53, 1, 23);
			}
		}
}
