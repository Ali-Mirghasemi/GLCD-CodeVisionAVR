#include "GLCD.h"

#if BUSY == 0
uint8_t GLCD_Buffer[128][8];
uint8_t GLCD_curserx = 0, GLCD_cursery = 0;
#endif

#pragma used+

static float GLCD_powerOfTen(int num) {
    float rst = 1.0;
    int   i;
    if (num >= 0) {
        for (i = 0; i < num; i++) {
            rst *= 10.0;
        }
    } else {
        for (i = 0; i < (0 - num); i++) {
            rst *= 0.1;
        }
    }
    return rst;
}
static float GLCD_sqrt(float a) {
    /*
          find more detail of this method on wiki methods_of_computing_square_roots

          *** Babylonian method cannot get exact zero but approximately value of the square_root
     */
    float z   = a;
    float rst = 0.0;
    int   max = 8;
    int   i;
    float j = 1.0;
    for (i = max; i > 0; i--) {

        if (z - ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i)) >= 0) {
            while (z - ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i)) >= 0) {
                j++;
                if (j >= 10)
                    break;
            }
            j--;
            z -= ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i));

            rst += j * GLCD_powerOfTen(i);

            j = 1.0;
        }
    }

    for (i = 0; i >= 0 - max; i--) {
        if (z - ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i)) >= 0) {
            while (z - ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i)) >= 0) {
                j++;
            }
            j--;
            z -= ((2 * rst) + (j * GLCD_powerOfTen(i))) * (j * GLCD_powerOfTen(i));

            rst += j * GLCD_powerOfTen(i);
            j = 1.0;
        }
    }

    return rst;
}
static uint8_t GLCD_min(uint8_t a, uint8_t b) {
    if (a < b)
        return a;
    else
        return b;
}
static uint8_t GLCD_max(uint8_t a, uint8_t b) {
    if (a < b)
        return b;
    else
        return a;
}
static int GLCD_abs(int a) {
    if (a < 0)
        return a * -1;
    else
        return a;
}
#pragma used-
/**
 * @brief write command or data to GLCD
 * 
 * @param byte 
 * @param command 
 */
void GLCD_write(uint8_t byte, uint8_t command) {
    if (command == 1) {
        RS = 0;
    } else {
        RS = 1;
#if BUSY == 0
        GLCD_Buffer[GLCD_curserx][GLCD_cursery] = byte;
        GLCD_curserx++;
#endif
    }
    E = 1;
    delay_us(3);
    DataPort = 0;
    DataPort = byte;
    delay_us(1);
    E = 0;
    delay_us(3);
}
/**
 * @brief write byte to GLCD
 * 
 * @param byte 
 */
void GLCD_writeByte(uint8_t byte) {
    GLCD_write(byte, 0);
}
/**
 * @brief glcd on
 * 
 */
void GLCD_on(void) {
#if (MODE == 0)
    CS1 = 0;
    CS2 = 0;
#endif
#if (MODE == 1)
    CS1 = 1;
    CS2 = 1;
#endif
    GLCD_write(0x3f, 1);
}
/**
 * @brief  glcd off
 * 
 */
void GLCD_off(void) {
#if (MODE == 0)
    CS1 = 0;
    CS2 = 0;
#endif
#if (MODE == 1)
    CS1 = 1;
    CS2 = 1;
#endif
    GLCD_write(0x3e, 1);
}
/**
 * @brief  initialize GLCD
 * 
 * @param line 
 */
void GLCD_startline(uint8_t line) {
#if (MODE == 0)
    CS1 = 0;
    CS2 = 0;
#endif
#if (MODE == 1)
    CS1 = 1;
    CS2 = 1;
#endif
    GLCD_write(0xc0 | line, 1);
}
/**
 * @brief  initialize GLCD
 * 
 * @param x 
 * @param y 
 */
void GLCD_gotoxy(uint8_t x, uint8_t y) {
#if BUSY == 0
    GLCD_curserx = x;
    GLCD_cursery = y;
#endif
    if (x < 64) {
#if (MODE == 0)
        CS1 = 0;
        CS2 = 1;
#endif
#if (MODE == 1)
        CS1 = 1;
        CS2 = 0;
#endif
        x = x;
    } else {
#if (MODE == 0)
        CS1 = 1;
        CS2 = 0;
#endif
#if (MODE == 1)
        CS1 = 0;
        CS2 = 1;
#endif
        x -= 64;
    }
    x = (x | 0x40) & 0x7F;
    GLCD_write(x, 1);
    y = (y | 0xB8) & 0xBF;
    GLCD_write(y, 1);
}
/**
 * @brief  clear line on GLCD
 * 
 * @param line 
 */
void GLCD_clearLine(uint8_t line) {
    uint8_t i = 0;
    for (i = 0; i < 128; i++) {
        GLCD_gotoxy(i, line);
        GLCD_writeByte(0);
    }
}
/**
 * @brief  clear GLCD
 * 
 */
void GLCD_clear(void) {
    uint8_t i = 0;
    for (i = 0; i < 8; i++)
        GLCD_clearLine(i);
}
/**
 * @brief  read data from GLCD
 * 
 * @param x 
 * @param y 
 * @return uint8_t 
 */
