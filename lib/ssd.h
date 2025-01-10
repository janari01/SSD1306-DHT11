#ifndef SSD_H
#define SSD_H

/*
    In this file:

    declare addresses for SSD1306
    declare functions
*/

#include <stdint.h> 
#include "font.h"  
#include "com.h"

#define DEVICE_ADDRESS              0x3C

// Fundamental commands
#define SSD1306_SET_CONTRAST        0x81 // Double byte command to select 1 out of 256 contrast steps.
#define SSD1306_DIS_ENT_DISP_ON     0xA4 // Entire display on, X0=0b: Resume to RAM content display (RESET), output follows RAM content
#define SSD1306_DISP_ON_IGNORE_RAM  0xA5 // Output ignores RAM content
#define SSD1306_DISP_NORMAL         0xA6 // Normal display (RESET), 0 in RAM: OFF in display panel, 1 in RAM: ON in display panel
#define SSD1306_DISP_INVERSE        0xA7 // Inverse display, 0 in RAM: ON in display panel, 1 in RAM: OFF in display panel 
#define SSD1306_DISP_OFF            0xAE // Display OFF (sleep mode) (RESET) 
#define SSD1306_DISP_ON             0xAF // Display ON in normal mode

// Addressing settings
#define SSD1306_START_ADDRESS       0x00 // Set the lower nibble of the column start address register for Page Addressing Mode using X[3:0] as data bits.
#define SSD1306_SET_ADDR_MODE_MEM   0x20 // Set memory, addressing mode
#define SSD1306_SET_ADDR_MODE_COL   0x21 // Setup column start and end address 
/*
    A[6:0] : Column start address, range : 0-127d,
        (RESET=0d)
    B[6:0]: Column end address, range : 0-127d,
        (RESET =127d)
*/
#define SSD1306_SET_ADDR_PAGE       0x22 // Setup page start and end address
/*
    A[2:0] : Page start Address, range : 0-7d,
        (RESET = 0d)
    B[2:0] : Page end Address, range : 0-7d,
        (RESET = 7d)
*/

// Hardware configuration (panel resolution & layout related) command table
#define SSD1306_SET_START_LINE      0x40 // Set display RAM display start line register from 0-63 using X5 X3 X2 X1 X0, Display start line register is reset to 000000b
#define SSD1306_SEG_REMAP_ZERO      0xA0 // Column address 0 is mapped to SEG0 (RESET)
#define SSD1306_SEG_REMAP_END       0xA1 // Column address 127 (END) is mapped to SEG0
#define SSD1306_SET_MULTPLX_RATION  0xA8 // Set MUX ration to N+1 MUX
#define SDD1306_SCAN_DIRECTION      0xC0 // X[3]=0b: normal mode (RESET) Scan from COM0 to COM[N –1]. Where N is the Multiplex ratio. 
#define SSD1306_SCAN_DIRECTION_OP   0xC8 // OP - opposite, X[3]=1b: remapped mode. Scan from COM[N-1] to COM0. Where N is the Multiplex ratio. 
#define SSD1306_SET_DISP_OFFSET     0xD3 // Set vertical shift by COM from 0d~63d, The value is reset to 00h after RESET
#define SSD1306_SET_COM_PIN_CONFIG  0xDA // Set COM Pins Hardware Configuration, 
// A[4]=0b, Sequential COM pin configuration, A[4]=1b(RESET), Alternative COM pin configuration
// A[5]=0b(RESET), Disable COM Left/Right remap, A[5]=1b, Enable COM Left/Right remap 

// Timing & driving scheme setting command table
#define SSD1306_SET_OSC_FREQ        0xD5 // Set Display Clock Divide Ratio/Oscillator Frequency
// A[3:0] : Define the divide ratio (D) of the  display clocks (DCLK): Divide ratio= A[3:0] + 1, RESET is 0000b (divide ratio = 1) 
// A[7:4] : Set the Oscillator Frequency, FOSC. Oscillator Frequency increases with the value of A[7:4] and vice versa. RESET is 1000b
#define SSD1306_SET_PRE_CHARGE_PERD 0xD9 // Set Pre-charge Period
#define SSD1306_SET_VCOMH_DESELECT  0xDB // Set VCOMH deselect level
#define SSD1306_NOP                 0xE3 // Command for no operation

#define SSD1306_SET_CHAR_REG        0x8D // Enable charge pump during display on
#define SSD1306_DEACT_SCROLL        0x2E // deactivate scroll

// #define INIT_STATUS                 0xFF // 255 in decimal

// --- screen ---
#define START_PAGE_ADDRESS          0
#define END_PAGE_ADDRESS            7
#define START_COL_ADDRESS           0
#define END_COL_ADDRESS             127
#define RAM_X_END                   END_COL_ADDRESS + 1
#define RAM_Y_END                   END_PAGE_ADDRESS + 1

#define MAX_X                       END_COL_ADDRESS
#define MAX_Y                       (END_PAGE_ADDRESS + 1) * 8

// cache
#define CACHE_SIZE_MEMORY           (1 + END_PAGE_ADDRESS) * (1 + END_COL_ADDRESS)

// TEXT AND LINES

// initialization
uint8_t ssd1306_init(uint8_t addr);

// set position?
void ssd1306_set_position(uint8_t x, uint8_t y);

// update position
uint8_t ssd1306_update_position();

// for character selection 
uint8_t ssd1306_draw_char(char chara);

// draw string
void ssd1306_draw_string(char *str);

// clear screen
void ssd1306_clear_screen();

// update screen with cached oled data
uint8_t ssd1306_update_screen(uint8_t addr);

// draw line
uint8_t ssd1306_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// place pixels into buffer
uint8_t ssd1305_put_pixel(uint8_t x, uint8_t y);


// DISPLAYING FRAMES OR ANIMATIONS

// for animations, before set ssd1306_set_animations_mode
uint8_t ssd1306_draw_bitmap(uint8_t* buf, uint8_t w, uint8_t h);

// set the animation or image starting location
// MUST be set before using ssd1306_draw_bitmap
uint8_t ssd1306_set_animations_mode(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

// reset the current buffer init variable
void ssd1306_reset_animation_state();

// draw a blank frame of width w and height h (all px)
void ssd1306_draw_blank_frame(uint8_t w, uint8_t h);

#endif                  