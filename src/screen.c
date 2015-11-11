/* ** por compatibilidad se omiten tildes **
================================================================================
TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
definicion de funciones del scheduler
*/

#include "screen.h"
#include "game.h"

#define POSICION_RELOJES_F   46
#define POSICION_RELOJES_C_A 4
#define POSICION_RELOJES_C_B 60
extern int ultimo_cambio;

extern jugador_t jugadorA, jugadorB;


static ca (*p)[VIDEO_COLS] = (ca (*)[VIDEO_COLS]) VIDEO;

const char reloj[] = "|/-\\";
#define reloj_size 4

ca backup[VIDEO_FILS][VIDEO_COLS];

struct {
  uint eax;
  uint ebx;
  uint ecx;
  uint edx;
  uint esi;
  uint edi;
  uint ebp;
  uint esp;
  uint eip;
  ushort cs;
  ushort ds;
  ushort es;
  ushort fs;
  ushort gs;
  ushort ss;
  uint eflags;
  uint cr0;
  uint cr2;
  uint cr3;
  uint cr4;
  uint stack0;
  uint stack1;
  uint stack2;
  uint stack3;
  uint stack4;
} __attribute__((__packed__)) debug_info;



void screen_actualizar_reloj_global()
{
    static uint reloj_global = 0;

    reloj_global = (reloj_global + 1) % reloj_size;

    screen_pintar(reloj[reloj_global], C_BW, 49, 79);
}

void screen_pintar(uchar c, uchar color, uint fila, uint columna)
{
    p[fila][columna].c = c;
    p[fila][columna].a = color;
}

uchar screen_valor_actual(uint fila, uint columna)
{
    return p[fila][columna].c;
}

void print(const char * text, uint x, uint y, unsigned short attr) {
    int i;
    for (i = 0; text[i] != 0; i++)
     {
        screen_pintar(text[i], attr, y, x);

        x++;
        if (x == VIDEO_COLS) {
            x = 0;
            y++;
        }
    }
}

void print_hex(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char hexa[8];
    char letras[16] = "0123456789ABCDEF";
    hexa[0] = letras[ ( numero & 0x0000000F ) >> 0  ];
    hexa[1] = letras[ ( numero & 0x000000F0 ) >> 4  ];
    hexa[2] = letras[ ( numero & 0x00000F00 ) >> 8  ];
    hexa[3] = letras[ ( numero & 0x0000F000 ) >> 12 ];
    hexa[4] = letras[ ( numero & 0x000F0000 ) >> 16 ];
    hexa[5] = letras[ ( numero & 0x00F00000 ) >> 20 ];
    hexa[6] = letras[ ( numero & 0x0F000000 ) >> 24 ];
    hexa[7] = letras[ ( numero & 0xF0000000 ) >> 28 ];
    for(i = 0; i < size; i++) {
        p[y][x + size - i - 1].c = hexa[i];
        p[y][x + size - i - 1].a = attr;
    }
}

void print_dec(uint numero, int size, uint x, uint y, unsigned short attr) {
    int i;
    char letras[16] = "0123456789";

    for(i = 0; i < size; i++) {
        int resto  = numero % 10;
        numero = numero / 10;
        p[y][x + size - i - 1].c = letras[resto];
        p[y][x + size - i - 1].a = attr;
    }
}


void screen_pintar_rect(uchar letra, uchar color, int fila, int columna, int alto, int ancho)
{
    int f, c;
    for (f = fila; f < fila + alto; f++)
    {
        for (c = columna; c < columna+ancho; c++)
            screen_pintar(letra, color, f, c);
    }
}

void screen_pintar_linea_h(uchar c, uchar color, int fila, int columna, int ancho)
{
    screen_pintar_rect(c, color, fila, columna, 1, ancho);
}

void screen_pintar_linea_v(uchar c, uchar color, int fila, int columna, int alto)
{
    screen_pintar_rect(c, color, fila, columna, alto, 1);
}

void screen_inicializar()
{
    screen_pintar_rect(' ', C_BG_BLACK | C_FG_WHITE, 0, 0, VIDEO_FILS, VIDEO_COLS);
    screen_pintar_rect(' ', C_BG_RED   | C_FG_WHITE, 45, 33, 5, 7);
    screen_pintar_rect(' ', C_BG_BLUE  | C_FG_WHITE, 45, 40, 5, 7);


    screen_pintar_relojes();
    screen_pintar_puntajes();

    int i, j;
    for (i = 0; i < MAPA_ALTO; i++)
    {
        for (j = 0; j < MAPA_ANCHO; j++)
            screen_actualizar_posicion_mapa(j, i);
    }
}

