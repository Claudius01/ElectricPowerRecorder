// $Id: GestionLCD.h,v 1.19 2025/05/05 16:31:25 administrateur Exp $

#ifndef __GESTION_LCD__
#define __GESTION_LCD__

#include <stdint.h>
#include <stdio.h>

#if !USE_SIMULATION
#include <pgmspace.h>
#endif

#include <stdlib.h>
#include <string.h>		//memset()
#include <math.h>

#if USE_SIMULATION
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

#include "fonts.h"

#if !USE_SIMULATION
#include <SPI.h>
#endif

#define LIGHTS_POSITION_X       45

// Positions des feux @ RTC
#define LIGHTS_POSITION_RTC               LIGHTS_POSITION_X
#define LIGHTS_POSITION_RTC_GREEN_X       LIGHTS_POSITION_RTC
#define LIGHTS_POSITION_RTC_YELLOW_X      (LIGHTS_POSITION_RTC + 12)
#define LIGHTS_POSITION_RTC_RED_X         (LIGHTS_POSITION_RTC + 24)

// Positions des feux @ SDCard
#define LIGHTS_POSITION_SDC               (LIGHTS_POSITION_X + 78)
#define LIGHTS_POSITION_SDC_GREEN_X       LIGHTS_POSITION_SDC
#define LIGHTS_POSITION_SDC_YELLOW_X      (LIGHTS_POSITION_SDC + 12)
#define LIGHTS_POSITION_SDC_RED_X         (LIGHTS_POSITION_SDC + 24)
#define LIGHTS_POSITION_SDC_BLUE_X        (LIGHTS_POSITION_SDC + 36)

#define LIGHTS_POSITION_Y       115

#define LIGHT_FULL_IDX          14
#define LIGHT_BORD_IDX          15

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// Code tire de 'DEV_Config.h'
/**
 * GPIO config
**/
#define DEV_RST_PIN 9
#define DEV_DC_PIN  8
#define DEV_CS_PIN  10
#define DEV_BL_PIN  7

/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) digitalWrite(_pin, _value == 0? LOW:HIGH)
#define DEV_Digital_Read(_pin) digitalRead(_pin)

/**
 * SPI
**/
#define DEV_SPI_WRITE(_dat)   SPI.transfer(_dat)

/**
 * delay x ms
**/
#define DEV_Delay_ms(__xms)    delay(__xms)

/**
 * PWM_BL
**/
 #define  DEV_Set_BL(_Pin, _Value)  analogWrite(_Pin, _Value)

class Config {
	private:
		void GPIO_Init();

	public:
		Config();
		~Config();

		void Config_Init();
};
// Fin: Code tire de 'DEV_Config.h'

// Code tire de 'LCD_Driver.h'
#define LCD_WIDTH   135     // LCD width
#define LCD_HEIGHT  240     // LCD height

#define LCD_X_SIZE_MAX    (max(LCD_WIDTH, LCD_HEIGHT))
#define LCD_Y_SIZE_MAX    (max(LCD_WIDTH, LCD_HEIGHT))

class LCD
{
	private:

	protected:
		LCD();
		~LCD();

		void LCD_Reset(void);

		void LCD_WriteData_Byte(UBYTE da); 
		void LCD_WriteData_Word(UWORD da);
		void LCD_WriteReg(UBYTE da);

		void LCD_SetCursor(UWORD x1, UWORD y1, UWORD x2,UWORD y2);
		void LCD_SetUWORD(UWORD x, UWORD y, UWORD Color);

		void LCD_Init(void);
		void LCD_SetBacklight(UWORD Value);
		void LCD_Clear(UWORD Color);
		void LCD_ClearWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD UWORD);

		void LCD_SetWindowColor(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD  Color);
};
// Fin: Code tire de 'LCD_Driver.h'

// Code tire de 'GUI_Paint.h'
/**
 * Image attributes
**/
typedef struct {
    UBYTE *Image;
    UWORD Width;
    UWORD Height;
    UWORD WidthMemory;
    UWORD HeightMemory;
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
} PAINT;

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;

