#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_SSD1306.h>
#include <DFRobotDFPlayerMini.h>

namespace Pins {
constexpr uint8_t dfPlayerRx = 5;  // XIAO D3 receives from DFPlayer TX.
constexpr uint8_t dfPlayerTx = 4;  // XIAO D2 sends through 1 kOhm to DFPlayer RX.
constexpr uint8_t i2cSda = 6;      // XIAO D4.
constexpr uint8_t i2cScl = 7;      // XIAO D5.
}  // namespace Pins

namespace Controls {
constexpr uint8_t previous = 0;
constexpr uint8_t next = 1;
constexpr uint8_t encoderA = 2;
constexpr uint8_t encoderB = 3;
constexpr uint8_t encoderPress = 4;
}  // namespace Controls

constexpr uint8_t oledAddress = 0x3C;
constexpr uint8_t mcpAddress = 0x20;
constexpr uint8_t screenWidth = 128;
constexpr uint8_t screenHeight = 64;

HardwareSerial dfSerial(1);
DFRobotDFPlayerMini player;
Adafruit_MCP23X17 controls;
Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, -1);

bool playerReady = false;
bool controlsReady = false;
bool displayReady = false;
bool paused = false;
int volumeLevel = 15;  // DFPlayer range: 0 to 30.
int currentTrack = 1;
int trackCount = 0;
uint8_t lastEncoderState = 0b11;
uint32_t lastInputScanMs = 0;

bool pressed(uint8_t pin) {
  return controls.digitalRead(pin) == LOW;
}

void drawScreen(const char* eventText) {
  if (!displayReady) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("CAT PLAYER LITE");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(0, 17);
  if (!playerReady) {
    display.println("PLAYER NOT FOUND");
  } else {
    display.printf("TRACK %04d", currentTrack);
    if (trackCount > 0) {
      display.printf(" / %d", trackCount);
    }
    display.println();
    display.printf("VOLUME %d / 30\n", volumeLevel);
    display.println(paused ? "PAUSED" : "PLAYING");
  }

  display.setCursor(0, 54);
  display.print(eventText);
  display.display();
}

void selectPreviousTrack() {
  if (!playerReady) return;
  player.previous();
  currentTrack = max(1, currentTrack - 1);
  paused = false;
  drawScreen("PREVIOUS");
}

void selectNextTrack() {
  if (!playerReady) return;
  player.next();
  currentTrack = trackCount > 0 ? min(trackCount, currentTrack + 1) : currentTrack + 1;
  paused = false;
  drawScreen("NEXT");
}

void togglePause() {
  if (!playerReady) return;
  paused = !paused;
  if (paused) {
    player.pause();
  } else {
    player.start();
  }
  drawScreen(paused ? "PAUSE" : "PLAY");
}

void scanControls() {
  static bool lastPrevious = false;
  static bool lastNext = false;
  static bool lastPress = false;
  static int8_t encoderAccumulator = 0;
  static const int8_t transitionDelta[16] = {
      0, -1, 1, 0,
      1, 0, 0, -1,
      -1, 0, 0, 1,
      0, 1, -1, 0};

  const bool nowPrevious = pressed(Controls::previous);
  const bool nowNext = pressed(Controls::next);
  const bool nowPress = pressed(Controls::encoderPress);

  if (nowPrevious && !lastPrevious) selectPreviousTrack();
  if (nowNext && !lastNext) selectNextTrack();
  if (nowPress && !lastPress) togglePause();

  lastPrevious = nowPrevious;
  lastNext = nowNext;
  lastPress = nowPress;

  const uint8_t encoderState =
      (controls.digitalRead(Controls::encoderA) << 1) |
      controls.digitalRead(Controls::encoderB);
  const uint8_t transition = (lastEncoderState << 2) | encoderState;
  encoderAccumulator += transitionDelta[transition];
  lastEncoderState = encoderState;

  if (encoderAccumulator >= 4 || encoderAccumulator <= -4) {
    const int8_t detent = encoderAccumulator > 0 ? 1 : -1;
    encoderAccumulator = 0;
    volumeLevel = constrain(volumeLevel + detent, 0, 30);
    if (playerReady) {
      player.volume(volumeLevel);
    }
    drawScreen(detent > 0 ? "VOLUME UP" : "VOLUME DOWN");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin(Pins::i2cSda, Pins::i2cScl);
  displayReady = display.begin(SSD1306_SWITCHCAPVCC, oledAddress);
  if (!displayReady) {
    Serial.println("OLED not found");
  }

  controlsReady = controls.begin_I2C(mcpAddress, &Wire);
  if (controlsReady) {
    for (uint8_t pin = Controls::previous; pin <= Controls::encoderPress; ++pin) {
      controls.pinMode(pin, INPUT_PULLUP);
    }
    lastEncoderState =
        (controls.digitalRead(Controls::encoderA) << 1) |
        controls.digitalRead(Controls::encoderB);
  }

  dfSerial.begin(9600, SERIAL_8N1, Pins::dfPlayerRx, Pins::dfPlayerTx);
  playerReady = player.begin(dfSerial, true, true);
  if (playerReady) {
    player.volume(volumeLevel);
    trackCount = player.readFileCounts();
    player.playMp3Folder(currentTrack);
  }

  if (displayReady) {
    drawScreen(playerReady ? "READY" : "CHECK PLAYER");
  }

  Serial.printf("DFPlayer: %s\n", playerReady ? "OK" : "not found");
  Serial.printf("Controls: %s\n", controlsReady ? "OK" : "not found");
  Serial.printf("Tracks: %d\n", trackCount);
}

void loop() {
  if (controlsReady && millis() - lastInputScanMs >= 5) {
    lastInputScanMs = millis();
    scanControls();
  }

  if (playerReady && player.available()) {
    const uint8_t eventType = player.readType();
    const int eventValue = player.read();
    Serial.printf("DFPlayer event %u value %d\n", eventType, eventValue);

    if (eventType == DFPlayerPlayFinished && !paused) {
      selectNextTrack();
    }
  }
}