void screen_pintar_puntajes()
{
    print_dec(jugadorA.puntos, 3, 33+2, 45+2, C_BG_RED  | C_FG_WHITE);
    print_dec(jugadorB.puntos, 3, 40+2, 45+2, C_BG_BLUE | C_FG_WHITE);
}

uchar screen_color_jugador(jugador_t *j)
{
    if (j == NULL)
        return C_FG_LIGHT_GREY;

    if (j->index == JUGADOR_A)
        return C_FG_RED;
    else
        return C_FG_BLUE;
}

uchar screen_caracter_perro(uint tipo)
{
    if (tipo == TIPO_1) return '1';
    if (tipo == TIPO_2) return '2';
    while(1){};
    return '?';
}


void screen_pintar_reloj_perro(perro_t *perro)
{
	jugador_t *j = perro->jugador;
    uint columna = j->index == JUGADOR_A ? POSICION_RELOJES_C_A : POSICION_RELOJES_C_B;

    uchar col_libre = C_BG_BLACK | screen_color_jugador(j);

    uchar c, col;
    if (perro->libre == TRUE) {
        c = 'X';
        col = col_libre;
    } else {
        c = reloj[perro->indice_reloj];
        col = C_BW;
    }

    screen_pintar('1' + perro->index, C_BW, POSICION_RELOJES_F    , columna + perro->index * 2);
    screen_pintar(                  c,  col, POSICION_RELOJES_F + 2, columna + perro->index * 2);
}

void screen_pintar_reloj_perros(jugador_t *j)
{
    int i;
    for(i = 0; i < MAX_CANT_PERROS_VIVOS; i++)
        screen_pintar_reloj_perro(&j->perros[i]);
}

void screen_pintar_relojes()
{
    screen_pintar_reloj_perros(&jugadorA);
    screen_pintar_reloj_perros(&jugadorB);
}

void screen_actualizar_reloj_perro (perro_t *perro)
{
    perro->indice_reloj = (perro->indice_reloj + 1) % reloj_size;
    screen_pintar_reloj_perro(perro);
}


void screen_pintar_perro(perro_t *perro)
{
    uchar c     = screen_caracter_perro(perro->tipo);
    uchar color = C_MAKE_BG(screen_color_jugador(perro->jugador)) | C_FG_WHITE;

    screen_pintar(c, color, perro->y+1, perro->x);
}

void screen_borrar_perro(perro_t *perro)
{
    screen_pintar('.', C_BG_GREEN | C_FG_BLACK, perro->y+1, perro->x);
    screen_actualizar_posicion_mapa(perro->x, perro->y);
}

void screen_pintar_jugador(jugador_t *j)
{
    uchar c     = 'A' + j->index;
    uchar color = C_MAKE_BG(screen_color_jugador(j)) | C_FG_WHITE;

    screen_pintar(c, color, j->y+1, j->x);
}

void screen_borrar_jugador(jugador_t *j)
{
    screen_pintar('.', C_BG_GREEN | C_FG_BLACK, j->y+1, j->x);
    screen_actualizar_posicion_mapa(j->x, j->y);
}


uchar screen_caracter_tesoro(int valor) {
    if (valor > 100)
        return 'O';
    else
        return 'o';
}

void screen_actualizar_posicion_mapa(uint x, uint y)
{
    uchar bg = C_BG_GREEN;

    uchar letra;
    uint valor = game_huesos_en_posicion(x,y);
    perro_t *perro = game_perro_en_posicion(x, y);
    if (perro != NULL) {
        letra = screen_caracter_perro(perro->tipo);
    } else if (valor > 0) {
        letra = screen_caracter_tesoro(valor);
    } else if ((jugadorA.x_cucha == x && jugadorA.y_cucha == y) || (jugadorB.x_cucha == x && jugadorB.y_cucha == y))
    {
        letra = 'x';
    }
    else
    {
        letra = screen_valor_actual(y+1, x);
    }

    screen_pintar(letra, bg | C_FG_BLACK, y+1, x);

}


