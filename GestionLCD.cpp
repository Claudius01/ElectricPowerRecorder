// $Id: GestionLCD.cpp,v 1.50 2025/06/02 14:42:46 administrateur Exp $

#if USE_SIMULATION
#include "ArduinoTypes.h"
#include "Serial.h"

#include "PixelToaster.h"
#include "AppPixelToaster.h"
#endif

#include "DateTime.h"
#include "Menus.h"
#include "GestionLCD.h"
#include "AnalogRead.h"

#define USE_PAINT_LINE    1

#if USE_SIMULATION
using namespace PixelToaster;

bool g__dump_cache_f2 = false;
bool g__dump_cache_f3 = false;
#endif

// Code tire de 'DEV_Config.cpp'
Config::Config()
{
	Serial.printf("Config::Config()\n");
}

Config::~Config()
{
	Serial.printf("Config::~Config()\n");
}

void Config::GPIO_Init()
{
	Serial.printf("Config::GPIO_Init()\n");

#if !USE_SIMULATION
	pinMode(DEV_CS_PIN, OUTPUT);
	pinMode(DEV_RST_PIN, OUTPUT);
	pinMode(DEV_DC_PIN, OUTPUT);
	pinMode(DEV_BL_PIN, OUTPUT);

	analogWrite(DEV_BL_PIN, 140);
#endif
}

void Config::Config_Init()
{
	Serial.printf("Config::Config_Init()\n");

	GPIO_Init();

#if !USE_SIMULATION
	// spi
	SPI.setDataMode(SPI_MODE3);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2);
	SPI.begin();
#endif
}
// Fin: Code tire de 'DEV_Config.cpp'

// Code tire de 'LCD_Driver.cpp'
/*******************************************************************************
function:
  Hardware reset
*******************************************************************************/
LCD::LCD()
{
	Serial.printf("LCD::LCD()\n");
}

LCD::~LCD()
{
	Serial.printf("LCD::~Config()\n");
}

void LCD::LCD_Reset(void)
{
#if !USE_SIMULATION
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Delay_ms(20);
  DEV_Digital_Write(DEV_RST_PIN,0);
  DEV_Delay_ms(20);
  DEV_Digital_Write(DEV_RST_PIN,1);
  DEV_Delay_ms(20);
#endif
}

/*******************************************************************************
function:
  Setting backlight
parameter :
    value : Range 0~255   Duty cycle is value/255
*******************************************************************************/
void LCD::LCD_SetBacklight(UWORD Value)
{
#if !USE_SIMULATION
  DEV_Set_BL(DEV_BL_PIN, Value);
#endif
}

/*******************************************************************************
function:
    Write register address and data
*******************************************************************************/
void LCD::LCD_WriteData_Byte(UBYTE da) 
{ 
#if !USE_SIMULATION
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,1);
  DEV_SPI_WRITE(da);  
  DEV_Digital_Write(DEV_CS_PIN,1);
#endif
}  

void LCD::LCD_WriteData_Word(UWORD da)
{
#if !USE_SIMULATION
  UBYTE i=(da>>8)&0xff;

  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,1);
  DEV_SPI_WRITE(i);
  DEV_SPI_WRITE(da);
  DEV_Digital_Write(DEV_CS_PIN,1);
#endif
}   

void LCD::LCD_WriteReg(UBYTE da)  
{ 
#if !USE_SIMULATION
  DEV_Digital_Write(DEV_CS_PIN,0);
  DEV_Digital_Write(DEV_DC_PIN,0);
  DEV_SPI_WRITE(da);
  //DEV_Digital_Write(DEV_CS_PIN,1);
#endif
}

/******************************************************************************
function: 
    Common register initialization
******************************************************************************/
void LCD::LCD_Init(void)
{
  LCD_Reset();

  LCD_WriteReg(0x36); 
  LCD_WriteData_Byte(0x70);

  LCD_WriteReg(0x3A); 
  LCD_WriteData_Byte(0x05);

  LCD_WriteReg(0xB2);
  LCD_WriteData_Byte(0x0C);
  LCD_WriteData_Byte(0x0C);
  LCD_WriteData_Byte(0x00);
  LCD_WriteData_Byte(0x33);
  LCD_WriteData_Byte(0x33);

  LCD_WriteReg(0xB7); 
  LCD_WriteData_Byte(0x35);  

  LCD_WriteReg(0xBB);
  LCD_WriteData_Byte(0x19);

  LCD_WriteReg(0xC0);
  LCD_WriteData_Byte(0x2C);

  LCD_WriteReg(0xC2);
  LCD_WriteData_Byte(0x01);

  LCD_WriteReg(0xC3);
  LCD_WriteData_Byte(0x12);   

  LCD_WriteReg(0xC4);
  LCD_WriteData_Byte(0x20);  

  LCD_WriteReg(0xC6); 
  LCD_WriteData_Byte(0x0F);    

  LCD_WriteReg(0xD0); 
  LCD_WriteData_Byte(0xA4);
  LCD_WriteData_Byte(0xA1);

  LCD_WriteReg(0xE0);
  LCD_WriteData_Byte(0xD0);
  LCD_WriteData_Byte(0x04);
  LCD_WriteData_Byte(0x0D);
  LCD_WriteData_Byte(0x11);
  LCD_WriteData_Byte(0x13);
  LCD_WriteData_Byte(0x2B);
  LCD_WriteData_Byte(0x3F);
  LCD_WriteData_Byte(0x54);
  LCD_WriteData_Byte(0x4C);
  LCD_WriteData_Byte(0x18);
  LCD_WriteData_Byte(0x0D);
  LCD_WriteData_Byte(0x0B);
  LCD_WriteData_Byte(0x1F);
  LCD_WriteData_Byte(0x23);

  LCD_WriteReg(0xE1);
  LCD_WriteData_Byte(0xD0);
  LCD_WriteData_Byte(0x04);
  LCD_WriteData_Byte(0x0C);
  LCD_WriteData_Byte(0x11);
  LCD_WriteData_Byte(0x13);
  LCD_WriteData_Byte(0x2C);
  LCD_WriteData_Byte(0x3F);
  LCD_WriteData_Byte(0x44);
  LCD_WriteData_Byte(0x51);
  LCD_WriteData_Byte(0x2F);
  LCD_WriteData_Byte(0x1F);
  LCD_WriteData_Byte(0x1F);
  LCD_WriteData_Byte(0x20);
  LCD_WriteData_Byte(0x23);

  LCD_WriteReg(0x21); 

  LCD_WriteReg(0x11); 

  LCD_WriteReg(0x29);
} 

/******************************************************************************
function: Set the cursor position
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinatesen
******************************************************************************/
void LCD::LCD_SetCursor(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD  Yend)
{ 
  LCD_WriteReg(0x2a);
  LCD_WriteData_Word(Xstart	+52);
  LCD_WriteData_Word(Xend	+52);
  LCD_WriteReg(0x2b);
  LCD_WriteData_Word(Ystart	+40);
  LCD_WriteData_Word(Yend	+40);

  LCD_WriteReg(0x2C);
}

