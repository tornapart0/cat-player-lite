# Cat Player Lite wiring

## XIAO ESP32-C3 to DFPlayer Mini

| XIAO | DFPlayer | Purpose |
|---|---|---|
| `5V` | `VCC` | USB-derived power |
| `GND` | `GND` | Common ground |
| `D2 / GPIO4` | `RX` through 1 kΩ | Commands from XIAO |
| `D3 / GPIO5` | `TX` | Status back to XIAO |

The 1 kΩ series resistor on the DFPlayer RX connection is inexpensive insurance against noise and level-related instability.

## I2C bus

| XIAO | OLED | MCP23017 |
|---|---|---|
| `3V3` | `VCC` | `VCC` |
| `GND` | `GND` | `GND` |
| `D4 / GPIO6` | `SDA` | `SDA` |
| `D5 / GPIO7` | `SCL` | `SCL` |

Set the MCP23017 address pins `A0`, `A1`, and `A2` low for address `0x20`. Use only one set of I2C pull-ups; most breakout boards and OLED modules already include them.

## Controls on MCP23017

Every switch connects its input to ground when active. Firmware enables the pull-ups.

| MCP pin | Control |
|---|---|
| `GPA0` | Previous button |
| `GPA1` | Next button |
| `GPA2` | Encoder A |
| `GPA3` | Encoder B |
| `GPA4` | Encoder push switch — play/pause |

## Speaker

Connect the speaker only between DFPlayer `SPK1` and `SPK2`. Neither speaker lead goes to ground because this is a bridge-tied output.

Do not connect headphones to `SPK1`/`SPK2`. Headphones were removed from the budget revision.

## microSD layout

1. Format the card as FAT32.
2. Create a folder named `mp3` at the card root.
3. Name tracks `0001.mp3`, `0002.mp3`, and so on.
4. On macOS, remove `._` metadata files before ejecting the card; DFPlayer may mistake them for tracks.

## Assembly order

1. Test the DFPlayer, speaker, and microSD by themselves.
2. Add the XIAO UART connection and verify next/previous commands.
3. Add the OLED.
4. Add the MCP23017 and controls.
5. Transfer the working circuit to perfboard.
6. Print the enclosure fit-check section before the full case.

Stay on USB power for this revision. A battery and charger consume most of the remaining contingency and introduce a more serious safety requirement.