uint8_t GLCD_read(uint8_t x, uint8_t y) {
#if BUSY == 0
    return GLCD_Buffer[x][y];
#endif
#if (BUSY == 1)
    uint8_t read_data = 0;
    DataDDR           = 0x00;
    RW                = 1;
    RS                = 1;
    GLCD_gotoxy(x, y);
    delay_us(1);
    E = 1;
    delay_us(1);
    E = 0;
    delay_us(20);
    E = 1;
    delay_us(1);
    read_data = DataPin;
    E         = 0;
    delay_us(1);
    DataDDR = 0xff;
    return read_data;
#endif
}
/**
 * @brief  display horizontal line on GLCD
 * 
 * @param x 
 * @param y 
 * @param color 
 */
void GLCD_point(uint8_t x, uint8_t y, uint8_t color) {
    GLCD_gotoxy(x, (y / 8));
    switch (color) {
    case 0:
        y = ~(1 << (y % 8)) & GLCD_read(x, (y / 8));
        break;
    case 1:
        y = (1 << (y % 8)) | GLCD_read(x, (y / 8));
        break;
    }
    GLCD_writeByte(y);
}
/**
 * @brief  display horizontal line on GLCD
 * 
 * @param x 
 * @param y 
 * @param l 
 * @param s 
 * @param c 
 */
void GLCD_hLine(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c) {
    uint8_t i;
    for (i = x; i < (l + x); i += (uint8_t)(s + 1))
        GLCD_point(i, y, c);
}
/**
 * @brief  display vertical line on GLCD
 * 
 * @param x 
 * @param y 
 * @param l 
 * @param s 
 * @param c 
 */
void GLCD_vLine(uint8_t x, uint8_t y, uint8_t l, uint8_t s, uint8_t c) {
    uint8_t i;
    for (i = y; i < (y + l); i += (uint8_t)(s + 1))
        GLCD_point(x, i, c);
}
/**
 * @brief  display line on GLCD
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param s 
 * @param c 
 */
void GLCD_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c) {
    uint8_t i;
    uint8_t y01;

    uint8_t temp;

    float a;
    float b;

    uint8_t y00;
    uint8_t y010;

    if (x1 == x2) {
        GLCD_vLine(x1, GLCD_min(y1, y2), GLCD_abs(y2 - y1) + 1, s, c);
    } else if (y1 == y2) {
        GLCD_hLine(GLCD_min(x1, x2), y1, GLCD_abs(x2 - x1) + 1, s, c);
    } else {
        if (x1 > x2) {
            temp = x1;
            x1   = x2;
            x2   = temp;
            temp = y1;
            y1   = y2;
            y2   = temp;
        }

        a    = (float)(signed)(y2 - y1) / (x2 - x1);
        b    = y1 - a * x1;
        y00  = a * x1 + b;
        y010 = y00;

        for (i = (x1 + 1); i <= x2; i++) {
            y01 = a * i + b + 0.5;

            if ((signed char)(y01 - y010) > 1) {
                GLCD_vLine((i - 1), (uint8_t)(y010 + 1), (y01 - y010 - 1), s, c);
            } else if ((signed char)(y010 - y01) > 1) {
                GLCD_vLine((i - 1), (uint8_t)(y01 + 1), (y010 - y01 - 1), s, c);
            }

            if ((i == x2) && (y00 == y01))
                GLCD_hLine(x1, y01, (x2 - x1), 0, c);
            y010 = y01;
            if (y00 == y01)
                continue;
            GLCD_hLine(x1, y00, (i - x1), 0, c);
            x1  = i;
            y00 = y01;
        }
        GLCD_point(x2, y2, c);
    }
}
/**
 * @brief  display rectangle on GLCD
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param s 
 * @param c 
 */
void GLCD_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t s, uint8_t c) {
    GLCD_hLine(x1, y1, (x2 - x1), s, c);
    GLCD_hLine(x1, y2, (x2 - x1), s, c);
    GLCD_vLine(x1, y1, (y2 - y1), s, c);
    GLCD_vLine(x2, y1, (y2 - y1 + 1), s, c);
}
/**
 * @brief  display cuboid on GLCD
 * 
 * @param x11 
 * @param y11 
 * @param x12 
 * @param y12 
 * @param x21 
 * @param y21 
 * @param x22 
 * @param y22 
 * @param s 
 * @param c 
 */
void GLCD_cuboid(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t s, uint8_t c) {
    GLCD_rectangle(x11, y11, x12, y12, s, c);
    GLCD_rectangle(x21, y21, x22, y22, s, c);
    GLCD_line(x11, y11, x21, y21, s, c);
    GLCD_line(x12, y11, x22, y21, s, c);
    GLCD_line(x11, y12, x21, y22, s, c);
    GLCD_line(x12, y12, x22, y22, s, c);
}
/**
 * @brief  display parallelogram on GLCD
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param l 
 * @param s 
 * @param c 
 */
void GLCD_hparallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l, uint8_t s, uint8_t c) {
    GLCD_hLine(x1, y1, l, s, c);
    GLCD_hLine((x2 - l + 1), y2, l, s, c);
    GLCD_line(x1, y1, (x2 - l + 1), y2, s, c);
    GLCD_line((x1 + l - 1), y1, x2, y2, s, c);
}
/**
 * @brief  display parallelogram on GLCD
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param l 
 * @param s 
 * @param c 
 */