/******************************************************************************
function: Clear screen function, refresh the screen to a certain color
parameter :
    Color :   The color you want to clear all the screen
******************************************************************************/
void LCD::LCD_Clear(UWORD Color)
{
  UWORD i,j;    
  LCD_SetCursor(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

#if !USE_SIMULATION
  DEV_Digital_Write(DEV_DC_PIN,1);
#endif

  for(i = 0; i < LCD_WIDTH; i++){
    for(j = 0; j < LCD_HEIGHT; j++){
      LCD_WriteData_Word((Color>>8)&0xff);
      LCD_WriteData_Word(Color);
    }
   }
}

/******************************************************************************
function: Refresh a certain area to the same color
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinates
    color :   Set the color
******************************************************************************/
void LCD::LCD_ClearWindow(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD color)
{          
  UWORD i,j; 
  LCD_SetCursor(Xstart, Ystart, Xend-1,Yend-1);
  for(i = Ystart; i <= Yend-1; i++){                                
    for(j = Xstart; j <= Xend-1; j++){
      LCD_WriteData_Word(color);
    }
  }                   
}

/******************************************************************************
function: Set the color of an area
parameter :
    Xstart:   Start UWORD x coordinate
    Ystart:   Start UWORD y coordinate
    Xend  :   End UWORD coordinates
    Yend  :   End UWORD coordinates
    Color :   Set the color
******************************************************************************/
void LCD::LCD_SetWindowColor(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,UWORD  Color)
{
  LCD_SetCursor( Xstart,Ystart,Xend,Yend);
  LCD_WriteData_Word(Color);      
}

/******************************************************************************
function: Draw a UWORD
parameter :
    X     :   Set the X coordinate
    Y     :   Set the Y coordinate
    Color :   Set the color
******************************************************************************/
void LCD::LCD_SetUWORD(UWORD x, UWORD y, UWORD Color)
{
  LCD_SetCursor(x,y,x,y);
  LCD_WriteData_Word(Color);      

#if USE_SIMULATION
  unsigned char l__red   = RGB_RED(Color);
  unsigned char l__green = RGB_GREEN(Color);
  unsigned char l__blue  = RGB_BLUE(Color);

  g__apppixeltoaster->setPixel(x, y, l__red, l__green, l__blue);
#endif
} 
// Fin: Code tire de 'LCD_Driver.cpp'

// Code tire de 'GUI_Paint.cpp'
Paint::Paint() : m__flg_use_cache(false), m__rotate_in_progress(false)
{
	Serial.printf("Paint::Paint()\n");

  // Initialisation du cache de couleur a 'BLACK'
  Serial.printf("-> Init. cache (        %u x %u) (sizeof %u bytes)\n",
    LCD_X_SIZE_MAX, LCD_Y_SIZE_MAX, (unsigned int)sizeof(m__cache_color));
  Serial.printf("-> Init. cache previous (%u x %u) (sizeof %u bytes)\n",
    LCD_X_SIZE_MAX, LCD_Y_SIZE_MAX, (unsigned int)sizeof(m__cache_color_previous));

  for (UWORD y = 0; y < LCD_Y_SIZE_MAX; y++) {
    for (UWORD x = 0; x < LCD_X_SIZE_MAX; x++) {
      m__cache_color[x][y] = BLACK;

      m__cache_color_previous[x][y] = BLACK;
    }
  }

  resetCacheStatistics();

  // Initialisation de l'ecran virtual a 'BLACK'
  Serial.printf("-> Init. screen virtual (%u x %u) (sizeof %u bytes)\n",
    (SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X), (SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y),
    (unsigned int)sizeof(m__screen_virtual));
  
  for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y); y++) {
    for (UWORD x = 0; x < (SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X); x++) {
      m__screen_virtual[x][y] = BLACK;
    }
  }
}

Paint::~Paint()
{
	Serial.printf("Paint::~GestionLCD()\n");
}

/* Mise en BLACK de l'ensemble de l'ecran
   => Reset des statistiques d'utilisation du cache dans la methode 'refreshFromCache()'
 */
void Paint::clearScreen()
{
  for (UWORD y = 0; y < LCD_Y_SIZE_MAX; y++) {
    for (UWORD x = 0; x < LCD_X_SIZE_MAX; x++) {
      m__cache_color[x][y] = BLACK;
      m__cache_color_previous[x][y] = WHITE;    // Forcage reecriture de tous les pixels a BLACK
    }
  }

  refreshFromCache();
}

void Paint::resetCacheStatistics()
{
  memset(&m__statistics, '\0', sizeof(ST_CACHE_STATISTICS));

  m__statistics.percent_min     = 100.0;
  m__statistics.percent_current = 0.0;
  m__statistics.percent_max     = 0.0;
}

ST_CACHE_STATISTICS Paint::getCacheStatistics() const
{
  return m__statistics;
}

void Paint::refreshFromCache()
{
  setUseCache(false);

  m__statistics.nbr_pixels_in_cache  = 0;
  m__statistics.nbr_pixels_out_cache = 0;
  m__statistics.nbr_pixels_total     = 0;

  for (UWORD y = 1; y < 135; y++) {
    for (UWORD x = 1; x < 240; x++) {
      m__statistics.nbr_pixels_total++;

      if (m__cache_color[x-1][y-1] != m__cache_color_previous[x-1][y-1]) {
        Paint_SetPixel(x, y, m__cache_color[x-1][y-1], false, true);

        m__cache_color_previous[x-1][y-1] = m__cache_color[x-1][y-1];

        m__statistics.nbr_pixels_out_cache++;
      }
      else {
        m__statistics.nbr_pixels_in_cache++;
      }
    }
  }

  m__statistics.percent_current = 100.0 * ((float)m__statistics.nbr_pixels_in_cache / (float)m__statistics.nbr_pixels_total);

  if (m__statistics.percent_current < m__statistics.percent_min) m__statistics.percent_min = m__statistics.percent_current;
  if (m__statistics.percent_current > m__statistics.percent_max) m__statistics.percent_max = m__statistics.percent_current;

  setUseCache(true);
}

/******************************************************************************
  function: Create Image
  parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint::Paint_NewImage(UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
  m__paint.WidthMemory = Width;
  m__paint.HeightMemory = Height;
  m__paint.Color = Color;
  m__paint.WidthByte = Width;
  m__paint.HeightByte = Height;
  
  m__paint.Rotate = Rotate;
  m__paint.Mirror = MIRROR_NONE;

  if (Rotate == ROTATE_0 || Rotate == ROTATE_180) {
    m__paint.Width = Width;
    m__paint.Height = Height;
  } else {
    m__paint.Width = Height;
    m__paint.Height = Width;
  }
}

/******************************************************************************
  function: Select Image Rotate
  parameter:
    Rotate   :   0,90,180,270
******************************************************************************/
void Paint::Paint_SetRotate(UWORD Rotate)
{
  if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270) {
    //Debug("Set image Rotate %d\r\n", Rotate);
    m__paint.Rotate = Rotate;
  } else {
    //Debug("rotate = 0, 90, 180, 270\r\n");
    //  exit(0);
  }
}

