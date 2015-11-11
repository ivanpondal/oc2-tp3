/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de las rutinas de atencion de interrupciones
*/

#ifndef __ISR_H__
#define __ISR_H__

#include "sched.h"
#include "game.h"

void _isr0x00();
void _isr0x01();
void _isr0x02();
void _isr0x03();
void _isr0x04();
void _isr0x05();
void _isr0x06();
void _isr0x07();
void _isr0x08();
void _isr0x09();
void _isr0x0A();
void _isr0x0B();
void _isr0x0C();
void _isr0x0D();
void _isr0x0E();
void _isr0x0F();
void _isr0x10();
void _isr0x11();
void _isr0x12();
void _isr0x13();

//Handler del clock
void _isr0x20();

//Handler del teclado
void _isr0x21();

// Servicio del sistema
void _isr0x46();


#endif  /* !__ISR_H__ */
