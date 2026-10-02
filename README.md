# Cat Player Lite — under-$30 revision

This version preserves the cat enclosure, OLED, previous/next controls, volume knob, play/pause, speaker, and removable microSD music library while keeping the hardware subtotal below **$30 USD**.

## Target cost

- Planned parts subtotal: **$25.64**
- Uncommitted contingency: **$4.36**
- Hard cap: **$30.00**

Shipping and sales tax are not included because they depend on location. To keep the actual checkout below $30, buy the small generic parts from one seller or use switches, wire, a USB cable, and a microSD card already on hand.

## Removed from the first build

- camera;
- microphone/recording;
- headphone output;
- battery and charging system;
- color OLED;
- custom PCB;
- Linux/cyberdeck capability.

These are later upgrades. Trying to include them now would either exceed the budget or make the first build unnecessarily fragile.

## Core architecture

```text
USB-C power
   │
   ├── XIAO ESP32-C3 ── I2C ── 0.96-inch OLED
   │        │             └──── MCP23017 controls
   │        └── UART ────────── DFPlayer Mini
   │
   └── DFPlayer Mini ── microSD + MP3 decoder + 3 W speaker output
```

The DFPlayer handles the microSD card, MP3 decoding, and speaker amplification. The ESP32-C3 only manages the screen and controls, which keeps the firmware simple.

## Files

- `BOM_UNDER_30.csv` — item-by-item spending caps.
- `WIRING.md` — complete wiring and safe assembly order.
- `enclosure/cat_player_lite_enclosure.scad` — smaller two-piece case.
- `firmware/cat_player_lite.ino` — starter player firmware.

## Price references checked 2026-10-01

- XIAO ESP32-C3: https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html
- DFPlayer Mini: https://www.dfrobot.com/product-1121.html
- DFPlayer documentation: https://wiki.dfrobot.com/dfr0299/docs/20906

The enclosure is original and derived from the supplied sketch. It does not copy an existing commercial enclosure.
