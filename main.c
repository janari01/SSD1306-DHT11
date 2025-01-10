#include "lib/ssd.h"
#include "dht11/dht.h"
#include <stdio.h>
#include "lib/curveAM.h"
#include <unistd.h> 

uint8_t display_parabola()
{
    /*
        NB!
        
        ssd1306_set_animations_mode()
            - ONLY FOR ANIMATIONS AND IMAGES
            - sets col and page start/end

        ssd1306_reset_animation_state()
            - needs to be used before calling ssd1306_set_animations_mode the second time
            - ability to make changes to oled display addresses
        
        ssd1306_draw_blank_frame()
            - ssd1306_reset_animation_state and ssd1306_set_animations_mode
            needs to be called if clearing for the second time
            - clears screen at x, y
    */
    uint8_t status = 0;
    ssd1306_reset_animation_state();
    status |= ssd1306_set_animations_mode(0, 16, 128, 48);
    ssd1306_draw_blank_frame(128, 48);
    ssd1306_reset_animation_state(); // if removed, pixel coordinates will be wrong
    status |= ssd1306_set_animations_mode(40, 16, 48, 48); // y = 16 = 2. page

    if (status != 0) { return status; }

    for (uint8_t i = 0; i < 6; i++)
    {
        for (uint8_t j = 0; j < 27; j++)
        {
            ssd1306_draw_bitmap(curve_animation_arr_holder[j], 48, 48);
        }
    }

    // clear a section (for text)
    ssd1306_reset_animation_state();
    status = ssd1306_set_animations_mode(0, 0, 128, 64);
    if (status != 0) { return status; }

    return 0;
}

int main()
{
    ssd1306_init(DEVICE_ADDRESS);

    ssd1306_draw_blank_frame(128, 64); 

    while (1)
    {
        int* data_arr = read_sensor();
        if (data_arr == NULL) { continue; }

        // format hum. string, buffer
        char rh_buf[20];
        snprintf(rh_buf, sizeof(rh_buf), "RH: %d.%d %%", data_arr[0], data_arr[1]);

        // format temp. string, buffer
        char temp_buf[20];
        snprintf(temp_buf, sizeof(temp_buf), "temp: %d.%d *C", data_arr[2], data_arr[3]);

        // look at the screen sections: P 25/59
        // 0-th page to 7-th page
        // 0 - 15 is the yellow section

        // page 0
        ssd1306_set_position(5, 0);
        ssd1306_draw_string("DHT11: temp/humidity");

        // page 0
        ssd1306_draw_line(0, 10, MAX_X, 10);
        ssd1306_draw_line(0, 11, MAX_X, 11);

        // page 3
        ssd1306_set_position(30, 3);
        ssd1306_draw_string(temp_buf);

        // page 4
        ssd1306_draw_line(10, 35, 117, 35);

        // page 5
        ssd1306_set_position(59, 5);
        ssd1306_draw_string(rh_buf);

        ssd1306_update_screen(DEVICE_ADDRESS);
        free(data_arr);
        usleep(15000000); // 15 sec

        uint8_t par_st = display_parabola();
        if (par_st != 0) { return 1; }
    }

    return 0;
}