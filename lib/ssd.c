#include <stdint.h>
#include "ssd.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// like a pointer, points where data will be written (framebuffer)
// used for text and lines, not for animations and images
unsigned int _counter;

uint8_t INIT_SSD1306[] = {
  SSD1306_DISP_OFF,                                     // 0xAE, Set Display OFF
  SSD1306_SET_MULTPLX_RATION,     0x3F,                 // 0xA8 - 0x3F for 128 x 64 version (64MUX)
  SSD1306_SET_DISP_OFFSET,        0x00,                 // 0xD3
  SSD1306_SET_START_LINE,                               // 0x40
  SSD1306_SEG_REMAP_END,                                // 0xA1 
  SSD1306_SCAN_DIRECTION_OP,                            // 0xC8
  SSD1306_SET_COM_PIN_CONFIG,     0x12,                 // 0xDA
  SSD1306_SET_CONTRAST,           0xFF,                 // 0x81
  SSD1306_DIS_ENT_DISP_ON,                              // 0xA4
  SSD1306_DISP_NORMAL,                                  // 0xA6
  SSD1306_SET_OSC_FREQ,           0x80,                 // 0xD5
  SSD1306_SET_CHAR_REG,           0x14,                 // 0x8D
  SSD1306_DISP_ON,                                      // 0xAF
  SSD1306_SET_ADDR_MODE_MEM,      0x00,                 // 0x20, 0x00 = horizontal, 1 = vertical, 2 = page
  SSD1306_SET_ADDR_MODE_COL,      0, END_COL_ADDRESS,   // Set the screen wrapping points
  SSD1306_SET_ADDR_PAGE,          0, END_PAGE_ADDRESS
};

// cache mem 8*128=1024
uint8_t _framebuffer[CACHE_SIZE_MEMORY];
uint8_t *framebuffer = _framebuffer;

// Register 0x00 is for sending control commands.
// Register 0x01 is for sending data.

// send starting commands
// args: addr (device address)
uint8_t ssd1306_init(uint8_t addr)
{
  uint8_t *list = INIT_SSD1306;
  int status = 1;
  uint8_t commands = 27;

  i2c_init(); // initialize i2c connection

  while (commands--)
  {
    status = i2c_write(DEVICE_ADDRESS, SSD1306_START_ADDRESS, *list++); // increments pointer to other array elements
    if (status != 0)
    {
      i2c_stop();
      return status;
    }
  }

  i2c_stop();
  return 0;
}

// args: addr (device address)
uint8_t ssd1306_update_screen(uint8_t addr)
{
  uint8_t status = 1;
  uint16_t p = 0;

  i2c_init();

  // p < 1024
  while (p < CACHE_SIZE_MEMORY)
  {
    // write image buffer to ssd1306
    status = i2c_write(DEVICE_ADDRESS, 0x40, _framebuffer[p++]);
    if (status != 0)
    {
      i2c_stop();
      return status;
    }
  }

  i2c_stop();
  return 0;
}

// args: x (start x), y (pages -> 0 ... 7)
void ssd1306_set_position(uint8_t x, uint8_t y) 
{
  _counter = x + (y << 7);  // (page * 128px as width per row) + offset
}

// args: 
uint8_t ssd1306_update_position()
{
  uint8_t y = _counter >> 3;                    // y / 8, previosuly was >> 7
  uint8_t x = _counter & 0x07;                  // x % 8
  uint8_t x_new = x + CHARS_COLS_LENGTH + 1;

  if (x_new > END_COL_ADDRESS) 
  {                                             // check position
    if (y > END_PAGE_ADDRESS) 
    {                                           // if more than allowable number of pages
      return 1;                                 // return out of range
    } 
    else if (y < (END_PAGE_ADDRESS - 1)) 
    {                                           // if x reach the end but page in range
      _counter = ((++y) << 7);                  // update
    }
  }
  return 0;
}

// args: chara (single character)
uint8_t ssd1306_draw_char(char chara)
{
  uint8_t p = 0;

  if (ssd1306_update_position() != 0) { return 1; }

  while (p < CHARS_COLS_LENGTH)
  {
    // (ASCII)
    framebuffer[_counter++] = FONTS[(uint8_t)chara-32][p++];
  }
  _counter++;

  return 0;
}

// args: *str (pointer to a string)
void ssd1306_draw_string(char *str)
{
  int p = 0;
  while (str[p] != '\0')
  {
    ssd1306_draw_char(str[p++]);
  }
}

// args: 
void ssd1306_clear_screen()
{
  // overwrite buffer with 0's
  memset(framebuffer, 0x00, CACHE_SIZE_MEMORY); 
}