void GLCD_vparallelogram(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t l, uint8_t s, uint8_t c) {
    GLCD_vLine(x1, y1, l, s, c);
    GLCD_vLine(x2, (y2 - l + 1), l, s, c);
    GLCD_line(x1, y1, x2, (y2 - l + 1), s, c);
    GLCD_line(x1, (y1 + l - 1), x2, y2, s, c);
}
/**
 * @brief  display parallelepiped on GLCD
 *
 * @param x11
 * @param y11
 * @param x12
 * @param y12
 * @param l1
 * @param x21
 * @param y21
 * @param x22
 * @param y22
 * @param l2
 * @param s
 * @param c
 */
void GLCD_hparallelepiped(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t l1, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t l2, uint8_t s, uint8_t c) {
    GLCD_hparallelogram(x11, y11, x12, y12, l1, s, c);
    GLCD_hparallelogram(x21, y21, x22, y22, l2, s, c);
    GLCD_line(x11, y11, x21, y21, s, c);
    GLCD_line(x12, y12, x22, y22, s, c);
    GLCD_line((x11 + l1 - 1), y11, (x21 + l2 - 1), y21, s, c);
    GLCD_line((x12 - l1 + 1), y12, (x22 - l2 + 1), y22, s, c);
}
/**
 * @brief  display parallelepiped on GLCD
 *
 * @param x11
 * @param y11
 * @param x12
 * @param y12
 * @param l1
 * @param x21
 * @param y21
 * @param x22
 * @param y22
 * @param l2
 * @param s
 * @param c
 */
void GLCD_vparallelepiped(uint8_t x11, uint8_t y11, uint8_t x12, uint8_t y12, uint8_t l1, uint8_t x21, uint8_t y21, uint8_t x22, uint8_t y22, uint8_t l2, uint8_t s, uint8_t c) {
    GLCD_vparallelogram(x11, y11, x12, y12, l1, s, c);
    GLCD_vparallelogram(x21, y21, x22, y22, l2, s, c);
    GLCD_line(x11, y11, x21, y21, s, c);
    GLCD_line(x12, y12, x22, y22, s, c);
    GLCD_line(x11, (y11 + l1 - 1), x21, (y21 + l2 - 1), s, c);
    GLCD_line(x12, (y12 - l1 + 1), x22, (y22 - l2 + 1), s, c);
}
/**
 * @brief  display circle on GLCD
 *
 * @param x0
 * @param y0
 * @param r
 * @param s
 * @param c
 */
void GLCD_circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t s, uint8_t c) {
    uint8_t i, y, y00;
    y00 = r; // Point (0,r) is the 1st point
    for (i = 0; i < r; i++) {
        y = GLCD_sqrt((int)r * r - (int)i * i);
        GLCD_point((x0 + i), (y0 + y), c);
        GLCD_point((x0 + i), (y0 - y), c);
        GLCD_point((x0 - i), (y0 + y), c);
        GLCD_point((x0 - i), (y0 - y), c);
        if (GLCD_abs(y00 - y) > 1) {
            GLCD_vLine(x0 + (i - 1), y0 + GLCD_min(y00, y) + 1, GLCD_abs(y00 - y), s, c);
            GLCD_vLine(x0 + (i - 1), y0 - GLCD_max(y00, y), GLCD_abs(y00 - y), s, c);
            GLCD_vLine(x0 - (i - 1), y0 + GLCD_min(y00, y) + 1, GLCD_abs(y00 - y), s, c);
            GLCD_vLine(x0 - (i - 1), y0 - GLCD_max(y00, y), GLCD_abs(y00 - y), s, c);
        }
        y00 = y;
    }
    GLCD_vLine(x0 + (i - 1), y0, y, s, c);
    GLCD_vLine(x0 + (i - 1), y0 - y, y, s, c);
    GLCD_vLine(x0 - (i - 1), y0, y, s, c);
    GLCD_vLine(x0 - (i - 1), y0 - y, y, s, c);
}
/**
 * @brief  display char on GLCD
 *
 * @param c
 * @param x
 * @param y
 * @param font
 * @param size
 */
void GLCD_putcharxy(uint8_t c, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size) {
    uint16_t i, j = 0;
    if (size < 2) {
        for (i = 0; i < 8; i++) {
            GLCD_gotoxy(x + i, y);
            GLCD_writeByte(font[(8 * c) + i]);
        }
    }
    if (size == 2) {
        for (i = 0; i < 16; i++) {
            GLCD_gotoxy(x + i, y);
            GLCD_writeByte(font[(128 * 8) + (c * 32) + j]);
            j++;
            GLCD_gotoxy(x + i, y + 1);
            GLCD_writeByte(font[(128 * 8) + (c * 32) + j]);
            j++;
        }
    }
}
/**
 * @brief  display char on GLCD
 *
 * @param c
 * @param x
 * @param y
 * @param font
 * @param size
 */
void GLCD_putcharPxy(uint8_t c, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size) {
    uint16_t i, j = 0;
    if (size < 2) {
        for (i = 0; i < 8; i++) {
            GLCD_gotoxy(x + i, y);
            GLCD_writeByte(font[(8 * c) + i]);
        }
    }
    if (size == 2) {
        for (i = 0; i < 16; i++) {
            GLCD_gotoxy(x + i, y);
            GLCD_writeByte(font[(210 * 8) + (c * 32) + j]);
            j++;
            GLCD_gotoxy(x + i, y + 1);
            GLCD_writeByte(font[(210 * 8) + (c * 32) + j]);
            j++;
        }
    }
}
/**
 * @brief  display string on GLCD
 *
 * @param left
 * @param c
 * @param right
 * @return uint8_t
 */
