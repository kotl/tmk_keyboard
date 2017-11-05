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
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "keycode.h"
#include "action.h"
#include "action_util.h"
#include "action_code.h"
#include "action_macro.h"
#include "action_layer.h"
#include "bootloader.h"
#include "report.h"
#include "host.h"
#include "print.h"
#include "debug.h"
#include "keymap.h"
#include "ergodox.h"

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = { \
{ KC_9,KC_0,KC_MINS,KC_EQL,KC_BSPC,KC_6,KC_7,KC_8},  \
{ KC_O,KC_P,KC_LBRC,KC_RBRC,KC_BSLS,KC_FN0,KC_U,KC_I},  \
{ KC_L,KC_SCLN,KC_QUOT,KC_ENT,KC_NO,KC_H,KC_J,KC_K},  \
{ KC_COMM,KC_DOT,KC_SLSH,KC_RSFT,KC_NO,KC_MENU,KC_N,KC_M},  \
{ KC_DOWN,KC_RIGHT,KC_INS,KC_RCTL,KC_DEL,KC_NO,KC_LEFT,KC_UP},  \
{ KC_5,KC_4,KC_3,KC_2,KC_1,KC_GRV,KC_FN2,KC_ESC},  \
{ KC_Y,KC_T,KC_R,KC_E,KC_W,KC_Q,KC_TAB,KC_NO},  \
{ KC_FN1,KC_G,KC_F,KC_D,KC_S,KC_A,ACTION_MODS_KEY(MOD_LALT, KC_LSFT),KC_NO},  \
{ KC_SPC,KC_B,KC_V,KC_C,KC_X,KC_Z,KC_LSFT,KC_NO},  \
{ KC_NO,KC_END,KC_PGUP,KC_PGDN,KC_HOME,KC_LALT,KC_LCTL,KC_LGUI}  \
},  
[1] = { \
{ KC_F9,KC_F10,KC_TRNS,KC_TRNS,KC_PSCR,KC_F6,KC_F7,KC_F8},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,ACTION_LAYER_MOMENTARY(2),KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_VOLD,ACTION_MODS_KEY(MOD_LCTL, KC_PGDOWN),ACTION_MODS_KEY(MOD_LCTL, KC_T),KC_TRNS,ACTION_MODS_KEY(MOD_LCTL, KC_W),KC_NO,ACTION_MODS_KEY(MOD_LCTL, KC_PGUP),KC_VOLU},  \
{ KC_F5,KC_F4,KC_F3,KC_F2,KC_F1,KC_TRNS,ACTION_LAYER_TOGGLE(1),KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ ACTION_LAYER_MOMENTARY(2),KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ KC_NO,ACTION_MODS_KEY(MOD_LALT|MOD_LSFT, KC_TAB),KC_TRNS,ACTION_MODS_KEY(MOD_LALT, KC_F4),ACTION_MODS_KEY(MOD_LALT, KC_TAB),KC_TRNS,KC_TRNS,KC_TRNS}  \
},  
[2] = { \
{ KC_F9,KC_F10,KC_TRNS,KC_TRNS,KC_TRNS,KC_F6,KC_F7,KC_F8},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO,KC_TRNS,KC_TRNS},  \
{ KC_F5,KC_F4,KC_F3,KC_F2,KC_F1,KC_TRNS,KC_TRNS,KC_TRNS},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_NO},  \
{ KC_NO,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS,KC_TRNS}  \
},  
};
static const uint16_t PROGMEM fn_actions[] = {
};

#define KEYMAPS_SIZE    (sizeof(keymaps) / sizeof(keymaps[0]))
#define FN_ACTIONS_SIZE (sizeof(fn_actions) / sizeof(fn_actions[0]))

/* translates key to keycode */
uint8_t keymap_key_to_keycode(uint8_t layer, keypos_t key)
{
    if (layer < KEYMAPS_SIZE) {
        return pgm_read_byte(&keymaps[(layer)][(key.row)][(key.col)]);
    } else {
        // fall back to layer 0
        return pgm_read_byte(&keymaps[0][(key.row)][(key.col)]);
    }
}

#if defined(KEYMAP_CUB)

// function keymap_fn_to_action will be defined in keymap_cub.h

#else
/* translates Fn keycode to action */
action_t keymap_fn_to_action(uint8_t keycode)
{
    action_t action;
    if (FN_INDEX(keycode) < FN_ACTIONS_SIZE) {
        action.code = pgm_read_word(&fn_actions[FN_INDEX(keycode)]);
    } else {
        action.code = ACTION_NO;
    }
    return action;
}
#endif

