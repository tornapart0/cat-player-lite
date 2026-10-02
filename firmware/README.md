# Firmware setup

Install the current ESP32 Arduino board package, select **XIAO_ESP32C3**, and install:

- DFRobotDFPlayerMini;
- Adafruit GFX Library;
- Adafruit SSD1306;
- Adafruit MCP23017 Arduino Library.

Upload `cat_player_lite.ino` and open Serial Monitor at 115200 baud.

The code expects:

- SSD1306 address `0x3C`;
- MCP23017 address `0x20`;
- DFPlayer UART at 9600 baud;
- tracks named `/mp3/0001.mp3`, `/mp3/0002.mp3`, and so on.

The sketch is intentionally direct and small: the encoder changes volume, its push switch toggles play/pause, and the two face buttons select previous/next.