/******************************************************************************
  function: Select Image mirror
  parameter:
    mirror   :       Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint::Paint_SetMirroring(UBYTE mirror)
{
  if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
      mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
    //Debug("mirror image x:%s, y:%s\r\n", (mirror & 0x01) ? "mirror" : "none", ((mirror >> 1) & 0x01) ? "mirror" : "none");
    m__paint.Mirror = mirror;
  } else {
    //Debug("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    //exit(0);
  }
}

/******************************************************************************
  function: Draw Pixels
  parameter:
    Xpoint  :   At point X
    Ypoint  :   At point Y
    Color   :   Painted colors
******************************************************************************/
void Paint::Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color, bool i__flg_screen_virtual, bool i__flg_force_lcd)
{
  //Serial.printf("%s(X:[%d] Y:[%d] Color:[0x%04x])\n", __FUNCTION__ , Xpoint, Ypoint, Color);

  if (i__flg_screen_virtual == false) {
    if (Xpoint > m__paint.Width || Ypoint > m__paint.Height) {
      //Debug("Exceeding display boundaries\r\n");
      return;
    }
  }

  if (i__flg_screen_virtual == true) {
    if (((Xpoint - 1) >= SCREEN_VIRTUAL_TOP_X) && ((Xpoint - 1) <= SCREEN_VIRTUAL_BOTTOM_X)
     && ((Ypoint - 1) >= SCREEN_VIRTUAL_TOP_Y) && ((Ypoint - 1) <= SCREEN_VIRTUAL_BOTTOM_Y))
    {
      m__screen_virtual[Xpoint - 1 - SCREEN_VIRTUAL_TOP_X][Ypoint - 1 - SCREEN_VIRTUAL_TOP_Y] = Color;
    }

    return;
  }

  UWORD X, Y;

  switch (m__paint.Rotate) {
    case 0:
      X = Xpoint;
      Y = Ypoint;
      break;
    case 90:
      X = m__paint.WidthMemory - Ypoint - 1;
      Y = Xpoint;
      break;
    case 180:
      X = m__paint.WidthMemory - Xpoint - 1;
      Y = m__paint.HeightMemory - Ypoint - 1;
      break;
    case 270:
      X = Ypoint;
      Y = m__paint.HeightMemory - Xpoint - 1;
      break;

    default:
      return;
  }

  switch (m__paint.Mirror) {
    case MIRROR_NONE:
      break;
    case MIRROR_HORIZONTAL:
      X = m__paint.WidthMemory - X - 1;
      break;
    case MIRROR_VERTICAL:
      Y = m__paint.HeightMemory - Y - 1;
      break;
    case MIRROR_ORIGIN:
      X = m__paint.WidthMemory - X - 1;
      Y = m__paint.HeightMemory - Y - 1;
      break;
    default:
      return;
  }

  /* Retablissement des coordonnees dans le cas d'une rotation de 270 degres
     => Cf. l'organisation du cache ;-)
     => TODO: Implementer les 2 autres cas de rotation: 90 et 180 degres
   */
  if (m__rotate_in_progress == true) {
#if USE_SIMULATION
    // Rotation de 270 degres
    Xpoint = X;
    Ypoint = Y;
#else
    // Rotation de 270 degres
    Xpoint = Y;
    Ypoint = m__paint.WidthMemory - 1 - X;
#endif
  }

  if (m__flg_use_cache == false) {
    // Si le cache n'est pas utilse -> Appel a 'LCD_SetUWORD()'...
    if (i__flg_force_lcd == false) {
      /* Test si le pixel est deja affiche avec la meme couleur
         => Si Oui: Pas d'appel a 'LCD_SetUWORD()'
       */
      if (m__cache_color[Xpoint-1][Ypoint-1] == Color) {
        return;
      }
      m__cache_color[Xpoint-1][Ypoint-1] = Color;   // Maj du cache
    }
  }
  else {
    m__cache_color[Xpoint-1][Ypoint-1] = Color;     // Maj du cache
    return;
  }

  if (X >= m__paint.WidthMemory || Y >= m__paint.HeightMemory) {
    Serial.printf("error Paint::Paint_SetPixel(): %u >= %u || %u >= %u\n", X, m__paint.WidthMemory, Y, m__paint.HeightMemory);
    return;
  }

  // UDOUBLE Addr = X / 8 + Y * m__paint.WidthByte;
  LCD_SetUWORD(X, Y, Color);
}

/******************************************************************************
  function: Clear the color of the picture
  parameter:
    Color   :   Painted colors
******************************************************************************/
void Paint::Paint_Clear(UWORD Color)
{
  LCD_SetCursor(0, 0, m__paint.WidthByte-1 , m__paint.HeightByte-1);
  for (UWORD Y = 0; Y < m__paint.HeightByte; Y++) {
    for (UWORD X = 0; X < m__paint.WidthByte; X++ ) {//8 pixel =  1 byte
      LCD_WriteData_Word(Color);
    }
  }
}

/******************************************************************************
  function: Clear the color of a window
  parameter:
    Xstart :   x starting point
    Ystart :   Y starting point
    Xend   :   x end point
    Yend   :   y end point
******************************************************************************/
void Paint::Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
  UWORD X, Y;
  for (Y = Ystart; Y < Yend; Y++) {
    for (X = Xstart; X < Xend; X++) {//8 pixel =  1 byte
      Paint_SetPixel(X, Y, Color);
    }
  }
}

/******************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		:   The Xpoint coordinate of the point
    Ypoint		:   The Ypoint coordinate of the point
    Color		:   Set color
    Dot_Pixel	:	point size
******************************************************************************/
void Paint::Paint_DrawPoint( UWORD Xpoint, UWORD Ypoint, UWORD Color,
                      DOT_PIXEL Dot_Pixel,DOT_STYLE Dot_FillWay,
                      bool i__flg_screen_virtual, UWORD Color_Background)
{
    if (Xpoint > m__paint.Width || Ypoint > m__paint.Height) {
        //Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    int16_t XDir_Num , YDir_Num;
    if (Dot_FillWay == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2*Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if(Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                if (Color_Background != m__cache_color[Xpoint + XDir_Num - Dot_Pixel - 1][Ypoint + YDir_Num - Dot_Pixel - 1]) {
                  Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color, i__flg_screen_virtual);
                }
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                if (Color_Background != m__cache_color[Xpoint + XDir_Num - Dot_Pixel - 1][Ypoint + YDir_Num - Dot_Pixel - 1]) {
                  Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color, i__flg_screen_virtual);
                }
            }
        }
    }
}

