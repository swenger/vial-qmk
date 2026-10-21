# Meet Pad

A two-key pad that controls Google Meet calls
through [Meet's call control](https://support.google.com/meet/answer/12562325) (WebHID, Chromium browsers only).
It presents itself as a USB HID telephony headset,
so it works while Meet is in a background tab.

* Keyboard Maintainer: you
* Hardware Supported: Raspberry Pi Pico (RP2040)

## Functions

| Key / LED | Behaviour |
|---|---|
| Mute key | Toggles mute |
| Call key | Hangs up; answers while ringing |
| Muted LED | On while muted |
| Call LED | On during a call, blinks while ringing |

Google only documents mute and its LED.
Hang up and the call LED follow the same HID usages as other call-control devices.
Ringing and answering depend on Meet sending the Ring LED report and accepting Hook Switch for incoming calls,
which is untested.

## Wiring

All connections are on physical pins 3–8 of the Pico (top left, USB pointing up).

| Function | GPIO | Physical pin |
|---|---|---|
| Mute switch | GP2 | 4 |
| Call switch | GP3 | 5 |
| Muted LED | GP4 | 6 |
| Call LED | GP5 | 7 |

GND is on physical pins 3 and 8.

* Switches: between the pin and GND (internal pull-ups, no resistors needed).
* LEDs: pin → resistor (330 Ω–1 kΩ at 3.3 V) → LED anode; LED cathode → GND.

## Using it with Meet

1. Plug in the pad.
2. In Meet, open *More options → Settings → Audio → Call control* and click *Connect device*.
3. Select *Meet Pad* in Meet's list and in the browser's WebHID prompt.

## Building and flashing

    make handwired/meet_pad:vial
    make handwired/meet_pad:vial:flash

To enter the bootloader, hold BOOTSEL while plugging in the Pico.
`make … :flash` then copies the firmware to the `RPI-RP2` drive;
alternatively, copy `handwired_meet_pad_vial.uf2` onto it by hand.

The telephony interface is enabled with `TELEPHONY_ENABLE = yes` in `rules.mk`.