#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/
#define RGB_RED(a)	 (((a & RED) >> 11) & 0x1F)		// Codage sur 5 bits
#define RGB_GREEN(a)  (((a & GREEN) >> 5) & 0x3F)		// Codage sur 6 bits
#define RGB_BLUE(a)	 ((a & BLUE) & 0x1F)					// Codage sur 5 bits

#define RGB_SCALE_RED	 (31.0f)			// Mise a l'echelle pour X11 (31 -> 1.0)
#define RGB_SCALE_GREEN	 (63.0f)			// Mise a l'echelle pour X11 (63 -> 1.0)
#define RGB_SCALE_BLUE	 (31.0f)			// Mise a l'echelle pour X11 (31 -> 1.0)

// 10 couleurs fondamentales
#define WHITE         0xFFFF
#define BLACK         0x0000    
#define RED           0xF800			// 1111 1000 0000 0000
#define BLUE          0x001F  		// 0000 0000 0001 1111
#define GREEN         0x07E0			// 0000 0111 1110 0000
#define MAGENTA       0xF81F
#define CYAN          0x7FFF
#define YELLOW        0xFFE0
#define BROWN         0XBC40 
#define GRAY          0X8430 

#define DARKBLUE      0X01CF  

#define BRED          0XF81F
#define BRRED         0XFC07 

#define GRED          0XFFE0
#define GBLUE         0X07FF
#define GRAYBLUE      0X5458 

#define LIGHTBLUE     0X7D7C   
#define LGRAYBLUE     0X841F 
#define LGRAY         0XC618 
#define LIGHTGREEN    0XA651
#define LBBLUE        0X2B12 

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

#define TRANSPARENCY  0x0001

/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,   // 1 x 1
    DOT_PIXEL_2X2  ,    // 2 X 2
    DOT_PIXEL_3X3  ,    // 3 X 3
    DOT_PIXEL_4X4  ,    // 4 X 4
    DOT_PIXEL_5X5  ,    // 5 X 5
    DOT_PIXEL_6X6  ,    // 6 X 6
    DOT_PIXEL_7X7  ,    // 7 X 7
    DOT_PIXEL_8X8  ,    // 8 X 8
} DOT_PIXEL;

#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,   // dot pixel 1 x 1
    DOT_FILL_RIGHTUP  ,     // dot pixel 2 X 2
} DOT_STYLE;

#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
**/
typedef struct {
    UWORD Year;	// 0000
    UBYTE  Month; // 1 - 12
    UBYTE  Day;   // 1 - 30
    UBYTE  Hour;  // 0 - 23
    UBYTE  Min;   // 0 - 59
    UBYTE  Sec;   // 0 - 59
} PAINT_TIME;

/* Definitions pour l'ecran virtuel
 */
#define SCREEN_VIRTUAL_SCALE_PIXELS   100          // 100 pixels entre les echelles

#define SCREEN_VIRTUAL_TOP_X          1
#define SCREEN_VIRTUAL_TOP_Y          64
#define SCREEN_VIRTUAL_BOTTOM_X      (255 + 1)     // Multiple de 8 + Place pour ecrire une fonte 16x11 et la decaler a gauche 
#define SCREEN_VIRTUAL_BOTTOM_Y      109           // Derniere ligne de la fonte 8x5

/* Definitions pour le decalage avec un marquage des echelles tous les 100 pixels (cf. 'SCREEN_VIRTUAL_SCALE_PIXELS')
   - 100 * SCREEN_VIRTUAL_PERIOD_SHIFT = SCREEN_VIRTUAL_PERIOD
 */
//#define SCREEN_VIRTUAL_PERIOD        60L                                                   // Echelle de 1'
//#define SCREEN_VIRTUAL_PERIOD        300L                                                   // Echelle de 5'
#define SCREEN_VIRTUAL_PERIOD        900L                                                   // Echelle de 15'
//#define SCREEN_VIRTUAL_PERIOD        3600L                                                  // Echelle de 1H
//#define SCREEN_VIRTUAL_PERIOD        (6 * 3600L)                                            // Echelle de 6H