/******************************************************************************
function:	Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
******************************************************************************/
void Paint::Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, 
                    UWORD Color_Foreground, DOT_PIXEL Line_width, LINE_STYLE Line_Style,
                    bool i__flg_screen_virtual, UWORD Color_Background)
{
#if 0
    Serial.printf("Entering in 'Paint_DrawLine(%u, %u, %u, %u, 0x%04x, %u, %u, %u, 0x%04x)\n",
    	Xstart, Ystart, Xend, Yend, Color_Foreground, Line_width, Line_Style, i__flg_screen_virtual, Color_Background);
#endif

    if (Xstart > m__paint.Width || Ystart > m__paint.Height ||
        Xend > m__paint.Width || Yend > m__paint.Height) {
        Serial.printf("Paint_DrawLine Input exceeds the normal display range\n");
        Serial.printf("%u > %u || %u > %u\n", Xstart, m__paint.Width, Ystart, m__paint.Height);
        Serial.printf("%u > %u || %u > %u\n", Xend, m__paint.Width, Yend, m__paint.Height);
        return;
    }

    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT, i__flg_screen_virtual, Color_Background);
            Dotted_Len = 0;
        } else {
            Paint_DrawPoint(Xpoint, Ypoint, Color_Foreground, Line_width, DOT_STYLE_DFT, i__flg_screen_virtual, Color_Background);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
function:	Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Filled : Whether it is filled--- 1 solid 0：empty
******************************************************************************/
void Paint::Paint_DrawRectangle( UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, 
                          UWORD Color_Foreground, DOT_PIXEL Line_width, DRAW_FILL Filled, UWORD Color_Background,
                          bool i__flg_screen_virtual)
{
#if 0
    Serial.printf("Entering in 'Paint_DrawRectangle(%u, %u, %u, %u, 0x%04x, %u, %u, 0x%04x)\n",
    	Xstart, Ystart, Xend, Yend, Color_Foreground, Line_width, Filled, Color_Background);
#endif

    if (Xstart > m__paint.Width || Ystart > m__paint.Height ||
        Xend > m__paint.Width || Yend > m__paint.Height) {
        Serial.printf("error Paint::Paint_DrawRectangle(): Input exceeds the normal display range\n");
        Serial.printf("error    [%d > %d || %d > %d]\n", Xstart, m__paint.Width, Ystart, m__paint.Height);
        Serial.printf("error || [%d > %d || %d > %d]\n", Xend, m__paint.Width, Yend, m__paint.Height);
        return;
    }

    if (Filled ) {
        UWORD Ypoint;
        for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color_Foreground, Line_width, LINE_STYLE_SOLID, i__flg_screen_virtual, Color_Background);
        }
    } else {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color_Foreground, Line_width, LINE_STYLE_SOLID, i__flg_screen_virtual, Color_Background);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color_Foreground, Line_width, LINE_STYLE_SOLID, i__flg_screen_virtual, Color_Background);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color_Foreground, Line_width, LINE_STYLE_SOLID, i__flg_screen_virtual, Color_Background);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color_Foreground, Line_width, LINE_STYLE_SOLID, i__flg_screen_virtual, Color_Background);
    }
}

/******************************************************************************
function:	Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void Paint::Paint_DrawCircle(  UWORD X_Center, UWORD Y_Center, UWORD Radius, 
                        UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill, UWORD Color_Background)
{
    if (X_Center > m__paint.Width || Y_Center >= m__paint.Height) {
        //Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    //Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1 );

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//0

            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

/******************************************************************************
  function: Show English characters
  parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character

    Remarque: Si 'Color_Background' est egal a 'TRANSPARENCY' -> Pas de maj du pixel ;-)
******************************************************************************/
void Paint::Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT* Font, UWORD Color_Background, UWORD Color_Foreground,
                    bool i__flg_screen_virtual)
{
  //Serial.printf("%s('%c')\n", __FUNCTION__, Acsii_Char);

  UWORD Page, Column;

  if (i__flg_screen_virtual == false) {
    if (Xpoint > m__paint.Width || Ypoint > m__paint.Height) {
      //Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
      return;
    }
  }

  uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
  const unsigned char *ptr = &Font->table[Char_Offset];

  for ( Page = 0; Page < Font->Height; Page ++ ) {
    for ( Column = 0; Column < Font->Width; Column ++ ) {

      //To determine whether the font background color and screen background color is consistent
      if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
#if !USE_SIMULATION
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
#else
        if (*ptr & (0x80 >> (Column % 8)))
#endif
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground, i__flg_screen_virtual);
      } else {
#if !USE_SIMULATION
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
#else
        if (*ptr & (0x80 >> (Column % 8)))
#endif
        {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground, i__flg_screen_virtual);
        }
        else if (TRANSPARENCY != Color_Background) {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Background, i__flg_screen_virtual);
        }
      }
      //One pixel is 8 bits
      if (Column % 8 == 7) {
        ptr++;
      }
    }/* Write a line */
    if (Font->Width % 8 != 0) {
      ptr++;
    }
  }/* Write all */
}

/******************************************************************************
  function: Show Symbol characters
  parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character

    Remarque: Si 'Color_Background' est egal a 'TRANSPARENCY' -> Pas de maj du pixel ;-)
******************************************************************************/
void Paint::Paint_DrawSymbol(UWORD Xpoint, UWORD Ypoint, const char Num_Symbol,
                    sFONT* Font, UWORD Color_Background, UWORD Color_Foreground,
                    bool i__flg_screen_virtual)
{
  //Serial.printf("%s(%d, %d, %d)\n", __FUNCTION__, Xpoint, Ypoint,  Num_Symbol);

  UWORD Page, Column;

  // TODO: Faire une methode ;-)
  if (i__flg_screen_virtual == false) {
    if (Xpoint == 0 || Xpoint > m__paint.Width || Ypoint == 0 || Ypoint > m__paint.Height) {
      Serial.printf("error: Paint_DrawSymbol Input out of range X:[%d/%d] Y:[%d/%d]\n",
        Xpoint, m__paint.Width, Ypoint, m__paint.Height);

#if USE_SIMULATION
      exit(1);
#else
      return;	// Operation ignoree
#endif
    }
  }

  uint32_t Char_Offset = Num_Symbol * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
  const unsigned char *ptr = &Font->table[Char_Offset];

  for ( Page = 0; Page < Font->Height; Page ++ ) {
    for ( Column = 0; Column < Font->Width; Column ++ ) {

      //To determine whether the font background color and screen background color is consistent
      if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
#if !USE_SIMULATION
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
#else
        if (*ptr & (0x80 >> (Column % 8)))
#endif
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground, i__flg_screen_virtual);
      } else {
#if !USE_SIMULATION
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
#else
        if (*ptr & (0x80 >> (Column % 8)))
#endif
        {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground, i__flg_screen_virtual);
        }
        else if (TRANSPARENCY != Color_Background) {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Background, i__flg_screen_virtual);
        }
      }
      //One pixel is 8 bits
      if (Column % 8 == 7) {
        ptr++;
      }
    }/* Write a line */
    if (Font->Width % 8 != 0) {
      ptr++;
    }
  }/* Write all */
}