uint8_t GLCD_getCharPFont(uint8_t left, uint8_t c, uint8_t right) {
    if (left == 1 && right == 0) {
        switch (c) {
        case PersianFont_be:
            return PersianFont__be;
        case PersianFont_pe:
            return PersianFont__pe;
        case PersianFont_te:
            return PersianFont__te;
        case PersianFont_se:
            return PersianFont__se;
        case PersianFont_jim:
            return PersianFont__jim;
        case PersianFont_che:
            return PersianFont__che;
        case PersianFont_he:
            return PersianFont__he;
        case PersianFont_khe:
            return PersianFont__khe;
        case PersianFont_sin:
            return PersianFont__sin;
        case PersianFont_shin:
            return PersianFont__shin;
        case PersianFont_sad:
            return PersianFont__sad;
        case PersianFont_zad:
            return PersianFont__zad;
        case PersianFont_ta:
            return PersianFont__ta;
        case PersianFont_za:
            return PersianFont__za;
        case PersianFont_ain:
            return PersianFont__ain;
        case PersianFont_ghain:
            return PersianFont__ghain;
        case PersianFont_fe:
            return PersianFont__fe;
        case PersianFont_ghaf:
            return PersianFont__ghaf;
        case PersianFont_ke:
            return PersianFont__ke;
        case PersianFont_ge:
            return PersianFont__ge;
        case PersianFont_lam:
            return PersianFont__lam;
        case PersianFont_mim:
            return PersianFont__mim;
        case PersianFont_non:
            return PersianFont__non;
        case PersianFont_hee:
            return PersianFont__hee;
        case PersianFont_ye:
            return PersianFont__ye;
        }
    }
    if (right == 1 && left == 0) {
        switch (c) {
        case PersianFont_alf:
            return PersianFont_alf_;
        case PersianFont_be:
            return PersianFont_be_;
        case PersianFont_pe:
            return PersianFont_pe_;
        case PersianFont_te:
            return PersianFont_te_;
        case PersianFont_se:
            return PersianFont_se_;
        case PersianFont_jim:
            return PersianFont_jim_;
        case PersianFont_che:
            return PersianFont_che_;
        case PersianFont_he:
            return PersianFont_he_;
        case PersianFont_khe:
            return PersianFont_khe_;
        case PersianFont_dal:
            return PersianFont_dal_;
        case PersianFont_zal:
            return PersianFont_zal_;
        case PersianFont_re:
            return PersianFont_re_;
        case PersianFont_ze:
            return PersianFont_ze_;
        case PersianFont_zhe:
            return PersianFont_zhe_;
        case PersianFont_sin:
            return PersianFont_sin_;
        case PersianFont_shin:
            return PersianFont_shin_;
        case PersianFont_sad:
            return PersianFont_sad_;
        case PersianFont_zad:
            return PersianFont_zad_;
        case PersianFont_ta:
            return PersianFont_ta_;
        case PersianFont_za:
            return PersianFont_za_;
        case PersianFont_ain:
            return PersianFont_ain_;
        case PersianFont_ghain:
            return PersianFont_ghain_;
        case PersianFont_fe:
            return PersianFont_fe_;
        case PersianFont_ghaf:
            return PersianFont_ghaf_;
        case PersianFont_ke:
            return PersianFont_ke_;
        case PersianFont_ge:
            return PersianFont_ge_;
        case PersianFont_lam:
            return PersianFont_lam_;
        case PersianFont_mim:
            return PersianFont_mim_;
        case PersianFont_non:
            return PersianFont_non_;
        case PersianFont_vav:
            return PersianFont_vav_;
        case PersianFont_hee:
            return PersianFont_hee_;
        case PersianFont_ye:
            return PersianFont_ye_;
        }
    }
    if (left == 1 && right == 1) {
        switch (c) {
        case PersianFont_be:
            return PersianFont__be_;
        case PersianFont_pe:
            return PersianFont__pe_;
        case PersianFont_te:
            return PersianFont__te_;
        case PersianFont_se:
            return PersianFont__se_;
        case PersianFont_jim:
            return PersianFont__jim_;
        case PersianFont_che:
            return PersianFont__che_;
        case PersianFont_he:
            return PersianFont__he_;
        case PersianFont_khe:
            return PersianFont__khe_;
        case PersianFont_sin:
            return PersianFont__sin_;
        case PersianFont_shin:
            return PersianFont__shin_;
        case PersianFont_sad:
            return PersianFont__sad_;
        case PersianFont_zad:
            return PersianFont__zad_;
        case PersianFont_ta:
            return PersianFont__ta_;
        case PersianFont_za:
            return PersianFont__za_;
        case PersianFont_ain:
            return PersianFont__ain_;
        case PersianFont_ghain:
            return PersianFont__ghain_;
        case PersianFont_fe:
            return PersianFont__fe_;
        case PersianFont_ghaf:
            return PersianFont__ghaf_;
        case PersianFont_ke:
            return PersianFont__ke_;
        case PersianFont_ge:
            return PersianFont__ge_;
        case PersianFont_lam:
            return PersianFont__lam_;
        case PersianFont_mim:
            return PersianFont__mim_;
        case PersianFont_non:
            return PersianFont__non_;
        case PersianFont_hee:
            return PersianFont__hee_;
        case PersianFont_ye:
            return PersianFont__ye_;
        }
    }
}
/**
 * @brief  display string on GLCD
 *
 * @param str
 * @param x
 * @param y
 * @param font
 * @param size
 */
