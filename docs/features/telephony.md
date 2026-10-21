# Telephony {#telephony}

This feature adds a HID interface on the Telephony usage page,
which conferencing apps use to talk to headsets:
the keyboard can answer, hang up and mute calls,
and the host reports the call state (muted, off hook, ringing, …) back through LEDs.

For example, Google Meet supports such devices through WebHID in Chromium-based browsers,
once access to the device has been granted in the browser's permission prompt.

## Usage {#usage}

Add the following to your `rules.mk`:

```make
TELEPHONY_ENABLE = yes
```

::: warning
This feature is only available with LUFA and ChibiOS, not V-USB.
It uses a dedicated endpoint, so on MCUs with few endpoints it may not fit alongside other features.
:::

## Buttons {#buttons}

The input report is a bitmask of the following buttons:

|Bit                      |Telephony Usage|
|-------------------------|---------------|
|`TELEPHONY_HOOK_SWITCH`  |Hook Switch    |
|`TELEPHONY_FLASH`        |Flash          |
|`TELEPHONY_HOLD`         |Hold           |
|`TELEPHONY_REDIAL`       |Redial         |
|`TELEPHONY_DROP`         |Drop           |
|`TELEPHONY_SPEAKER_PHONE`|Speaker Phone  |
|`TELEPHONY_PHONE_MUTE`   |Phone Mute     |

Each bit is a level, not an event.
Hook Switch and Phone Mute are on/off controls:
set Hook Switch to answer or stay in a call and clear it to hang up,
set Phone Mute to mute and clear it to unmute.
Momentary buttons such as Flash, Redial and Drop are set on press and cleared on release.
Which usages are honored depends on the app.

## LEDs {#leds}

The host sets the following bits in the output report:

|Bit                            |LED Usage      |
|-------------------------------|---------------|
|`TELEPHONY_LED_MUTE`           |Mute           |
|`TELEPHONY_LED_OFF_HOOK`       |Off-Hook       |
|`TELEPHONY_LED_RING`           |Ring           |
|`TELEPHONY_LED_MESSAGE_WAITING`|Message Waiting|
|`TELEPHONY_LED_SPEAKER`        |Speaker        |
|`TELEPHONY_LED_HEAD_SET`       |Head Set       |
|`TELEPHONY_LED_HOLD`           |Hold           |
|`TELEPHONY_LED_MICROPHONE`     |Microphone     |

## Example {#example}

This example adds keys to toggle mute and to answer or hang up,
and shows the mute state on an LED.
Since the input report carries the desired state of both controls,
it starts from the state reported by the host,
so that muting doesn't hang up and vice versa:

```c
enum custom_keycodes {
    CALL_MUTE = SAFE_RANGE,
    CALL_HOOK,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode != CALL_MUTE && keycode != CALL_HOOK) {
        return true;
    }
    if (record->event.pressed) {
        uint8_t leds    = host_telephony_leds();
        uint8_t buttons = 0;
        if (leds & TELEPHONY_LED_OFF_HOOK) buttons |= TELEPHONY_HOOK_SWITCH;
        if (leds & TELEPHONY_LED_MUTE) buttons |= TELEPHONY_PHONE_MUTE;
        buttons ^= keycode == CALL_MUTE ? TELEPHONY_PHONE_MUTE : TELEPHONY_HOOK_SWITCH;
        host_telephony_send(buttons);
    }
    return false;
}

void housekeeping_task_user(void) {
    gpio_write_pin(MUTE_LED_PIN, host_telephony_leds() & TELEPHONY_LED_MUTE);
}
```

## API {#api}

### `void host_telephony_send(uint8_t buttons)` {#api-host-telephony-send}

Send an input report to the host.

#### Arguments {#api-host-telephony-send-arguments}

 - `uint8_t buttons`  
   A bitmask of [buttons](#buttons).

---

### `uint8_t host_telephony_leds(void)` {#api-host-telephony-leds}

Get the most recent output report from the host.

#### Return Value {#api-host-telephony-leds-return}

A bitmask of [LEDs](#leds).
