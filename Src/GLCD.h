/* GLCD */
#ifndef _GLCD_H_
#define _GLCD_H_

#include <stdint.h>
#include <delay.h>
#include <string.h>
#include <io.h>

//Settings
#define ControlDDR  DDRB
#define ControlPort PORTB
#define RS          PORTB.0
#define RW          PORTB.1
#define E           PORTB.2
#define CS1         PORTB.3
#define CS2         PORTB.4
#define D0          PORTA.0
#define D1          PORTA.1
#define D2          PORTA.2
#define D3          PORTA.3
#define D4          PORTA.4
#define D5          PORTA.5
#define D6          PORTA.6
#define D7          PORTA.7
#define DataPort    PORTA
#define DataDDR     DDRA
#define DataPin     PINA
#define BUSY 1 //0==RW -> GND , 1==RW->Micro-PIN
#define MODE 1
#define FONT1
#define FONT2
#define PERSIAN_FONT1
#define PERSIAN_FONT2


#define GLCD_CONST		flash

#pragma used+

#ifdef FONT1
	#include "Font1.h"
#endif
#ifdef FONT2
	#include "Font2.h"
#endif
#ifdef PERSIAN_FONT1
	#include "PersianFont1.h"
#endif
#ifdef PERSIAN_FONT2
	#include "PersianFont2.h"
#endif

void GLCD_init(void);
void GLCD_clear(void);
void GLCD_clearLine(uint8_t line);
void GLCD_gotoxy(uint8_t x, uint8_t y);
void GLCD_startline(uint8_t line);
void GLCD_off(void);
void GLCD_on(void);
void GLCD_writeByte(uint8_t byte);
void GLCD_write(uint8_t byte, uint8_t command);

uint8_t GLCD_read(uint8_t x, uint8_t y);
void GLCD_point(uint8_t x, uint8_t y, uint8_t color);
void GLCD_hLine(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c);
void GLCD_vLine(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c);
void GLCD_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c);
void GLCD_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c);
void GLCD_cuboid(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t s, uint8_t c);
void GLCD_hparallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l, uint8_t s, uint8_t c);
void GLCD_vparallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l, uint8_t s, uint8_t c);
void GLCD_hparallelepiped(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t l1, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t l2, uint8_t s, uint8_t c);
void GLCD_vparallelepiped(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t l1, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t l2, uint8_t s, uint8_t c);
void GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t s, uint8_t c);

void GLCD_putcharxy(uint8_t c, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size);
void GLCD_putcharPxy(uint8_t c, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size);
uint8_t GLCD_getCharPFont(uint8_t left, uint8_t c, uint8_t right);
void GLCD_putsP(uint8_t* str, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size);
void GLCD_puts(char* str, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size);

void GLCD_bitmap(uint8_t *bmp, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
uint8_t GLCD_readBitmapPixel(uint8_t *bmp,uint32_t length,uint32_t x,uint32_t y);

#pragma used-


#endif // _GLCD_H_
