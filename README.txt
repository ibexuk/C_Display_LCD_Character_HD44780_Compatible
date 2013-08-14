
######################################################################
##### Open sourced by IBEX, an electronic product design company #####
#####    based in South East England.  http://www.ibexuk.com     #####
######################################################################


Most LCD Character displays are designed to be HD44780 compatible. This generic driver provides control of many different variants of these screens and operates in 4 bit mode to reduce the pins required from the microcontroller.

This driver provides the following features:

    Compatible with most standard 1, 2 or 4 line displays with up to 40 characters
    Reads the busy flag back from the LCD screen so that the display may be updated as quickly as possible
    Functions in the background checking for being able to write the next character every time its main function is called, to avoid code execution stall when using slow displays.
    Supports left, middle and right aligned text.
    Display from constant program memory or variable ram memory.
    Only store as many characters as required in program memory – Null character determines end of lines text an avoids wasted program memory space.
    A simple driver that just drops in to your project and makes your life easier



This driver provides the following functions:


INITIALISE LCD
void lcd_initialise (void)
Call on power-up


UPDATE LCD DISPLAY
void lcd_update_display (void)
Call as part of your main loop. This function outputs characters to the display and exits if the display is currently busy to avoid application stall with slow displays.


WRITE LINE OF LCD DISPLAY (Versions for constant and variable strings)
void lcd_display_const_string (BYTE line_number, BYTE alignment, CONSTANT BYTE *text_line)
void lcd_display_variable_string (BYTE line_number, BYTE alignment, BYTE *text_line)
Call these functions to display a line of text
line_number 0 = top line
alignment LCD_ALIGN_LEFT, LCD_ALIGN_CENTER or, LCD_ALIGN_RIGHT
text_line Null terminated string


IS SCREEN BUSY
BYTE lcd_is_screen_busy (void)
Returns 1 if screen is still busy from last operation, 0 if not


SEND HIGH AND LOW NIBBLES OF W TO LCD DATA REG
void lcd_write_data_byte (BYTE data)


SEND HIGH AND LOW NIBBLES TO LCD CTRL REG
void lcd_write_control_byte (BYTE data)


SEND HI NIBBLE OF W TO LCD CTRL REG
void lcd_write_ctrl_hi_nibble (BYTE data)


SEND LO NIBBLE OF W TO LCD CTRL REG
void lcd_write_ctrl_lo_nibble (BYTE data)


Please view the sample source code files for full information regarding the driver.