/******************************************************************************
  function: Display the string
  parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void Paint::Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString,
                         sFONT* Font, UWORD Color_Background, UWORD Color_Foreground,
                         bool i__flg_screen_virtual)
{
  //Serial.printf("%s(\"%s\")\n", __FUNCTION__, pString);

  UWORD Xpoint = Xstart;
  UWORD Ypoint = Ystart;

  if (i__flg_screen_virtual == false) {
    if (Xstart > m__paint.Width || Ystart > m__paint.Height) {
      Serial.printf("Paint_DrawString_EN Input exceeds the normal display range\n");
      Serial.printf("Xstart [%d] > [%d] or Ystart[%d] > [%d]\n",
		    Xstart, m__paint.Width, Ystart, m__paint.Height);

      return;
    }
  }

  while (* pString != '\0') {
    //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
    if ((Xpoint + Font->Width ) > m__paint.Width + 100) {
      Xpoint = Xstart;
      Ypoint += Font->Height;
    }

    // If the Y direction is full, reposition to(Xstart, Ystart)
    if ((Ypoint  + Font->Height ) > m__paint.Height + 100) {
      Xpoint = Xstart;
      Ypoint = Ystart;
    }
    Paint_DrawChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground, i__flg_screen_virtual);

    //The next character of the address
    pString ++;

    //The next word of the abscissa increases the font of the broadband
    Xpoint += Font->Width;
  }
}

/******************************************************************************
  function: Display nummber
  parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Background : Select the background color of the English character
    Color_Foreground : Select the foreground color of the English character
******************************************************************************/
#define  ARRAY_LEN 50
void Paint::Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber,
                   sFONT* Font, UWORD Color_Background, UWORD Color_Foreground )
{

  int16_t Num_Bit = 0, Str_Bit = 0;
  uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
  uint8_t *pStr = Str_Array;

  if (Xpoint > m__paint.Width || Ypoint > m__paint.Height) {
    //Debug("Paint_DisNum Input exceeds the normal display range\r\n");
    return;
  }

  //Converts a number to a string
  do{
    Num_Array[Num_Bit] = Nummber % 10 + '0';
    Num_Bit++;
    Nummber /= 10;
  }while (Nummber);

  //The string is inverted
  while (Num_Bit > 0) {
    Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
    Str_Bit ++;
    Num_Bit --;
  }

  //show
  Paint_DrawString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display float number
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The float data that you want to display
	Decimal_Point	 : Show decimal places
    Font             ：A structure pointer that displays a character size
    Color            : Select the background color of the English character
******************************************************************************/
void Paint::Paint_DrawFloatNum(UWORD Xpoint, UWORD Ypoint, double Nummber,  UBYTE Decimal_Point, 
                        sFONT* Font,  UWORD Color_Background, UWORD Color_Foreground)
{
  char Str[ARRAY_LEN] = {0};

#if !USE_SIMULATION
  dtostrf(Nummber,0,Decimal_Point+2,Str);
#else
  printf("Paint::Paint_DrawFloatNum(): dtostrf not implemented\n");
  exit(1);
#endif

  char * pStr= (char *)malloc((strlen(Str))*sizeof(char));
  memcpy(pStr,Str,(strlen(Str)-2));
  * (pStr+strlen(Str)-1)='\0';
  if((*(pStr+strlen(Str)-3))=='.')
  {
	*(pStr+strlen(Str)-3)='\0';
  }
  //show
  Paint_DrawString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Foreground, Color_Background);
  free(pStr);
  pStr=NULL;
}

