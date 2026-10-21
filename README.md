# Cat Player Lite — Music macro Pad

A cat-shaped desktop MP3 player with a screen, two track buttons, and a rotary volume knob. This is the budget revision of my Music macro Pad project: the XIAO handles controls and display, while a DFPlayer Mini reads music from microSD and drives a speaker. The target is a USB-powered perfboard build in a printed enclosure.

## Features and controls

- Previous and next buttons select tracks.
- Turning the encoder changes volume from 0 to 30; pressing it toggles play/pause.
- A 128 × 64 SSD1306 OLED shows the track number, volume, playback state, and last action. It does not read song-title metadata.
- An MCP23017 provides five control inputs over I2C; the firmware enables input pull-ups.
- The DFPlayer handles MP3 decoding and speaker output. The firmware attempts to advance when a track finishes.

The supplied revision has no Bluetooth audio receiver, headphone output, microphone, website uploader, battery, or AI assistant. Those appeared in earlier ideas or prototype experiments, but are not implemented in this firmware. The October 4 journal describes a borrowed-parts Bluetooth experiment; it is not validation of this XIAO ESP32-C3 / DFPlayer configuration.

## Electronics

![Electronics wiring plan](docs/images/electronics-wiring.png)

This is a wiring plan, not a manufactured PCB or an assembled-board photograph. Follow [WIRING.md](WIRING.md) for all connections and staged assembly. XIAO D4/GPIO6 and D5/GPIO7 carry I2C to the OLED at `0x3C` and MCP23017 at `0x20`. XIAO D2/GPIO4 sends UART commands through 1 kΩ to DFPlayer RX; D3/GPIO5 receives DFPlayer TX. DFPlayer uses USB-derived 5 V; the OLED and expander use 3.3 V. Share ground.

The speaker connects **between SPK1 and SPK2**. Neither lead goes to ground; do not attach headphones to these outputs. Start at low volume. Bench-check power rails and module pin labels before soldering.

## Enclosure and CAD

![Enclosure concept blueprint](enclosure/cat_player_lite_blueprint.svg)

![Preview of exported STEP enclosure geometry](enclosure/cat_player_lite_step_preview.png)

The body is 116 mm wide, with an 84 mm elliptical head and ears reaching y = 64 mm. Wall thickness is 2.2 mm; front and rear depths are 8 mm and 16 mm. Openings include the 23 × 13 mm OLED window, two 10.5 mm button apertures, a 7.4 mm encoder shaft hole, speaker grille, USB access, and microSD access. Four screw positions join the shells.

- [OpenSCAD source](enclosure/cat_player_lite_enclosure.scad) — select `front`, `back`, `fit_check`, or `assembly`.
- [Front shell STEP](enclosure/cat_player_lite_front.step)
- [Rear shell STEP](enclosure/cat_player_lite_back.step)
- [Exploded assembly STEP](enclosure/cat_player_lite_assembly.step)
- [STEP generator](enclosure/export_step.py) and [geometry validation](enclosure/step-validation.json).
- [Dimensioned SVG blueprint](enclosure/cat_player_lite_blueprint.svg).

The original Shapr3D file was lost; these files derive from the repository's existing OpenSCAD reconstruction. The STEP outline retains its 64-segment profile, with analytic round bores. A 0.5 mm shoulder now connects the rear inset ring to the wall; without it the original ring floated across a 0.35 mm gap. Both sources include that support. The assembly STEP separates the shells by 5 mm for inspection.

These are enclosure shells, not a mechanically validated assembly. Module mounts, OLED active-area fit, button actuators, screw choice, and cable clearance still need measurement. Print the fit-check coupon first; the large face apertures need suitable button caps or an actuator solution for the small tactile switches. Do not assume the current cutouts prove the purchased modules will fit.

## Firmware setup

Open [firmware/cat_player_lite.ino](firmware/cat_player_lite.ino) in Arduino IDE and select **XIAO_ESP32C3** with the ESP32 board package. Install DFRobotDFPlayerMini, Adafruit GFX, Adafruit SSD1306, and Adafruit MCP23017 Arduino Library. The detailed setup is in [firmware/README.md](firmware/README.md).

Format a microSD card FAT32 and put tracks in `/mp3/0001.mp3`, `/mp3/0002.mp3`, etc. Remove macOS `._` metadata files before ejecting. Upload the sketch and open Serial Monitor at 115200 baud; the DFPlayer UART runs at 9600 baud. Check the player, then the UART, display, controls, and finally the combined perfboard build before fitting the enclosure.

## Bill of materials

| Part | Qty | Unit USD | Total USD | Vendor / product link |
| --- | ---: | ---: | ---: | --- |
| Microcontroller — Official XIAO ESP32-C3 price checked 2026-10-01 | 1 | $4.99 | $4.99 | [Seeed Studio](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html) |
| MP3 module — DFPlayer Mini DFR0299 | 1 | $5.90 | $5.90 | [DFRobot](https://www.dfrobot.com/product-1121.html) |
| OLED — Choose a four-pin 0.96 inch 128x64 I2C SSD1306 module | 1 | $3.88 | $3.88 | [AliExpress marketplace](https://www.aliexpress.us/item/1005008516112334.html) |
| I/O expander — MCP23017 I2C breakout | 1 | $5.95 | $5.95 | [Adafruit](https://www.adafruit.com/product/5346) |
| Speaker — 8 ohm 1 watt mini oval speaker | 1 | $1.95 | $1.95 | [Adafruit](https://www.adafruit.com/product/4227) |
| Rotary encoder — Bourns encoder with push switch | 1 | $2.89 | $2.89 | [DigiKey](https://www.digikey.com/en/products/detail/bourns-inc/PEC11R-4315K-S0012/4699233) |
| Momentary buttons — Omron B3F-1000 normally-open tactile switches | 2 | $0.35 | $0.70 | [DigiKey](https://www.digikey.com/en/products/detail/omron-electronics-inc-emc-div/B3F-1000/33150) |
| microSD card — Use an existing FAT32 card preferably 32 GB or smaller | 1 | $0.01 | $0.01 | Reuse existing card |
| Prototype materials — Use existing perfboard wire headers solder and 1k resistor | 1 | $0.01 | $0.01 | Reuse home or school stock |
| Enclosure materials — Use existing filament and four small screws | 1 | $0.01 | $0.01 | Reuse school makerspace stock |
| Parts subtotal | | | **$26.29** | |
| Tax and shipping allowance | | | **$1.84** | Original estimate |
| Total | | | **$28.13** | $1.87 below $30 |

These are the original project estimates, not fresh checkout quotes. The three $0.01 entries are platform placeholders for reused materials, not offers to buy those items for one cent; availability must be checked with the home/school stock owner. Delivery, tax, marketplace variants, and prices may differ. Use [BOM_UNDER_30.csv](BOM_UNDER_30.csv) for the itemized source and [BOM.md](BOM.md) for the Half Life mirror. Buy the four-pin I2C OLED variant, and check module dimensions against the case before purchasing.

## Status and next checks

The firmware, wiring plan, OpenSCAD source, STEP shells, and blueprint are available. STEP files are checked for valid solids and reimported to compare volume with the exported shapes; this does not prove printability or physical fit. This documentation update does not claim a new firmware compile or a working final hardware build. Remaining work includes confirming supplies and checkout costs, measuring actual modules, adding their internal mounts, printing a fit coupon, assembling the intended DFPlayer circuit, and testing playback, controls, and enclosure access. Historical work and screenshots are in [JOURNAL.md](JOURNAL.md).
