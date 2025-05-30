/**
  ******************************************************************************
  * @file    font24Symbols.c
  */

/* Includes ------------------------------------------------------------------*/
#include "fonts.h"

#if USE_SIMULATION
#define PROGMEM									// Mot cle non supporte en simulation
#endif

const uint8_t Font24Symbols_Table [] PROGMEM = 
{
  // Symbole Feu plein (17 pixels wide)
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x01, 0x80, 0x00, //        % %       
  0x0F, 0xF0, 0x00, //     %### ###%
  0x3F, 0xFC, 0x00, //   %##### #####%   
  0x7F, 0xFE, 0x00, //  %###### ######%  
  0x7F, 0xFE, 0x00, //  %###### ######%  
  0xFF, 0xFF, 0x00, // %####### #######% 
  0xFF, 0xFF, 0x00, // %####### #######% 
  0xFF, 0xFF, 0x00, // %####### #######% 
  0xFF, 0xFF, 0x00, // %####### #######% 
  0xFF, 0xFF, 0x00, // %####### #######% 
  0xFF, 0xFF, 0x00, // %####### #######%
  0x7F, 0xFE, 0x00, //  %###### ######% 
  0x7F, 0xFE, 0x00, //  %###### ######% 
  0x3F, 0xFC, 0x00, //   %##### #####%   
  0x0F, 0xF0, 0x00, //     %### ###%     
  0x01, 0x80, 0x00, //        % %       
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  

  // Symbole Feu contour (17 pixels wide)
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x01, 0x80, 0x00, //        # #       
  0x04, 0x20, 0x00, //      #     #      
  0x10, 0x08, 0x00, //    #         #    
  0x20, 0x04, 0x00, //   #           #   
  0x20, 0x04, 0x00, //   #           #   
  0x40, 0x02, 0x00, //  #             #  
  0x40, 0x02, 0x00, //  #             #  
  0x40, 0x02, 0x00, //  #             #  
  0x40, 0x02, 0x00, //  #             #  
  0x40, 0x02, 0x00, //  #             #  
  0x40, 0x02, 0x00, //  #             # 
  0x20, 0x04, 0x00, //   #           #  
  0x20, 0x04, 0x00, //   #           #  
  0x10, 0x08, 0x00, //    #         #    
  0x04, 0x20, 0x00, //      #     #      
  0x01, 0x80, 0x00, //        # #       
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  
  0x00, 0x00, 0x00, //                  

  // Symbole Bord de cadre (17 pixels wide)
  0xFF, 0xFF, 0x80, // ######## #########
  0x80, 0x00, 0x80, // #                 
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0x80, 0x00, 0x80, // #
  0xFF, 0xFF, 0xFF, // #
};

sFONT Font24Symbols = {
  Font24Symbols_Table,
  17, /* Width */
  24, /* Height */
};

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

