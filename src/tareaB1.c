/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

// #include "game.h"
// #include "syscall.h"

// // ejemplo de uso de memoria compartida para la guerra de tareas:
// // parte (1)
// // declaramos un stuct que va a guardar datos en 0x401000
// //typedef struct extra_t {
// // las variables de instancia pueden ser lo que queramos
// //	char mapa[80*50];
// //	int next_id;
// //	int huesos[8][2];
// //	
// //} extra_t;


// void task(int x, int y) {
//     /* Tarea */

// // parte (2)
// //	extra_t *extra = (extra_t*)0x401000;
// // a partir de aqui tenemos la estructura disponible 


//     while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
// }

#include "colors.h"
#include "defines.h"
#include "game.h"
#include "syscall.h"
#include "i386.h"


void ir_hacia_desde(char x_dest, char y_dest, char x_actual, char y_actual);
void actualizar(int *x, int *y, int direccion);


void task(int x_origen, int y_origen) {
    /* Tarea */
 
	int x_actual = x_origen;
	int y_actual = y_origen;

	while (TRUE)
	{
		int direccion = syscall_olfatear();
		if (direccion == AQUI)
			break;
		
		actualizar(&x_actual, &y_actual, direccion);
		syscall_moverse(direccion);
	}
	while (syscall_cavar() != 0)
	{}
	

	ir_hacia_desde(x_origen, y_origen, x_actual, y_actual);

    while(1) { __asm __volatile("mov $2, %%eax":::"eax"); }
}

void actualizar(int *x, int *y, int direccion)
{
	switch (direccion)
	{
		case ARR: (*y)--; break;
		case ABA: (*y)++; break;
		case IZQ: (*x)--; break;
		case DER: (*x)++; break;
	}
}

void ir_hacia_desde(char x_dest, char y_dest, char x_actual, char y_actual)
{
	char dir_h = x_dest > x_actual ? DER : IZQ;
	char dir_v = y_dest > y_actual ? ABA : ARR;

	int dist_x = x_dest > x_actual ? x_dest - x_actual : x_actual - x_dest;	
	int dist_y = y_dest > y_actual ? y_dest - y_actual : y_actual - y_dest;	

	int i = 0;
	for (i = dist_x; i>0; i--)
		syscall_moverse(dir_h);

	for (i = dist_y; i>0; i--)
		syscall_moverse(dir_v);

}