/******************************************************************************
  function: Display time
  parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color            : Select the background color of the English character
******************************************************************************/
void Paint::Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,
                    UWORD Color_Background, UWORD Color_Foreground)
{
  uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  UWORD Dx = Font->Width;

  //Write data into the cache
  Paint_DrawChar(Xstart                           , Ystart, value[pTime->Hour / 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx                      , Ystart, value[pTime->Hour % 10], Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx  + Dx / 4 + Dx / 2   , Ystart, ':'                    , Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 2 + Dx / 2         , Ystart, value[pTime->Min / 10] , Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 3 + Dx / 2         , Ystart, value[pTime->Min % 10] , Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':'                    , Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 5                  , Ystart, value[pTime->Sec / 10] , Font, Color_Background, Color_Foreground);
  Paint_DrawChar(Xstart + Dx * 6                  , Ystart, value[pTime->Sec % 10] , Font, Color_Background, Color_Foreground);
}

/******************************************************************************
  function: Display image
  parameter:
    image            ：Image start address
    xStart           : X starting coordinates
    yStart           : Y starting coordinates
    xEnd             ：Image width
    yEnd             : Image height
******************************************************************************/
void Paint::Paint_DrawImage(const unsigned char *image, UWORD xStart, UWORD yStart, UWORD W_Image, UWORD H_Image)
{
  int i, j;
  for (j = 0; j < H_Image; j++) {
    for (i = 0; i < W_Image; i++) {
      if (xStart + i < LCD_WIDTH  &&  yStart + j < LCD_HEIGHT) //Exceeded part does not display
#if !USE_SIMULATION

        Paint_SetPixel(xStart + i, yStart + j, (pgm_read_byte(image + j * W_Image * 2 + i * 2 + 1)) << 8 | (pgm_read_byte(image + j * W_Image * 2 + i * 2)));
#else
        Paint_SetPixel(xStart + i, yStart + j, (*image + j * W_Image * 2 + i * 2 + 1) << 8 | (*image + j * W_Image * 2 + i * 2));
#endif

      //Using arrays is a property of sequential storage, accessing the original array by algorithm
      //j*W_Image*2          Y offset
      //i*2                  X offset
      //pgm_read_byte()
    }
  }
}

/* Dessin d'un Bargraph...
   - Utilisation de la fonte passee en argument
   - Couleur de fond: 'GRAY'
 */
void Paint::Paint_DrawBarGraph(UWORD i__y, sFONT* Font, UWORD Color_Foreground, bool i__flg_screen_virtual)
{
  int l__nbr_block_x_full   = (LCD_X_SIZE_MAX / Font->Width);
  int l__nbr_block_x_residu = (LCD_X_SIZE_MAX % Font->Width);

  // Fond avec 'GRAY' sur toute la largeur de l'ecran
  int x = 0;
  for (x = 0; x < l__nbr_block_x_full; x++) {
    // 1st line: A 1 pixel du bord gauche
    Paint_DrawSymbol(1 + (x * Font->Width), i__y, 0, Font, BLACK, Color_Foreground, i__flg_screen_virtual);
  }

  /* 1 ligne  verticale  -> #1 de la font 'font16Symbols'
     2 lignes verticales -> #2  de la font 'font16Symbols'
       ...
     7 lignes verticales -> #7  de la font 'font16Symbols'
       ...
  */
  // Last line: A 2 pixels du bord droit du pave #(l__nbr_block_x_residu - 2)
  Paint_DrawSymbol(1 + (x * Font->Width), i__y, (l__nbr_block_x_residu - 2), Font, BLACK, Color_Foreground, i__flg_screen_virtual);
}

void Paint::Paint_DrawBarGraph(UWORD i__y, int i__value, int i__value_max, int i__index, sFONT* Font, UWORD Color_Foreground)
{
  /* Calcul de la position dans le bargraph
     => Position minimale a gauche: 1
     => Position maximale a droite: ('LCD_X_SIZE_MAX' - 'Font->Width' - 3)
   */
  if (i__value < 0) {
    i__value = 0;
  }
  else if (i__value > i__value_max) {
    i__value = i__value_max;
  }

  UWORD l__x = (int)(((float)i__value * (float)(LCD_X_SIZE_MAX - Font->Width - 3)) / (float)i__value_max);
  Paint_DrawSymbol(l__x + 1, i__y, i__index, Font, TRANSPARENCY, Color_Foreground);
}
// Fin: Code tire de 'GUI_Paint.cpp'

void Paint::Paint_DrawPeriodAndUnit()
{
  // Preparation de l'accueil de la presentation
  Paint_DrawSymbol(1, 64, 0, &Font16Symbols, BLACK, BLACK);
  Paint_DrawSymbol(1, 81, 0, &Font16Symbols, BLACK, BLACK);
  Paint_DrawSymbol(1 + Font16Symbols.Width, 64, 0, &Font16Symbols, BLACK, BLACK);
  Paint_DrawSymbol(1 + Font16Symbols.Width, 81, 0, &Font16Symbols, BLACK, BLACK);
  Paint_DrawSymbol(1 + 2 * Font16Symbols.Width, 64, 0, &Font16Symbols, BLACK, BLACK);
  Paint_DrawSymbol(1 + 2 * Font16Symbols.Width, 81, 0, &Font16Symbols, BLACK, BLACK);

  // Affichage en vertical de la periode
  const char *l__text_period = g__menu_periods[g__menus->getSubMenuPeriodCurrent()][MENU_TYPE_SHORT];
  size_t l__len_period = strlen(l__text_period);
  uint16_t l__x_period = 39;
  if (l__len_period == 2) {
    l__x_period = (l__x_period + Font16.Height / 2 - 4);
  } 

#if USE_SIMULATION
  g__gestion_lcd->setRotateInProgress(true);
  g__gestion_lcd->Paint_NewImage(LCD_HEIGHT, LCD_WIDTH, 270, WHITE);
  g__gestion_lcd->Paint_DrawString_EN(l__x_period, 4, l__text_period, &Font16, BLACK, WHITE);
  g__gestion_lcd->Paint_NewImage(LCD_HEIGHT, LCD_WIDTH, 0, WHITE);
  g__gestion_lcd->setRotateInProgress(false);
#else
  g__gestion_lcd->setRotateInProgress(true);
  g__gestion_lcd->Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
  g__gestion_lcd->Paint_DrawString_EN(l__x_period, 4, l__text_period, &Font16, BLACK, WHITE);
  g__gestion_lcd->Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
  g__gestion_lcd->setRotateInProgress(false);
#endif

  // Affichage en vertical de l'unite'
  const char *l__text_unit = g__menu_units[g__menus->getSubMenuUnitCurrent()][MENU_TYPE_SHORT];
  size_t l__len_unit = strlen(l__text_unit);
  uint16_t l__x_unit = 39;
  if (l__len_unit == 1) {
    l__x_unit = (l__x_unit + Font16.Height / 2 + 2);
  } 
  else if (l__len_unit == 2) {
    l__x_unit = (l__x_unit + Font16.Height / 2 - 4);
  }

#if USE_SIMULATION
  g__gestion_lcd->setRotateInProgress(true);
  g__gestion_lcd->Paint_NewImage(LCD_HEIGHT, LCD_WIDTH, 270, WHITE);
  g__gestion_lcd->Paint_DrawString_EN(l__x_unit, 7 + Font16.Width, l__text_unit, &Font16, BLACK, WHITE);
  g__gestion_lcd->Paint_NewImage(LCD_HEIGHT, LCD_WIDTH, 0, WHITE);
  g__gestion_lcd->setRotateInProgress(false);
#else
  g__gestion_lcd->setRotateInProgress(true);
  g__gestion_lcd->Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
  g__gestion_lcd->Paint_DrawString_EN(l__x_unit, 7 + Font16.Width, l__text_unit, &Font16, BLACK, WHITE);
  g__gestion_lcd->Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
  g__gestion_lcd->setRotateInProgress(false);
#endif
}

// Decalage a gauche des pixels de l'ecran virtuel et rafraichissement de celui-ci
void Paint::Paint_ShiftAndRefreshScreenVirtual(bool i__force_shift, bool i__flg_partial)
{
  static int g__period_for_shift = (SCREEN_VIRTUAL_PERIOD / SCREEN_VIRTUAL_SCALE_PIXELS);

  if (i__force_shift == true || g__period_for_shift == 0) {
    for (UWORD x = 1; x < (SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X); x++) {

      if (i__flg_partial == true) {
        for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y_PARTIAL - SCREEN_VIRTUAL_TOP_Y); y++) {
          m__screen_virtual[x-1][y] = m__screen_virtual[x][y];
        }
      }
      else {
        for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y); y++) {
          m__screen_virtual[x-1][y] = m__screen_virtual[x][y];
        }
      }
    }

#if 0
    if (i__force_shift == true) {
      Serial.printf("Paint::Paint_ShiftScreenVirtual(): Force shift...\n");
    }
#endif

    // Refresh the screen virtual to LCD 
    for (UWORD x = 0; x < (SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X) && x < LCD_HEIGHT; x++) {
      if (i__flg_partial == true) {
        for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y_PARTIAL - SCREEN_VIRTUAL_TOP_Y); y++) {
          Paint_SetPixel(min(x + SCREEN_VIRTUAL_TOP_X, LCD_HEIGHT), min(y + SCREEN_VIRTUAL_TOP_Y, LCD_WIDTH), m__screen_virtual[x][y]);
        }
      }
      else {
        for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y); y++) {
          Paint_SetPixel(min(x + SCREEN_VIRTUAL_TOP_X, LCD_HEIGHT), min(y + SCREEN_VIRTUAL_TOP_Y, LCD_WIDTH), m__screen_virtual[x][y]);
        }
      }
    }

    g__period_for_shift = (SCREEN_VIRTUAL_PERIOD / SCREEN_VIRTUAL_SCALE_PIXELS);
  }
  else {
    g__period_for_shift--;
  }

  // Affichage de la periode et de l'unite
  Paint_DrawPeriodAndUnit();
}

