/* Copyright 2020 BINEPAD
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once


#define POINTING_DEVICE_DEBUG
//#define AZOTEQ_IQS5XX_ADDRESS 0x74

//#define SPLIT_POINTING_ENABLE
//#define POINTING_DEVICE_THIS_SIDE true

//#define AZOTEQ_IQS5XX_TPS43
#if 0
//#define AZOTEQ_IQS5XX_TPS65
#else
#define AZOTEQ_IQS5XX_WIDTH_MM 75
#define AZOTEQ_IQS5XX_HEIGHT_MM 47
#define AZOTEQ_IQS5XX_RESOLUTION_X 3584
#define AZOTEQ_IQS5XX_RESOLUTION_Y 2048
#define POINTING_DEVICE_INVERT_Y
#endif
//#define AZOTEQ_IQS5XX_ROTATION_90
#define AZOTEQ_IQS5XX_ROTATION_180
//#define AZOTEQ_IQS5XX_ROTATION_270

#define POINTING_DEVICE_MOTION_PIN D7
#define POINTING_DEVICE_MOTION_PIN_ACTIVE_LOW


#if 0
#ifdef PS2_DRIVER_BUSYWAIT
#   define PS2_CLOCK_PIN   D0
#   define PS2_DATA_PIN    D1
#endif
#ifdef PS2_DRIVER_INTERRUPT
#define PS2_CLOCK_PIN   D2
#define PS2_DATA_PIN    D5

#define PS2_INT_INIT()  do {    \
    EICRA |= ((1<<ISC21) |      \
              (0<<ISC20));      \
} while (0)
#define PS2_INT_ON()  do {      \
    EIMSK |= (1<<INT2);         \
} while (0)
#define PS2_INT_OFF() do {      \
    EIMSK &= ~(1<<INT2);        \
} while (0)
#define PS2_INT_VECT   INT2_vect
#endif
#endif

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

//
//#define AZOTEQ_IQS5XX_TAP_ENABLE false
#define AZOTEQ_IQS5XX_TWO_FINGER_TAP_ENABLE false
//#define AZOTEQ_IQS5XX_PRESS_AND_HOLD_ENABLE false
//#define AZOTEQ_IQS5XX_SWIPE_X_ENABLE
//#define AZOTEQ_IQS5XX_SWIPE_Y_ENABLE
#define AZOTEQ_IQS5XX_ZOOM_ENABLE false
//#define AZOTEQ_IQS5XX_SCROLL_ENABLE false
//
//#define AZOTEQ_IQS5XX_TAP_TIME
//#define AZOTEQ_IQS5XX_TAP_DISTANCE
//#define AZOTEQ_IQS5XX_HOLD_TIME
//#define AZOTEQ_IQS5XX_SWIPE_INITIAL_TIME
//#define AZOTEQ_IQS5XX_SWIPE_INITIAL_DISTANCE
//#define AZOTEQ_IQS5XX_SWIPE_CONSECUTIVE_TIME
//#define AZOTEQ_IQS5XX_SWIPE_CONSECUTIVE_DISTANCE
//#define AZOTEQ_IQS5XX_SCROLL_INITIAL_DISTANCE
//#define AZOTEQ_IQS5XX_ZOOM_INITIAL_DISTANCE
//#define AZOTEQ_IQS5XX_ZOOM_CONSECUTIVE_DISTANCE
//
//#define AZOTEQ_IQS5XX_ROTATION_90
//#define AZOTEQ_IQS5XX_ROTATION_180
//#define AZOTEQ_IQS5XX_ROTATION_270