void screen_stop_game_show_winner(jugador_t *j) {
    int offy = 14; //(50/2 - 11);
    int offx = 20; //(80/2 - 20);

    int ancho = 40;
    int alto = 17;

    uchar color = screen_color_jugador(j);

    screen_pintar_rect(' ', C_MAKE_BG(color) | C_FG_WHITE, offy, offx, alto, ancho);

    offy++; offx++; alto -= 2; ancho-=2;

    screen_pintar_rect('*', C_BW, offy, offx, alto, ancho);

    offy++; offx++; alto -= 2; ancho-=2;

    screen_pintar_rect(' ', C_BG_LIGHT_GREY | C_FG_BLACK, offy, offx, alto, ancho);

    print("EL GANADOR ES EL JUGADOR",   offx+5  , offy+3 , C_BG_LIGHT_GREY | C_FG_BLACK);

    if(j == NULL)      print("EMPATE", offx+14, offy+6, C_MAKE_BG(color) | C_FG_BLACK);
    if(j == &jugadorA) print("<-- A",  offx+15, offy+6, C_BG_LIGHT_GREY  | color);
    if(j == &jugadorB) print("B -->",  offx+15, offy+6, C_BG_LIGHT_GREY  | color);
    // a partir de aca se termina el unviverso (STOP GAME)
    __asm __volatile( "cli\n" : : : );
    while(1){}
}

// Debug Mode

void screen_make_backup()
{
  uint i, j;
  for(i = 0; i<VIDEO_FILS; i++){
    for(j = 0; j<VIDEO_COLS; j++){
      backup[i][j] = p[i][j];
    }
  }
}

void screen_restore_backup()
{
  uint i, j;
  for(i = 0; i<VIDEO_FILS; i++){
    for(j = 0; j<VIDEO_COLS; j++){
      p[i][j] = backup[i][j];
    }
  }
}

void screen_pantalla_debug()
{
  uint x = 25, y = 7;

  screen_pintar_rect(0, 0x00, y, x, 36, 32);
  screen_pintar_rect(0, 0x77, y+1, x+1, 34, 30);
  screen_pintar_rect(0, 0x44, y+1, x+1, 1, 30);


  print("eax", x+2, y+3 , 0x70); print_hex(debug_info.eax, 8, x+6, y+3 , 0x7f);
  print("ebx", x+2, y+5 , 0x70); print_hex(debug_info.ebx, 8, x+6, y+5 , 0x7f);
  print("ecx", x+2, y+7 , 0x70); print_hex(debug_info.ecx, 8, x+6, y+7 , 0x7f);
  print("edx", x+2, y+9 , 0x70); print_hex(debug_info.edx, 8, x+6, y+9 , 0x7f);
  print("esi", x+2, y+11, 0x70); print_hex(debug_info.esi, 8, x+6, y+11, 0x7f);
  print("edi", x+2, y+13, 0x70); print_hex(debug_info.edi, 8, x+6, y+13, 0x7f);
  print("ebp", x+2, y+15, 0x70); print_hex(debug_info.ebp, 8, x+6, y+15, 0x7f);
  print("esp", x+2, y+17, 0x70); print_hex(debug_info.esp, 8, x+6, y+17, 0x7f);
  print("eip", x+2, y+19, 0x70); print_hex(debug_info.eip, 8, x+6, y+19, 0x7f);


  print("cs", x+3, y+21, 0x70); print_hex(debug_info.cs, 4, x+6, y+21, 0x7f);
  print("ds", x+3, y+23, 0x70); print_hex(debug_info.ds, 4, x+6, y+23, 0x7f);
  print("es", x+3, y+25, 0x70); print_hex(debug_info.es, 4, x+6, y+25, 0x7f);
  print("fs", x+3, y+27, 0x70); print_hex(debug_info.fs, 4, x+6, y+27, 0x7f);
  print("gs", x+3, y+29, 0x70); print_hex(debug_info.gs, 4, x+6, y+29, 0x7f);
  print("ss", x+3, y+31, 0x70); print_hex(debug_info.ss, 4, x+6, y+31, 0x7f);
  
  print("eflags", x+3, y+33, 0x70); print_hex(debug_info.eflags, 8, x+9, y+33, 0x7f);
  
  print("cr0", x+16, y+3, 0x70); print_hex(debug_info.cr0, 8, x+20, y+3, 0x7f);
  print("cr2", x+16, y+5, 0x70); print_hex(debug_info.cr2, 8, x+20, y+5, 0x7f);
  print("cr3", x+16, y+7, 0x70); print_hex(debug_info.cr3, 8, x+20, y+7, 0x7f);
  print("cr4", x+16, y+9, 0x70); print_hex(debug_info.cr4, 8, x+20, y+9, 0x7f);

  print("stack", x+16, y+20, 0x70); 
  print_hex(debug_info.stack0, 8, x+16, y+22, 0x7f);
  print_hex(debug_info.stack1, 8, x+16, y+23, 0x7f);
  print_hex(debug_info.stack2, 8, x+16, y+24, 0x7f);
  print_hex(debug_info.stack3, 8, x+16, y+25, 0x7f);
  print_hex(debug_info.stack4, 8, x+16, y+26, 0x7f);

}