#define SCREEN_VIRTUAL_PERIOD_SHIFT  (SCREEN_VIRTUAL_PERIOD / SCREEN_VIRTUAL_SCALE_PIXELS)  // Decalage toutes les 3 secondes
// Fin: Definitions pour l'ecran virtuel

class Paint : public LCD {
	private:
		volatile PAINT    m__paint;     // TBC: 'volatile'

    UWORD             m__cache_color[LCD_X_SIZE_MAX][LCD_Y_SIZE_MAX];    // m__cache_color[x-1][y-1] after rotation
    UWORD             m__screen_virtual[SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X][SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y];

	public:
		Paint();
		~Paint();

		// Init and Clear
		void Paint_SelectImage(UBYTE *image);
		void Paint_SetRotate(UWORD Rotate);
		void Paint_SetMirroring(UBYTE mirror);
		void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color, bool i__flg_screen_virtual = false);

		void Paint_Clear(UWORD Color);
		void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color);

		// Drawing
		void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay, bool i__flg_screen_virtual = false);
		void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style, bool i__flg_screen_virtual = false);
		void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Filled );
		void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill );

		// Display string
		void Paint_DrawChar(UWORD Xstart, UWORD Ystart, const char Acsii_Char, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground, bool i__flg_screen_virtual = false);
		void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground, bool i__flg_screen_virtual = false);
		void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground);
		void Paint_DrawFloatNum(UWORD Xpoint, UWORD Ypoint, double Nummber,  UBYTE Decimal_Point, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground);
		void Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground);

		// Pic
		void Paint_NewImage(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
		void Paint_DrawImage(const unsigned char *image, UWORD Startx, UWORD Starty, UWORD Endx, UWORD Endy);

		void Paint_DrawSymbol(UWORD Xpoint, UWORD Ypoint, const char Num_Symbol, sFONT* Font, UWORD Color_Background, UWORD Color_Foreground, bool i__flg_screen_virtual = false);

    // Analyse du cache
    bool Paint_GetCache_InUse(UWORD i__x, UWORD i__y) { return (m__cache_color[i__x][i__y] != BLACK ? true : false); };
    UWORD Paint_GetCache_Color(UWORD i__x, UWORD i__y) { return m__cache_color[i__x][i__y]; };

    void Paint_DrawBarGraph(UWORD i__y, sFONT* Font, UWORD Color_Foreground, bool i__flg_screen_virtual = false);
    void Paint_DrawBarGraph(UWORD i__y, int i__value, int i__value_max, int i__index, sFONT* Font, UWORD Color_Foreground);

    // Gestion de l'ecran virtuel
    bool Paint_GetScreenVirtual_InUse(UWORD i__x, UWORD i__y) { return (m__screen_virtual[i__x][i__y] != BLACK ? true : false); };
    UWORD Paint_GetScreenVirtual_Color(UWORD i__x, UWORD i__y) { return m__screen_virtual[i__x][i__y]; };
    void Paint_ShiftAndRefreshScreenVirtual(bool i__force_shift = false);
    void Paint_ClearScreenVirtual();
    void Paint_UpdateLcdFromScreenVirtual(bool i__force_updating = false);

    void Paint_Presentation_ValueInCurves(const char *i__label, unsigned int i__nbr_samples, float i__value, int *io__position, UWORD Color_Foreground);
};
// Fin: Code tire de 'GUI_Paint.h'

class GestionLCD : public Paint
{
	private:
		class Config m__config;

	public:
		GestionLCD();
		~GestionLCD();

		void init(UWORD i__val_back_light) {
			m__config.Config_Init();

			LCD_Init();
			LCD_SetBacklight(i__val_back_light);
		};

		void clear(UWORD i__color) {
			LCD_Clear(i__color);
		};
};

extern GestionLCD   *g__gestion_lcd;
#endif