// args: x (start x), y (in pixels, single page is 8 bits tall for each col)
uint8_t ssd1305_put_pixel(uint8_t x, uint8_t y)
{
  if ((x > MAX_X) || (y > MAX_Y)) { return 1; }

  uint8_t page = y >> 3;                  // y / 8, which page the pixel relies in

  /*
    (y - (page << 3))
    1)
    Since the screen is 64 pixels high and each page covers 8 rows, 
    when you divide y by 8 (or shift it by 3), you calculate which page the pixel is located in.

    2)
    Once you know which page the pixel is in, you need to calculate the position of the pixel within the page. 
    This is done by subtracting the page's starting vertical position from y

  */
  uint8_t pixel = 1 << (y - (page << 3));
  _counter = x + (page << 7);             // x + (page * 128) -> offset + which page * full width
  framebuffer[_counter++] |= pixel;       // pixel is the value of the row where it's located (on a page)

  return 0;
}

// Bresenham's Line Algorithm
// args: x0 (start x), y0 (start y), x1 (end x), y1 (end y)
uint8_t ssd1306_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  // so that we can have negative and positive horizontal values
  // basically make x1 the largest value, opposite to x0
  if (x0 > x1)
  {
    uint8_t tmp = x0;
    x0 = x1;
    x1 = tmp;
    
    tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  uint16_t dx = x1 - x0; // x change
  uint16_t dy = y1 - y0; // y change

  uint16_t dir = dy < 0 ? -1 : 1; // change the direction if needed
  dy *= dir;

  if (dx != 0)
  {
    int y = y0;

    // now we need to decide, do we move down or continue our line
    int p = (2 * dy) - dx; 
    for (int i = 0; i < (dx + 1); ++i)
    {
      ssd1305_put_pixel(x0 + i, y);
      if (p >= 0)
      {
        y += dir;
        p = p + (2 * dy) - (2 * dx);
      } 
      else 
      {
        p = p + (2 * dy);
      }

    }
  } 
  else 
  {
    perror("Division with 0");
    return 1;
  }

  return 0;
}

// updated by function (ssd1306_set_animations_mode)
typedef struct {
  uint8_t column_start;
  uint8_t column_end;
  uint8_t page_start;
  uint8_t page_end;
} ssd1306_address_state;

static ssd1306_address_state current_state = {0, 127, 0, 7};
static uint8_t is_disp_init = 0; // hold value between function calls

// args:
// x, y - where to start drawing the image (px)
// w, h - height and width of the image (px)
uint8_t ssd1306_set_animations_mode(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  uint8_t page_start;
  uint8_t page_end;
  uint8_t oled_height = 64; // px

  // basically, if this goes to negatives, user has entered too large y value for positioning
  // get the starting page value
  if (oled_height - y - h >= 0) { page_start = y >> 3; }

  // since value is too large, recompute the page start and end
  // & 0x07 as % 8, get the ramainder then calculate page (always 0)
  else { page_start = (y & 0x07) >> 3; }

  if ((128 - (x + w)) < 0) { return 1; } // incurious.

  page_end = page_start + (h >> 3) - 1; // page start + needed pages - 1 

  if (!is_disp_init)
  {
    uint8_t status = 0;
    i2c_init();
    status |= i2c_write(DEVICE_ADDRESS, 0x00, 0x21); // col start
    status |= i2c_write(DEVICE_ADDRESS, 0x00, x); // col start
    status |= i2c_write(DEVICE_ADDRESS, 0x00, (x + w - 1)); // col end
    status |= i2c_write(DEVICE_ADDRESS, 0x00, 0x22); // page start
    status |= i2c_write(DEVICE_ADDRESS, 0x00, page_start); // page start
    status |= i2c_write(DEVICE_ADDRESS, 0x00, page_end); // page end
    if (status != 0) 
    {
      i2c_stop();
      return status;
    }
    is_disp_init = 1;
    i2c_stop();
  }

  current_state.column_start = x;
  current_state.column_end = (x + w - 1);
  current_state.page_start = page_start;
  current_state.page_end = page_end;
  return 0;
}

// args:
void ssd1306_reset_animation_state() {
  is_disp_init = 0; // Reset the initialization flag
}

// args: w (width of the area, px), h (height of the area, px)
void ssd1306_draw_blank_frame(uint8_t w, uint8_t h)
{
  i2c_init();
  for (uint16_t i = 0; i < (w * ((h >> 3))); i++) {
    i2c_write(DEVICE_ADDRESS, 0x40, 0x00);
  }
  i2c_stop();
}

// args:
// buf - the image buffer
// w, h - height and width of the image (px)
uint8_t ssd1306_draw_bitmap(uint8_t* buf, uint8_t w, uint8_t h)
{
  i2c_init();
  uint8_t status = 1;

  uint8_t page_total = (h >> 3);
  for (uint16_t j = 0; j < (w * page_total); j++)
  {
    // try to update the display directly, makes things faster
    status = i2c_write(DEVICE_ADDRESS, 0x40, buf[j]);
    if (status != 0) { return status; }
  }
  i2c_stop();

  return 0;
}