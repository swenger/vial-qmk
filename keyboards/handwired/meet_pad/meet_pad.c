#include QMK_KEYBOARD_H

void keyboard_post_init_kb(void) {
    gpio_set_pin_output(LED_MUTE_PIN);
    gpio_set_pin_output(LED_CALL_PIN);
    keyboard_post_init_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) return false;
    if (keycode != MEET_MUTE && keycode != MEET_CALL) return true;

    if (record->event.pressed) {
        // Hook Switch and Phone Mute carry the desired state.
        // Start from the host's state so pressing one key never changes the other,
        // e.g. muting must not report on-hook, which would hang up.
        uint8_t leds    = host_telephony_leds();
        uint8_t buttons = 0;
        if (leds & TELEPHONY_LED_OFF_HOOK) buttons |= TELEPHONY_HOOK_SWITCH;
        if (leds & TELEPHONY_LED_MUTE) buttons |= TELEPHONY_PHONE_MUTE;
        buttons ^= keycode == MEET_MUTE ? TELEPHONY_PHONE_MUTE : TELEPHONY_HOOK_SWITCH;
        host_telephony_send(buttons);
    }
    return false;
}

void housekeeping_task_kb(void) {
    uint8_t leds = host_telephony_leds();
    gpio_write_pin(LED_MUTE_PIN, leds & TELEPHONY_LED_MUTE);
    // Blink at ~2 Hz while ringing.
    gpio_write_pin(LED_CALL_PIN, (leds & TELEPHONY_LED_OFF_HOOK) || ((leds & TELEPHONY_LED_RING) && (timer_read() & 0x100)));
    housekeeping_task_user();
}