void GLCD_putsP(uint8_t* str, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size) {
    uint16_t i    = 0;
    uint8_t  left = 0, right = 0;
    while (str[i] != '\0') {
        if (size < 2)
            x -= 8;
        if (size == 2)
            x -= 16;
        if (i == 0) {
            if ((str[i] != 'h') && (str[i] != 'n') && (str[i] != 'b') && (str[i] != 'v') && (str[i] != 'c') && (str[i] != 'C') && (str[i] != ',')) {
                if (str[i + 1] != '\0' && str[i + 1] != ' ' && str[i + 1] != '.') {
                    left = 1;
                } else
                    left = 0;
            } else
                left = 0;
            right = 0;
            if (left == 0 && right == 0)
                GLCD_putcharPxy(str[i], x, y, font, size);
            else
                GLCD_putcharPxy(GLCD_getCharPFont(left, str[i], right), x, y, font, size);
        }
        if (i > 0) {
            if ((str[i] != 'h') && (str[i] != 'n') && (str[i] != 'b') && (str[i] != 'v') && (str[i] != 'c') && (str[i] != 'C') && (str[i] != ',')) {
                if (str[i + 1] != '\0' && str[i + 1] != ' ' && str[i + 1] != '.') {
                    left = 1;
                } else
                    left = 0;
            } else
                left = 0;
            if ((str[i - 1] != 'h') && (str[i - 1] != 'n') && (str[i - 1] != 'b') && (str[i - 1] != 'v') && (str[i - 1] != 'c') && (str[i - 1] != 'C') && (str[i - 1] != ',')) {
                if (str[i - 1] != ' ') {
                    right = 1;
                } else
                    right = 0;
            } else
                right = 0;
            if (left == 0 && right == 0)
                GLCD_putcharPxy(str[i], x, y, font, size);
            else
                GLCD_putcharPxy(GLCD_getCharPFont(left, str[i], right), x, y, font, size);
        }
        if (size < 2) {
            if (x < 8) {
                x = 127;
                y++;
                if (y > 7) {
                    y = 0;
                }
            }
        }
        if (size == 2) {
            if (x < 16) {
                x = 127;
                y += 2;
                if (y > 7) {
                    y = 0;
                }
            }
        }
        i++;
    }
}
/**
 * @brief  display string on GLCD
 *
 * @param str
 * @param x
 * @param y
 * @param font
 * @param size
 */
void GLCD_puts(char* str, uint8_t x, uint8_t y, GLCD_CONST uint8_t* font, uint8_t size) {
    uint16_t i = 0;
    while (str[i] != '\0') {
        GLCD_putcharxy(str[i], x, y, font, size);
        if (size < 2)
            x += 8;
        if (size == 2)
            x += 16;
        i++;
        if (size < 2) {
            if (x > 127) {
                x = 0;
                y++;
                if (y > 7) {
                    y = 0;
                }
            }
        }
        if (size == 2) {
            if (x > 127) {
                x = 0;
                y += 2;
                if (y > 7) {
                    y = 0;
                }
            }
        }
    }
}
/**
 * @brief  display bitmap on GLCD
 *
 * @param bmp
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void GLCD_bitmap(uint8_t* bmp, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    uint16_t i, j;
    for (i = y1; i <= y2; i++) {
        for (j = x1; j <= x2; j++) {
            GLCD_gotoxy(j, i);
            GLCD_writeByte(bmp[(128 * i) + j]);
        }
    }
}
/**
 * @brief  read pixel from bitmap
 *
 * @param bmp
 * @param length
 * @param x
 * @param y
 * @return uint8_t
 */
