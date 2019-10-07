/* 
 * File:   lcd.h
 * Author: AB
 *
 * Created on January 30, 2019, 11:45 PM
 */

#ifndef LCD_H
#define LCD_H

/********************************* Includes **********************************/
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "configBits.h"

/********************************** Macros ***********************************/
#define RS LATDbits.LATD2          
#define E  LATDbits.LATD3

/** @brief Clears both LCD lines */
#define lcd_clear(){\
	lcdInst(0x01);\
	__delay_ms(5);\
}

/** @brief Sets cursor position to start of first line */
#define lcd_home(){\
	lcdInst(0x80);\
	__delay_ms(2);\
}

/**
 * @brief Sets the cursor's position to a specific display data RAM (DDRAM)
 *        address
 * @param addr The DDRAM address to move the cursor to (min: 0, max: 127)
 * @note The cursor will not be visible at all addresses
 */
#define lcd_set_ddram_addr(addr){\
    lcdInst(0x80 | addr);\
}

/**
 * @brief Backlight and cursor control
 * @param display_on Turns on the backlight if true, otherwise turns it off
 * @param cursor_on Turns on cursor if true, otherwise turns it off
 * @param blink_cursor Blinks the cursor if true, otherwise cursor is static
 */
#define lcd_display_control(\
	display_on,\
	cursor_on,\
	blink_cursor\
)\
{\
	lcdInst(\
        (unsigned char)(8 | (display_on << 2) | (cursor_on << 1) | blink_cursor)\
    );\
}

/******************************** Constants **********************************/
// Display dimensions as seen in the real world (before you use these in your
// code, double-check that they match the size of your LCD)
extern const unsigned char LCD_SIZE_HORZ; /**< Number of visible columns */
extern const unsigned char LCD_SIZE_VERT; /**< Number of visible rows    */

extern const unsigned char LCD_LINE1_ADDR; /**< Address of first line */
extern const unsigned char LCD_LINE2_ADDR; /**< Address of 2nd line   */
extern const unsigned char LCD_LINE3_ADDR; /**< Address of 3rd line   */
extern const unsigned char LCD_LINE4_ADDR; /**< Address of 4th line   */

/********************************** Types ************************************/
/** @brief The directions the display contents and cursor can be shifted */
typedef enum{
	LCD_SHIFT_LEFT = 0, /**< Left shift  */
	LCD_SHIFT_RIGHT = 1 /**< Right shift */
}lcd_direction_e;

/************************ Public Function Prototypes *************************/
/**
 * @brief Sends a command to a display control register
 * @param data The command byte for the Hitachi controller
 */
void lcdInst(char data);

/** @brief Performs the initial setup of the LCD */
void initLCD(void);

/**
 * @brief Moves the cursor in a given direction by numChars characters
 * @param numChars The number of character positions by which the cursor is to
 *        be moved (min: 0, max: 127)
 * @param direction The direction for which the shift is to occur
 */
void lcd_shift_cursor(unsigned char numChars, lcd_direction_e direction);

/**
 * @brief Shifts the display in a given direction by numChars characters
 * @param numChars The number of character positions by which the display
 *        contents are to be shifted (min: 0, max: 127)
 * @param direction The direction for which the shift is to occur
 */
void lcd_shift_display(unsigned char numChars, lcd_direction_e direction);

/**
 * @brief Sends a character to the display for printing
 * @details The familiar C function printf internally calls a function named
 *          "putch" (put character) whenever a character is to be written to
 *          the screen. Here we have chosen to implement putch so that it
 *          sends the character to the LCD, but you can choose to implement it
 *          however you'd like (e.g. send the character over UART, etc.)
 * @param data The character (byte) to be displayed
 */
void putch(char data);


const char keys[] = "123A456B789C*0#D";

const char msg_start1[] = "Press any key";
const char msg_start2[] = "to start";

const char msg_wait[] = "Starting up...";
const char msg_run_in_progress[] = "In progress.";
const char msg_run_complete[] = "Run complete.";

const char msg_press1_1[] = "Press 1 for";
const char msg_press1_2[] = "operation time";

const char msg_press2_1[] = "Press 2 for";
const char msg_press2_2[] = "tires dispensed";

const char msg_press3_1[] = "Press 3 for";
const char msg_press3_2[] = "tires after op";

const char msg_press4_1[] = "Press 4 for";
const char msg_press4_2[] = "# supplied tires";

const char msg_press5_1[] = "Press 5 for";
const char msg_press5_2[] = "# of poles";

const char msg_press6_1[] = "Press 6 for";
const char msg_press6_2[] = "pole distances";

const char msg_press7_1[] = "Press 7 for";
const char msg_press7_2[] = "pole distances";

const char msg_press8_1[] = "Press 8 for";
const char msg_press9_1[] = "Press 9 for";
const char msg_press0_1[] = "Press 0 for";
const char msg_presshash_1[] = "Press # to";

const int run_time_tick = 2000;
const int waiting_time = 1500;
const int display_interval = 2000;
const int run_data_interval = 4000;


//volatile bool key_was_pressed = true;

/**
 * @}
 */
/* Not */
/*Set cursor to specified row and column of LCD*/
void LCD_set_cursor(unsigned int row, unsigned int column);

void LCD_write_str(unsigned char *str);

void LCD_write_char(unsigned char str);

#endif	/* LCD_H */