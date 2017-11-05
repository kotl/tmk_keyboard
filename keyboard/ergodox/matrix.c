/*
Copyright 2013 Oleg Kostyuk <cub.uanic@gmail.com>

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

/*
 * scan matrix
 */
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "action_layer.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "ergodox.h"
#include "protocol/serial.h"

#ifdef DEBUG_MATRIX_SCAN_RATE
#include  "timer.h"
#endif

#ifndef DEBOUNCE
#   define DEBOUNCE 2
#endif
static uint8_t debouncing = DEBOUNCE;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static matrix_row_t read_cols(uint8_t row);
static void init_cols(void);
static void unselect_rows();
static void select_row(uint8_t row);

#ifdef DEBUG_MATRIX_SCAN_RATE
uint32_t matrix_timer;
uint32_t matrix_scan_count;
#endif

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    // initialize row and col
    init_ergodox();
    unselect_rows();
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debouncing[i] = 0;
    }

#ifdef DEBUG_MATRIX_SCAN_RATE
    matrix_timer = timer_read32();
    matrix_scan_count = 0;
#endif

    serial_init();
}

uint8_t temp_matrix[MATRIX_ROWS/2][MATRIX_COLS];

uint8_t matrix_scan(void)
{
  #ifdef DEBUG_MATRIX_SCAN_RATE
    matrix_scan_count++;

    uint32_t timer_now = timer_read32();
    if (TIMER_DIFF_32(timer_now, matrix_timer)>1000) {
        print("matrix scan frequency: ");
        pdec(matrix_scan_count);
        print("\n");

        matrix_timer = timer_now;
        matrix_scan_count = 0;
    }
  #endif


  memset(temp_matrix, 0, sizeof(matrix));
/*  for(uint8_t i=0;i<MATRIX_COLS; i++) {
    int16_t tmp = 0;
    for(uint8_t attempt = 0; attempt<350; attempt++) {
      tmp = serial_recv2();
      _delay_us(1);
      if(tmp>0) break;
    }
    if (tmp>0) {
      tmp = ~tmp & 0xFF;
      uint8_t col = tmp >> 5;
      uint8_t rows = tmp & 0b11111;
      for (int16_t j=MATRIX_ROWS/2-1;j>=0; j-- ) {
        temp_matrix[j][col] = (rows & 1);
        rows = rows >> 1;
      }
    }
  }
                */
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix_row_t cols = 0;
	if (i<MATRIX_ROWS/2) {
          select_row(i);
          _delay_us(30);  // without this wait read unstable value.
          cols = read_cols(i);
        } else {
          for (int16_t col = MATRIX_COLS-1; col >= 0; col--) {
            cols = cols << 1;
            cols += temp_matrix[i-MATRIX_ROWS/2][col];        
          }
        }
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            if (debouncing) {
                debug("bounce!: "); debug_hex(debouncing); debug("\n");
            }
            debouncing = DEBOUNCE;
        }
        unselect_rows();
    }

    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                matrix[i] = matrix_debouncing[i];
            }
        }
    }

  return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

static void  init_cols(void)
{
  // Input with pull-up(DDR:0, PORT:1)
  DDRF &= ~(1<<7);
  PORTF |= (1<<7);
  DDRD &= ~(1<<7);
  PORTD |= (1<<7);

  DDRB &= ~(1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<5 | 1<<6 );
  PORTB |= (1<<0 | 1<<1 | 1<<2 | 1<<4 | 1<<5 | 1<<6 );
   
}

static matrix_row_t read_cols(uint8_t row)
{
   return (PINF&(1<<7) ? 0 : (1<<0))|
	  (PINB&(1<<6) ? 0 : (1<<1))|
	  (PINB&(1<<5) ? 0 : (1<<2))|  
	  (PINB&(1<<4) ? 0 : (1<<3 ))|  
	  (PIND&(1<<7) ? 0 : (1<<4 ))|  
	  (PINB&(1<<0) ? 0 : (1<<5 ))|  
	  (PINB&(1<<1) ? 0 : (1<<6 ))|  
	  (PINB&(1<<2) ? 0 : (1<<7 ));
}

static void unselect_rows(void)
{
    // Hi-Z(DDR:0, PORT:0) to unselect
    DDRF &= ~0b01110011;
    PORTF &= ~0b01110011;
}

static void select_row(uint8_t row)
{
    // Output low(DDR:1, PORT:0) to select
    switch (row) {
        case 0:
            DDRF  |= (1<<0);
            PORTF &= ~(1<<0);
            break;
        case 1:
            DDRF  |= (1<<1);
            PORTF &= ~(1<<1);
            break;
        case 2:
            DDRF  |= (1<<4);
            PORTF &= ~(1<<4);
            break;
        case 3:
            DDRF  |= (1<<5);
            PORTF &= ~(1<<5);
            break;
        case 4:
            DDRF  |= (1<<6);
            PORTF &= ~(1<<6);
            break;
    }
}

