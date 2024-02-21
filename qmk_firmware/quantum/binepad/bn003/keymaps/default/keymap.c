#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_0,
        KC_1,
        KC_2
    ),
    [1] = LAYOUT(
        KC_NO,
        KC_NO,
        KC_NO
    ),
    [2] = LAYOUT(
        KC_NO,
        KC_NO,
        KC_NO
    )
};

#if 1
void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  //debug_keyboard=true;
  //debug_mouse=true;
}
#endif
#if 0
void pointing_device_init(void) {

        pointing_device_driver.init();
#ifdef POINTING_DEVICE_MOTION_PIN
#    ifdef POINTING_DEVICE_MOTION_PIN_ACTIVE_LOW
        setPinInputHigh(POINTING_DEVICE_MOTION_PIN);
#    else
        setPinInput(POINTING_DEVICE_MOTION_PIN);
#    endif
#endif

    pointing_device_init_kb();
    pointing_device_init_user();
}
#endif

#if 0
void azoteq_iqs5xx_init(void) {
    i2c_init();
    azoteq_iqs5xx_wake();
    azoteq_iqs5xx_reset_suspend(true, false, true);
    wait_ms(100);
    azoteq_iqs5xx_wake();
    if (azoteq_iqs5xx_get_product() != AZOTEQ_IQS5XX_UNKNOWN) {
        azoteq_iqs5xx_setup_resolution();
        azoteq_iqs5xx_init_status = azoteq_iqs5xx_set_report_rate(AZOTEQ_IQS5XX_REPORT_RATE, AZOTEQ_IQS5XX_ACTIVE, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_event_mode(false, false);
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_reati(true, false);
#    if defined(AZOTEQ_IQS5XX_ROTATION_90)
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_xy_config(false, true, true, true, false);
#    elif defined(AZOTEQ_IQS5XX_ROTATION_180)
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_xy_config(true, true, false, true, false);
#    elif defined(AZOTEQ_IQS5XX_ROTATION_270)
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_xy_config(true, false, true, true, false);
#    else
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_xy_config(false, false, false, true, false);
#    endif
        azoteq_iqs5xx_init_status |= azoteq_iqs5xx_set_gesture_config(true);
        wait_ms(AZOTEQ_IQS5XX_REPORT_RATE + 1);
    }
};
#endif