uint8_t GLCD_readBitmapPixel(uint8_t* bmp, uint32_t length, uint32_t x, uint32_t y) {
#if BUSY == 0
    return (bmp[(length * (y / 8)) + x] & (1 << (y % 8))) >> (y % 8);
#else
#warning "This function is not supported in BUSY mode"
    // UNUSED variables
    (void) bmp;
    (void) length;
    (void) x;
    (void) y;
    return 0;
#endif
}
// glcd move settings
// glcd move mode 1 and so on it's work just on GLCD_movestep 1
#define GLCD_movestep 4
#define GLCD_movemode 0 // 1...
// modes for this function
// 1:up    to inside
// 2:down  to inside
// 3:left  to inside
// 4:right to inside
// 5:inside to up
// 6:inside to down
// 7:inside to left
// 8:inside to right
/*void GLCD_Bitmapmove(uint8_t *bmp,uint32_t startx,uint32_t starty ,uint32_t height,uint32_t length,uint8_t mode,uint8_t speed)
{
        int i,j,c,S;
        #if GLCD_movestep == 1
        uint8_t temp0[128];
        #elif GLCD_movestep == 2
        uint8_t temp0[128];
        uint8_t temp1[128];
        #elif GLCD_movestep == 3
        uint8_t temp0[128];
        uint8_t temp1[128];
        uint8_t temp2[128];
        #elif GLCD_movestep == 4
        uint8_t temp0[128];
        uint8_t temp1[128];
        uint8_t temp2[128];
        uint8_t temp3[128];
        #endif
        GLCD_clear();
        switch(mode)
        {
                case 1: //up to inside ->up down
                                S=height;
                                for(i = height-1 ; i>=0 ; i-=GLCD_movestep)
                                        {
                                                for(c=0 ; c<length ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-2);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-2);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-3);
                                                                #endif
                                                        }
                                                for(j=0 ; j<S ; j+=GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<length ; c++)
                                                                {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(c+startx,j+0+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(c+startx,j+0+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(c+startx,j+0+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+2+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(c+startx,j+0+starty,temp3[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j+2+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+3+starty,temp0[c]);
                                                                        #endif
                                                                }
                                                                if(j-(GLCD_movestep+GLCD_movemode) >=0)
                                                                {
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j-(1+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j-(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(2+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j-(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(2+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(3+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j-(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(2+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(3+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j-(4+GLCD_movemode+starty),0);
                                                                                #endif
                                                                        }
                                                                }
                                                                delay_us(speed*1000);
                                                        }
                                                        #if GLCD_movemode != 0
                                                        if(S-(GLCD_movemode+GLCD_movestep) >= 0)
                                                        {
                                                        for(j=S-(GLCD_movemode+GLCD_movestep) ; j<(S-1) ; j+= GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                GLCD_point(c+startx,j+starty,0);
                                                                        }
                                                                        delay_us(speed*1000);
                                                        }
                                                        }
                                                        else
                                                        {
                                                                        for(j=0 ; j<(S-1) ; j+= GLCD_movestep)
                                                                                {
                                                                                for(c=0 ; c<length ; c++)
                                                                                {
                                                                                        GLCD_point(c+startx,j+starty,0);
                                                                                }
                                                                                delay_us(speed*1000);
                                                                                }
                                                        }
                                                        #endif
                                                        S-=GLCD_movestep;
                                        }
                                        break;
                case 2: //down to inside ->down up
                                S=0;
                                for(i = 0 ; i<height ; i+=GLCD_movestep)
                                        {
                                                for(c=0 ; c<length ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+2);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+2);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+3);
                                                                #endif
                                                        }
                                                for(j=height-1 ; j>=S ; j-=GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<length ; c++)
                                                                {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(c+startx,j-0+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(c+startx,j-0+starty,temp0[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp1[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(c+startx,j-0+starty,temp0[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j-2+starty,temp2[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(c+startx,j-0+starty,temp0[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j-2+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j-3+starty,temp3[c]);
                                                                        #endif
                                                                }
                                                                if(j+(GLCD_movestep+GLCD_movemode) <= height-1)
                                                                {
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j+(1+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j+(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(2+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j+(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(2+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(3+GLCD_movemode+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j+(1+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(2+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(3+GLCD_movemode+starty),0);
                                                                                GLCD_point(c+startx,j+(4+GLCD_movemode+starty),0);
                                                                                #endif
                                                                        }
                                                                }
                                                                delay_us(speed*1000);
                                                        }
                                                        #if GLCD_movemode != 0
                                                        if(S+(GLCD_movemode+GLCD_movestep) <= height-1)
                                                        {
                                                        for(j=S+(GLCD_movemode+GLCD_movestep) ; j>(S) ; j-= GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                GLCD_point(c+startx,j+starty,0);
                                                                        }
                                                                        delay_us(speed*1000);
                                                        }
                                                        }
                                                        else
                                                        {
                                                                        for(j=63 ; j>(S) ; j--)
                                                                                {
                                                                                for(c=0 ; c<length ; c++)
                                                                                {
                                                                                        GLCD_point(c+startx,j+starty,0);
                                                                                }
                                                                                delay_us(speed*1000);
                                                                                }
                                                        }
                                                        #endif
                                                        S+=GLCD_movestep;
                                        }
                                        break;
                case 3: //left to inside -> left right
                                S=length;
                                for(i = length-1 ; i>=0 ; i-=GLCD_movestep)
                                        {
                                                for(c=0 ; c<height ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i-2,c);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i-2,c);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,i-3,c);
                                                                #endif
                                                        }
                                                for(j=0 ; j<S ; j+=GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<height ; c++)
                                                                {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(j+0+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(j+0+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(j+0+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+2+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(j+0+startx,c+starty,temp3[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j+2+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+3+startx,c+starty,temp0[c]);
                                                                        #endif
                                                                }
                                                                if(j-(GLCD_movestep+GLCD_movemode) >=0)
                                                                {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j-(1+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j-(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(2+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j-(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(2+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(3+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j-(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(2+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(3+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j-(4+GLCD_movemode+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                                }
                                                                delay_us(speed*1000);
                                                        }
                                                        #if GLCD_movemode != 0
                                                        if(S-(GLCD_movemode+GLCD_movestep) >= 0)
                                                        {
                                                        for(j=S-(GLCD_movemode+GLCD_movestep) ; j<(S-1) ; j+= GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                GLCD_point(j+startx,c+starty,0);
                                                                        }
                                                                        delay_us(speed*1000);
                                                        }
                                                        }
                                                        else
                                                        {
                                                                        for(j=0 ; j<(S-1) ; j+= GLCD_movestep)
                                                                                {
                                                                                for(c=0 ; c<height ; c++)
                                                                                {
                                                                                        GLCD_point(j+startx,c+starty,0);
                                                                                }
                                                                                delay_us(speed*1000);
                                                                                }
                                                        }
                                                        #endif
                                                        S-=GLCD_movestep;
                                        }
                                        break;
                case 4: //right to inside -> right left
                                S=0;
                                for(i = 0 ; i<length ; i+=GLCD_movestep)
                                        {
                                                for(c=0 ; c<height ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i+2,c);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i+2,c);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,i+3,c);
                                                                #endif
                                                        }
                                                for(j=length-1 ; j>=S ; j-=GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<height ; c++)
                                                                {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(j-0+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(j-0+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(j-0+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-2+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(j-0+startx,c+starty,temp3[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j-2+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-3+startx,c+starty,temp0[c]);
                                                                        #endif
                                                                }
                                                                if(j+(GLCD_movestep+GLCD_movemode) <=length-1)
                                                                {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j+(1+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j+(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(2+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j+(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(2+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(3+GLCD_movemode+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j+(1+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(2+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(3+GLCD_movemode+startx),c+starty,0);
                                                                                GLCD_point(j+(4+GLCD_movemode+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                                }
                                                                delay_us(speed*1000);
                                                        }
                                                        #if GLCD_movemode != 0
                                                        if(S+(GLCD_movemode+GLCD_movestep) <= length-1)
                                                        {
                                                        for(j=S+(GLCD_movemode+GLCD_movestep) ; j>(S) ; j-= GLCD_movestep)
                                                        {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                GLCD_point(j+startx,c+starty,0);
                                                                        }
                                                                        delay_us(speed*1000);
                                                        }
                                                        }
                                                        else
                                                        {
                                                                        for(j=length-1 ; j>(S) ; j-= GLCD_movestep)
                                                                                {
                                                                                for(c=0 ; c<height ; c++)
                                                                                {
                                                                                        GLCD_point(j+startx,c+starty,0);
                                                                                }
                                                                                delay_us(speed*1000);
                                                                                }
                                                        }
                                                        #endif
                                                        S+=GLCD_movestep;
                                        }
                                        break;
                case 5: //inside to up -> down up
                                GLCD_Bitmap(bmp,startx,(starty%8),length-1,(height/8)-1);
                                S=0;
                                for(i = 0 ; i<height ; i+=GLCD_movestep)
                                        {
                                                for(c=0 ; c<length ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+2);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+2);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,c,i+3);
                                                                #endif
                                                        }
                                                        j=0;
                                                        for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j+(0+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j+(0+starty),0);
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j+(0+starty),0);
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                GLCD_point(c+startx,j+(2+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j+(0+starty),0);
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                GLCD_point(c+startx,j+(2+starty),0);
                                                                                GLCD_point(c+startx,j+(3+starty),0);
                                                                                #endif
                                                                        }
                                                for(j=S ; j>=0 ; j-=GLCD_movestep)
                                                        {
                                                                if(j-(GLCD_movestep) >= 0)
                                                                {
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(c+startx,j-0+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(c+startx,j-0+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(c+startx,j-0+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j-2+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(c+startx,j-0+starty,temp3[c]);
                                                                        GLCD_point(c+startx,j-1+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j-2+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j-3+starty,temp0[c]);
                                                                        #endif
                                                                        }
                                                                }
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                GLCD_point(c+startx,j+(2+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                GLCD_point(c+startx,j+(2+starty),0);
                                                                                GLCD_point(c+startx,j+(3+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j+(1+starty),0);
                                                                                GLCD_point(c+startx,j+(2+starty),0);
                                                                                GLCD_point(c+startx,j+(3+starty),0);
                                                                                GLCD_point(c+startx,j+(4+starty),0);
                                                                                #endif
                                                                        }
                                                                delay_us(speed*1000);
                                                        }
                                                        S+=GLCD_movestep;
                                        }
                                        break;
                case 6: //inside to down ->up down
                                GLCD_Bitmap(bmp,startx,(starty%8),length-1,(height/8)-1);
                                S=height-1;
                                for(i = height-1 ; i>=0 ; i-=GLCD_movestep)
                                        {
                                                for(c=0 ; c<length ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-2);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-0);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-1);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-2);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,c,i-3);
                                                                #endif
                                                        }
                                                        j=height-1;
                                                        for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j-(0+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j-(0+starty),0);
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j-(0+starty),0);
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                GLCD_point(c+startx,j-(2+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j-(0+starty),0);
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                GLCD_point(c+startx,j-(2+starty),0);
                                                                                GLCD_point(c+startx,j-(3+starty),0);
                                                                                #endif
                                                                        }
                                                for(j=S ; j<=63 ; j+=GLCD_movestep)
                                                        {
                                                                if(j+(GLCD_movestep) <=63)
                                                                {
                                                                for(c=0 ; c<length ; c++)
                                                                {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(c+startx,j+0+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(c+startx,j+0+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(c+startx,j+0+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+2+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(c+startx,j+0+starty,temp3[c]);
                                                                        GLCD_point(c+startx,j+1+starty,temp2[c]);
                                                                        GLCD_point(c+startx,j+2+starty,temp1[c]);
                                                                        GLCD_point(c+startx,j+3+starty,temp0[c]);
                                                                        #endif
                                                                }
                                                                }
                                                                for(c=0 ; c<length ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                GLCD_point(c+startx,j-(2+starty),0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                GLCD_point(c+startx,j-(2+starty),0);
                                                                                GLCD_point(c+startx,j-(3+starty),0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(c+startx,j-(1+starty),0);
                                                                                GLCD_point(c+startx,j-(2+starty),0);
                                                                                GLCD_point(c+startx,j-(3+starty),0);
                                                                                GLCD_point(c+startx,j-(4+starty),0);
                                                                                #endif
                                                                        }

                                                                delay_us(speed*1000);
                                                        }
                                                        S-=GLCD_movestep;
                                        }
                                        break;
                case 7: //inside to left -> right left
                                GLCD_Bitmap(bmp,startx,(starty%8),length-1,(height/8)-1);
                                S=startx;
                                for(i = 0 ; i<length ; i+=GLCD_movestep)
                                        {
                                                for(c=0 ; c<height ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i+2,c);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i+0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i+1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i+2,c);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,i+3,c);
                                                                #endif
                                                        }
                                                j=0;
                                                        for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j+(0+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j+(0+startx),c+starty,0);
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j+(0+startx),c+starty,0);
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                GLCD_point(j+(2+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j+(0+startx),c+starty,0);
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                GLCD_point(j+(2+startx),c+starty,0);
                                                                                GLCD_point(j+(3+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                for(j=S ; j>=0 ; j-=GLCD_movestep)
                                                        {
                                                                if(j-(GLCD_movestep) >=0)
                                                                {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(j-0+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(j-0+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(j-0+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-2+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(j-0+startx,c+starty,temp3[c]);
                                                                        GLCD_point(j-1+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j-2+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j-3+startx,c+starty,temp0[c]);
                                                                        #endif
                                                                        }
                                                                }
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                GLCD_point(j+(2+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                GLCD_point(j+(2+startx),c+starty,0);
                                                                                GLCD_point(j+(3+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j+(1+startx),c+starty,0);
                                                                                GLCD_point(j+(2+startx),c+starty,0);
                                                                                GLCD_point(j+(3+startx),c+starty,0);
                                                                                GLCD_point(j+(4+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                                delay_us(speed*1000);
                                                        }
                                                        S+=GLCD_movestep;
                                        }
                                        break;
                case 8: //inside to right -> left righ
                                GLCD_Bitmap(bmp,startx,(starty%8),length-1,(height/8)-1);
                                S=length-1;
                                for(i = length-1 ; i>=0 ; i-=GLCD_movestep)
                                        {
                                                for(c=0 ; c<height ; c++)//por kardan Buffer
                                                        {
                                                                #if GLCD_movestep == 1
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                #elif GLCD_movestep == 2
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                #elif GLCD_movestep == 3
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i-2,c);
                                                                #elif GLCD_movestep == 4
                                                                temp0[c] = GLCD_ReadBitmapPixel(bmp,length,i-0,c);
                                                                temp1[c] = GLCD_ReadBitmapPixel(bmp,length,i-1,c);
                                                                temp2[c] = GLCD_ReadBitmapPixel(bmp,length,i-2,c);
                                                                temp3[c] = GLCD_ReadBitmapPixel(bmp,length,i-3,c);
                                                                #endif
                                                        }
                                                j=length-1;
                                                        for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j-(0+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j-(0+startx),c+starty,0);
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j-(0+startx),c+starty,0);
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                GLCD_point(j-(2+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j-(0+startx),c+starty,0);
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                GLCD_point(j-(2+startx),c+starty,0);
                                                                                GLCD_point(j-(3+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                for(j=S ; j<=127 ; j+=GLCD_movestep)
                                                        {
                                                                if(j+(GLCD_movestep) <=127)
                                                                {
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                        #if GLCD_movestep == 1
                                                                        GLCD_point(j+0+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 2
                                                                        GLCD_point(j+0+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 3
                                                                        GLCD_point(j+0+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+2+startx,c+starty,temp0[c]);
                                                                        #elif GLCD_movestep == 4
                                                                        GLCD_point(j+0+startx,c+starty,temp3[c]);
                                                                        GLCD_point(j+1+startx,c+starty,temp2[c]);
                                                                        GLCD_point(j+2+startx,c+starty,temp1[c]);
                                                                        GLCD_point(j+3+startx,c+starty,temp0[c]);
                                                                        #endif
                                                                        }
                                                                }
                                                                for(c=0 ; c<height ; c++)
                                                                        {
                                                                                #if GLCD_movestep == 1
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 2
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                GLCD_point(j-(2+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 3
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                GLCD_point(j-(2+startx),c+starty,0);
                                                                                GLCD_point(j-(3+startx),c+starty,0);
                                                                                #elif GLCD_movestep == 4
                                                                                GLCD_point(j-(1+startx),c+starty,0);
                                                                                GLCD_point(j-(2+startx),c+starty,0);
                                                                                GLCD_point(j-(3+startx),c+starty,0);
                                                                                GLCD_point(j-(4+startx),c+starty,0);
                                                                                #endif
                                                                        }
                                                                delay_us(speed*1000);
                                                        }
                                                        S-=GLCD_movestep;
                                        }
                                        break;
        }
}*/
/**
 * @brief Initialize GLCD
 *
 */
void GLCD_init(void) {
#if BUSY == 0
    DataDDR    = 0xff;
    ControlDDR = 0x1d;
    E          = 1;
#endif
#if (BUSY == 1)
    DataDDR    = 0xff;
    ControlDDR = 0x1f;
    E          = 1;
#endif
    GLCD_on();
    GLCD_clear();
    GLCD_gotoxy(0, 0);
}
