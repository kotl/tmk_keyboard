/*
Copyright 2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONFIG_H
#define CONFIG_H


/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x1111
#define DEVICE_VER      0x0001
#define MANUFACTURER    geekhack
#define PRODUCT         kotl
#define DESCRIPTION     t.m.k. keyboard firmware for Onekey

/* key matrix size */
#define MATRIX_ROWS 10
#define MATRIX_COLS 8

// Cols: PF7 PB6 PB5 PB4 PD7 PB0 PB1 PB2

// Rows: PF0 PF1 PF4 PF5 PF6


/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE    3

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)



/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
// #define NO_DEBUG

/* disable print */
// #define NO_PRINT

#define TAPPING_TOGGLE  1

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
// #define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION


/*
 * Serial(USART) configuration
 */

#define DEBUG_MATRIX_SCAN_RATE

#ifdef SERIAL_UART_RTS_LO
  #undef SERIAL_UART_RTS_LO
  #undef SERIAL_UART_RTS_HI
#endif

#ifdef __AVR_ATmega32U4__
    #define SERIAL_UART_BAUD       ((57600  >> 1)-1)
//    #define SERIAL_UART_BAUD       57600
    #define SERIAL_UART_DATA       UDR1
    #define SERIAL_UART_UBRR       ((F_CPU/(16UL*SERIAL_UART_BAUD))-1)
    #define SERIAL_UART_RXD_VECT   USART1_RX_vect
    #define SERIAL_UART_TXD_READY  (UCSR1A&(1<<UDRE1))
    #define SERIAL_UART_INIT()     do { \
	UBRR1 = SERIAL_UART_UBRR; \
/*        UBRR1L = (uint8_t) SERIAL_UART_UBRR;       /* baud rate */ \
/*        UBRR1H = (uint8_t) (SERIAL_UART_UBRR>>8);  /* baud rate */ \
	UCSR1A = 0; \
	UCSR1B = (1<<RXEN1) | (0<<TXCIE1) | (0<<TXEN1) | (1<<RXCIE1); /* added from teensy */ \
        UCSR1C |= 0x6; /* constant 0x06 from teensy */ \
        sei(); \
    } while(0)
#else
    #error "USART configuration is needed."
#endif

#endif
