/**
  ******************************************************************************
  * @file    font16Symbols.c
  */

/* Includes ------------------------------------------------------------------*/
#include "fonts.h"

#if USE_SIMULATION
#define PROGMEM									// Mot cle non supporte en simulation
#endif

const unsigned char Font16Symbols_Table[] PROGMEM = 
{
  // #0: Pave plein de 16 x 11 pixels
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            
  0xFF, 0xE0, //            

  // #1: Pave semi-plein a gauche de 16 x 1 pixels
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            
  0x80, 0x00, //            

  // #2: Pave semi-plein a gauche de 16 x 2 pixels
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            
  0xC0, 0x00, //            

  // #3: Pave semi-plein a gauche de 16 x 3 pixels
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            
  0xE0, 0x00, //            

  // #4: Pave semi-plein a gauche de 16 x 4 pixels
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            
  0xF0, 0x00, //            

  // #5: Pave semi-plein a gauche de 16 x 5 pixels
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            
  0xF8, 0x00, //            

  // #6: Pave semi-plein a gauche de 16 x 6 pixels
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            
  0xFC, 0x00, //            

  // #7: Pave semi-plein a gauche de 16 x 7 pixels
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            
  0xFE, 0x00, //            

  // #8: Pave semi-plein a gauche de 16 x 8 pixels
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            
  0xFF, 0x00, //            

  // #9: Pave semi-plein a gauche de 16 x 9 pixels
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            
  0xFF, 0x80, //            

  // #10: Pave semi-plein a gauche de 16 x 10 pixels
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            
  0xFF, 0xC0, //            

  // #11: Ligne verticale de 3 pixels centres dans un pave 16 x 11 pixels
  0x00, 0x00, //            
  0x00, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x0E, 0x00, //            
  0x00, 0x00, //            
  0x00, 0x00, //            

  // #12: Ligne verticale de 9 pixels centres dans un pave 16 x 11 pixels
  0x00, 0x00, //            
  0x00, 0x00, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x7F, 0xC0, //            
  0x00, 0x00, //            
  0x00, 0x00, //            
};

sFONT Font16Symbols = {
  Font16Symbols_Table,
  11, /* Width */
  16, /* Height */
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
