// Define constants for pin connections
const int switchPin = 4;      // Pin connected to the switch
const int ledPin = 6;         // Pin connected to the LED
const int magPin = 10;        // Pin connected to the magazine sensor
const short rx = 11;          // RX pin for serial communication
const short tx = 12;          // TX pin for serial communication

// Variables for switch and magazine states
int switchState = 0;           // Current state of the switch
int lastMagState;              // Previous state of the magazine
int lastSwitchStateSw = 0;     // Previous state of the switch
int lastSwitchStateMag = 0;    // Previous state of the magazine
int lastHoldingState = 0;

// Variables for switch debouncing
unsigned long lastDebounceTimeSw = 0;  // Last time the switch state changed
unsigned long lastDebounceTimeMag = 0; // Last time the magazine state changed
unsigned long debounceDelay = 50;      // Debounce time in milliseconds

// Variables for various states and timings
bool isHolding;
bool isMagazine;
bool constFiring;
unsigned long timeLastPressedSw;   // Needs to be long to handle millis() overflow
int holdTime = 200;
int bulletRefresh = 300;
int bulletCount;
int fireSpeed = 40;
char dig1;
char dig2;
char dig3;

// Include necessary libraries
#include <Wire.h>
#include "HT16K33.h"
#include "SerialMP3Player.h"
#define NO_SERIALMP3_DELAY 1

// Initialize objects
HT16K33 seg(0x70);
SerialMP3Player mp3(rx, tx);

void setup() {
  // Initialize components and set initial values
  Wire.begin();
  Wire.setClock(100000);
  seg.begin();
  seg.displayColon(0);
  seg.cacheOn();
  setDisplay(bulletRefresh);
  constFiring = false;
  mp3.begin(9600);
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);
  pinMode(switchPin, INPUT_PULLUP);  // Use internal pull-up resistor for the switch
  pinMode(ledPin, OUTPUT);
  pinMode(magPin, INPUT_PULLUP);
  Serial.begin(9600);
  bulletCount = bulletRefresh;
}

void setDisplay(int bullets) {
  seg.displayInt(bullets);
}

// Function to simulate a single blaster shot
void blasterShot() {
  mp3.play(1);
  delay(5);
  digitalWrite(ledPin, HIGH);
  delay(50);
  bulletCount -= 1;
  setDisplay(bulletCount);
  digitalWrite(ledPin, LOW);
}

// Function to simulate continuous blaster shots
void constantShot() {
  digitalWrite(ledPin, HIGH);
  delay(fireSpeed);
  bulletCount -= 1;
  setDisplay(bulletCount);
  digitalWrite(ledPin, LOW);
  delay(fireSpeed);
}

// Function to reload the blaster
void reload() {
  bulletCount = bulletRefresh;
  setDisplay(bulletRefresh);
}

void loop() {
  // Clear the display and reset states
  isHolding = false;
  int readingSw = digitalRead(switchPin);
  int readingMag = digitalRead(magPin);

  // Debouncing the switch
  if (readingSw != lastSwitchStateSw) {
    lastDebounceTimeSw = millis();
    if (readingSw == LOW) {
      timeLastPressedSw = millis();
    }
    if (readingSw == HIGH) {
      isHolding = false;
    }
  }

  // Check magazine state
  if (readingMag == HIGH) {
    isMagazine = false;
    bulletCount = 0;
  } else {
    isMagazine = true;
  }

  // Reload when the magazine is inserted
  if (readingMag == LOW && lastSwitchStateMag == HIGH) {
    reload();
    lastSwitchStateMag = LOW; // Fixed assignment
  }

  if ((millis() - lastDebounceTimeSw) > debounceDelay) {
    if (readingSw != switchState) {
      switchState = readingSw;

      // Trigger a blaster shot if conditions are met
      if (switchState == HIGH && !isHolding && bulletCount > 0 && isMagazine && millis() > 500) {
        blasterShot();
      }
    }
  }

  // Continuous blaster shot when the switch is held
  if (lastSwitchStateSw == LOW && readingSw == LOW && millis() - timeLastPressedSw > holdTime && bulletCount > 0 && isMagazine) {
    isHolding = true;
    constantShot();
    if (!constFiring) {
      constFiring = true;
      //mp3.play(2);
    }
  }

  if (lastHoldingState != isHolding) {
    if (isHolding == 0) {
      Serial.println("DOWN");
      mp3.pause();
      lastHoldingState = 0;
    }
    if (isHolding == 1) {
      Serial.println("UP");
      mp3.play(2);
      lastHoldingState = 1;
    }
  }

  // Update and display bullet count
  lastSwitchStateSw = readingSw;
  lastSwitchStateMag = readingMag;
  lastHoldingState = isHolding;
}
