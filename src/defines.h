/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Tipos basicos */
/* -------------------------------------------------------------------------- */
#define NULL                    0
#define TRUE                    0x00000001
#define FALSE                   0x00000000
#define ERROR                   1

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;

/* Constantes basicas */
/* -------------------------------------------------------------------------- */
#define PAGE_SIZE               0x00001000
#define TASK_SIZE               4096

#define BOOTSECTOR              0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL                  0x00001200 /* direccion fisica de comienzo del kernel */


/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 31		//Lo aumentamos en 1 para poder tener el segmento de screen

#define GDT_IDX_NULL_DESC           0

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC			<< 3)
#define GDT_OFF_KERNEL_CODE_DESC    (GDT_ID8_KERNEL_CODE_DESC	<< 3)

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */
#define GDT_ID8_KERNEL_CODE_DESC	8
#define GDT_ID9_USER_CODE_DESC		9
#define GDT_ID10_KERNEL_DATA_DESC	10
#define GDT_ID11_USER_DATA_DESC		11
#define GDT_ID12_SCREEN_DESC		12
#define GDT_ID13_TSS_INICIAL_DESC	13
#define GDT_ID14_TSS_IDLE_DESC		14
#define GDT_ID15_TSS_JUG_A_PERRO_0	15
#define GDT_ID16_TSS_JUG_A_PERRO_1	16
#define GDT_ID17_TSS_JUG_A_PERRO_2	17
#define GDT_ID18_TSS_JUG_A_PERRO_3	18
#define GDT_ID19_TSS_JUG_A_PERRO_4	19
#define GDT_ID20_TSS_JUG_A_PERRO_5	20
#define GDT_ID21_TSS_JUG_A_PERRO_6	21
#define GDT_ID22_TSS_JUG_A_PERRO_7	22
#define GDT_ID23_TSS_JUG_B_PERRO_0	23
#define GDT_ID24_TSS_JUG_B_PERRO_1	24
#define GDT_ID25_TSS_JUG_B_PERRO_2	25
#define GDT_ID26_TSS_JUG_B_PERRO_3	26
#define GDT_ID27_TSS_JUG_B_PERRO_4	27
#define GDT_ID28_TSS_JUG_B_PERRO_5	28
#define GDT_ID29_TSS_JUG_B_PERRO_6	29
#define GDT_ID30_TSS_JUG_B_PERRO_7	30

#endif  /* !__DEFINES_H__ */