void Paint::Paint_ClearScreenVirtual()
{
  Serial.printf("Paint::Paint_ClearScreenVirtual\n");

  for (UWORD y = 0; y < (SCREEN_VIRTUAL_BOTTOM_Y - SCREEN_VIRTUAL_TOP_Y); y++) {
    for (UWORD x = 0; x < (SCREEN_VIRTUAL_BOTTOM_X - SCREEN_VIRTUAL_TOP_X); x++) {
      m__screen_virtual[x][y] = BLACK;
    }
  }

  g__gestion_lcd->Paint_DrawSymbol(230, 65, 1, &Font16Symbols, BLACK, GRAY, true);
  g__gestion_lcd->Paint_DrawSymbol(230, 65 + 16 + 1, 1, &Font16Symbols, BLACK, GRAY, true);
  g__gestion_lcd->Paint_DrawSymbol(230, 65 + 32, 1, &Font16Symbols, BLACK, BLACK, true);
}

void Paint::Paint_UpdateLcdFromScreenVirtual(bool i__force_updating)
{
  //Serial.printf("Paint::Paint_UpdateLcdFromScreenVirtual(%d)\n", i__force_updating);

  /* Update from screen virtual
     - Affichage des valeurs sous la forme de courbes horodatees
  */
  ENUM_IN_THE_PERIOD l__enum_period = g__date_time->isRtcSecInDayInRange();

  if ((l__enum_period == ENUM_PERIOD_NONE || l__enum_period == ENUM_MI_PERIOD_DONE) && i__force_updating == false) {
    Paint_DrawSymbol(230, 65, 1, &Font16Symbols, BLACK, GRAY, true);
    Paint_DrawSymbol(230, 65 + 16 + 1, 1, &Font16Symbols, BLACK, GRAY, true);

    /* Affichage des courbes...
       - Effacement de l'affichage precedent...
       - Utilisation du seul symbol #16 de la fonte 16x11 correctement positionne @ a la valeur
         => 33 positions verticales correspondant a la valeur analogique [0...3.3V]
         => 65 pour la ligne la plus haute             
     */

    /* Calcul des nouvelles valeurs des courbes avant presentation
       => Remise a zero des nombres d'echantillons car en debut de la periode
     */
    g__analog_read_1->calculOfValuesCurves(false);

    // Prise des valeurs pour presentation
    ST_ANALOG_VALUES_CURVES l__analog_values_curves;
    memset(&l__analog_values_curves, '\0', sizeof(ST_ANALOG_VALUES_CURVES));
    g__analog_read_1->getValuesCurves(&l__analog_values_curves);

    // Presentation des valeurs moyennes des min, moyenne et max
    Paint_Presentation_ValueInCurves("Min",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_min.value, &l__analog_values_curves.analogVolts_min.position, WHITE);
    Paint_Presentation_ValueInCurves("Max",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_max.value, &l__analog_values_curves.analogVolts_max.position, RED);
    Paint_Presentation_ValueInCurves("Avg",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_avg.value, &l__analog_values_curves.analogVolts_avg.position, GREEN);
    Paint_Presentation_ValueInCurves("Current", l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts.value,     &l__analog_values_curves.analogVolts.position,     YELLOW);

    if (l__enum_period == ENUM_MI_PERIOD_DONE) {
      Paint_DrawLine(231, 65, 231, 98, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);

      // Affichage du bargraph de progression des heures @ 24H dans l'ecran virtuel
      Paint_UpdateBargraph24H();

      Paint_ShiftAndRefreshScreenVirtual(true);
    }
    else {
      Paint_ShiftAndRefreshScreenVirtual();
    }
  }
  else {
    //Serial.printf("Paint::Paint_UpdateLcdFromScreenVirtual(%d): In the range\n", i__force_updating);

    // Horodatage de l'echelle
    char l__text[32];
    sprintf(l__text, "  %02uH%02u",   // Warning: 2 blancs avant "HHhMM"
      (unsigned int)(g__date_time->getRtcSecInDayLocal() / 3600L),
      (unsigned int)((g__date_time->getRtcSecInDayLocal() % 3600L) / 60L));

    Serial.printf("Paint::Paint_UpdateLcdFromScreenVirtual(%d): [%s]\n", i__force_updating, l__text);

    // Effacement eventuel des 2 precedents caracteres (cas d'une periode de 1' sans effacement ;-)
    Paint_DrawString_EN(211 - 2 * (Font12.Width), 101, l__text, &Font12, BLACK, WHITE, true);

    /* Calcul des nouvelles valeurs des courbes avant presentation
       => Pas de remise a zero des nombres d'echantillons car dans la periode
     */
    g__analog_read_1->calculOfValuesCurves(true);

    // Prise des valeurs pour presentation
    ST_ANALOG_VALUES_CURVES l__analog_values_curves;
    memset(&l__analog_values_curves, '\0', sizeof(ST_ANALOG_VALUES_CURVES));
    g__analog_read_1->getValuesCurves(&l__analog_values_curves);

    // Presentation des valeurs moyennes des min, moyenne et max
    Paint_Presentation_ValueInCurves("Min",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_min.value, &l__analog_values_curves.analogVolts_min.position, WHITE);
    Paint_Presentation_ValueInCurves("Max",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_max.value, &l__analog_values_curves.analogVolts_max.position, RED);
    Paint_Presentation_ValueInCurves("Avg",     l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts_avg.value, &l__analog_values_curves.analogVolts_avg.position, GREEN);
    Paint_Presentation_ValueInCurves("Current", l__analog_values_curves.nbr_samples, l__analog_values_curves.analogVolts.value,     &l__analog_values_curves.analogVolts.position,     YELLOW);

    // Barre verticale de l'echelle (demi-periode et periode)
    if (i__force_updating == false) {
#if USE_PAINT_LINE
      Paint_DrawLine(231, 65, 231, 98, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
#else
      Paint_DrawSymbol(230, 65, 0, &Font16Symbols, BLACK, BLACK, true);
      Paint_DrawSymbol(230, 65 + 16 + 1, 0, &Font16Symbols, BLACK, BLACK, true);
#endif
    }
    else {
#if USE_PAINT_LINE
      // Marquage de la reinitialisation de la periode en YELLOW
      g__gestion_lcd->Paint_DrawLine(231, 65, 231, 81, YELLOW, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
      g__gestion_lcd->Paint_DrawLine(231, 83, 231, 98, YELLOW, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
#else
      Paint_DrawSymbol(230, 65, 1, &Font16Symbols, BLACK, YELLOW, true);
      Paint_DrawSymbol(230, 65 + 16 + 1, 1, &Font16Symbols, BLACK, YELLOW, true);
#endif
    }

    // Decalage et raffraichissement de l'ecran virtuel
    Paint_ShiftAndRefreshScreenVirtual(true);
  }
}

void Paint::Paint_Presentation_ValueInCurves(const char *i__label, unsigned int i__nbr_samples, float i__value, int *io__position, UWORD Color_Foreground)
{
  //Serial.printf("Paint::Paint_Presentation_ValueInCurves(\"%s\", #%u, [%0.1f], 0x%x)\n", i__label, i__nbr_samples, i__value, Color_Foreground);

  int l__position = 0;

  // Protection si valeur negative
  if (i__value <= 0.0) {
    i__value = 0.0;
  }                             

  // Arrondi naturel...
  l__position = (int)((i__value / 100.0) + 0.5);

  // Protection si valeur au dela du max de la font 33x11 des symboles
  if (l__position > 32) {
    l__position = 32;
  }

  /* Pour ne pas effacer la ligne horizontale 50%, changement de 'Y' si 'l__position' = 16
     par rapport a la valeur precedente...
   */
  if (l__position == 16) {
    l__position = (l__position > *io__position) ? 17 : 15;
  }

  Paint_DrawSymbol(230, 65, l__position, &Font33Symbols, TRANSPARENCY, Color_Foreground, true);

  *io__position = l__position;
}

#if USE_SET_SCREEN_VIRTUAL
void Paint::Paint_SetScreenVirtual(UWORD i__x_from, UWORD i__y_from, UWORD i__x_to, UWORD i__y_to, UWORD Color_Background, UWORD Color_Foreground)
{
  for (UWORD x = i__x_from; x < i__x_to; x++) {
    for (UWORD y = i__y_from; y < i__y_to; y++) {
      if (Color_Background == TRANSPARENCY && m__screen_virtual[x-1][y-1] == BLACK) {
        m__screen_virtual[x-1][y-1] = Color_Foreground;
      }
      else if (m__screen_virtual[x-1][y-1] != TRANSPARENCY_2) {
        m__screen_virtual[x-1][y-1] = Color_Foreground;
      }
    }
  }
}
#endif

/* Bargraph des 24 heures
  => 0%   -> Xmax = 1
  => 100% -> Xmax = 238
*/
void Paint::Paint_UpdateBargraph24H()
{
  //Serial.printf("%s(): Entering...\n", __FUNCTION__);

    float l__mark_percent = 100.0 * (g__date_time->getRtcSecInDayLocal() / 86400.0);

#if USE_SET_SCREEN_VIRTUAL
    int   l__mark_pos     = max((238 - min((int)((238.0 * l__mark_percent) / 100.0), 238)), 1);
#else
    int   l__mark_pos     = max((239 - min((int)((239.0 * l__mark_percent) / 100.0), 238)), 1);
#endif

#if 0
    Serial.printf("Bargrap 24H: Epoch [%lu] Sec. l__mark_percent [%.1f%%] -> Pos [%d] [%d -> %d]\n",
      g__date_time->getRtcSecInDayLocal(), l__mark_percent, l__mark_pos,
      max(1, (l__mark_pos - 1)), min(238, (l__mark_pos + 1)));
#endif

#if USE_SET_SCREEN_VIRTUAL
    g__gestion_lcd->Paint_SetScreenVirtual(1, 136, (l__mark_pos - 1), 148, TRANSPARENCY, DARKGRAY);
    //g__gestion_lcd->Paint_SetScreenVirtual(min(238, l__mark_pos - 5), 136, max(1, (l__mark_pos - 5)), 148, TRANSPARENCY, YELLOW);
    g__gestion_lcd->Paint_SetScreenVirtual((l__mark_pos + 1), 136, 238, 148, TRANSPARENCY, DARKBLUE);
#else
    // Affichage du seul curseur de la progression de l'heure @ 24H 
    g__gestion_lcd->Paint_DrawRectangle(2, 100, max(2, (l__mark_pos - 1)), 112, DARKBLUE, DOT_PIXEL_1X1, DRAW_FILL_FULL, WHITE);
    //g__gestion_lcd->Paint_DrawRectangle(max(2, (l__mark_pos - 5)), 100, min(239, (l__mark_pos + 5)), 112, GRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL, WHITE);
    g__gestion_lcd->Paint_DrawRectangle(min(239, (l__mark_pos + 1)), 100, 240, 112, DARKGRAY, DOT_PIXEL_1X1, DRAW_FILL_FULL, WHITE);
#endif
}

/* Warning: L'initialisation des 2 attributs 'm__screen_virtual_period' et 'm__sub_menu_period' doivent etre coherent ;-)
 */
GestionLCD::GestionLCD() : m__screen_virtual_period(SCREEN_VIRTUAL_PERIOD_5_MIN),
                           m__sub_menu_period(SUB_MENU_PERIOD_5_MINUTES), m__sub_menu_unit(SUB_MENU_UNIT_WATTS_HOUR)                      
{
  Serial.printf("GestionLCD::GestionLCD() m__sub_menu_period [%d] m__sub_menu_unit [%d]\n", m__sub_menu_period, m__sub_menu_unit);
}

GestionLCD::~GestionLCD()
{
	Serial.printf("GestionLCD::~GestionLCD()\n");
}

void GestionLCD::setScreenVirtualPeriod(ENUM_SUB_MENU_PERIOD i__value)
{
  Serial.printf("%s(%d)\n", __FUNCTION__, i__value);

  switch (i__value) {
  case SUB_MENU_PERIOD_1_MINUTE:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_1_MIN;
    break;
  case SUB_MENU_PERIOD_5_MINUTES:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_5_MIN;
    break;
  case SUB_MENU_PERIOD_15_MINUTES:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_15_MIN;
    break;
  case SUB_MENU_PERIOD_30_MINUTES:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_30_MIN;
    break;
  case SUB_MENU_PERIOD_1_HOUR:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_1_HOUR;
    break;
  case SUB_MENU_PERIOD_3_HOURS:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_3_HOURS;
    break;
  case SUB_MENU_PERIOD_6_HOURS:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_6_HOURS;
    break;
  default:
    m__screen_virtual_period = SCREEN_VIRTUAL_PERIOD_1_MIN;
    break;
  }

  // Pour la presentation lors du deroulement de l'acquisition (cf. 'callback_menu_scrolling()' de la classe 'Menus')
  m__sub_menu_period = i__value;

  // Marquage de la reinitialisation de la periode en GREEN
  Paint_DrawLine(231, 65, 231, 81, GREEN, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);
  Paint_DrawLine(231, 83, 231, 98, GREEN, DOT_PIXEL_1X1, LINE_STYLE_SOLID, true);

  // Decalage et raffraichissement de l'ecran virtuel
  Paint_ShiftAndRefreshScreenVirtual(true, true);
}
