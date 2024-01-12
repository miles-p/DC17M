// Define constants for pin connections
const int switchPin = 4;      // Pin connected to the switch
const int ledPin = 6;         // Pin connected to the LED
const int magPin = 10;        // Pin connected to the magazine sensor
const short rx = 11;          // RX pin for serial communication
const short tx = 12;          // TX pin for serial communication

// Variables for switch and magazine states
int switchState = 0;           // Variable to store the current state of the switch
int lastMagState;              // Variable to store the previous state of the magazine
int lastSwitchStateSw = 0;     // Variable to store the previous state of the switch
int lastSwitchStateMag = 0;    // Variable to store the previous state of the magazine

// Variables for switch debouncing
unsigned long lastDebounceTimeSw = 0;  // Last time the switch state changed
unsigned long lastDebounceTimeMag = 0; // Last time the magazine state changed
unsigned long debounceDelay = 50;      // Debounce time in milliseconds

// Variables for various states and timings
bool isHolding;
bool isMagazine;
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
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "SerialMP3Player.h"
#define NO_SERIALMP3_DELAY 1

// Initialize objects
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
SerialMP3Player mp3(rx,tx);

void setup() {
  // Initialize components and set initial values
  alpha4.begin(0x70);  // pass in the address
  mp3.begin(9600);
  mp3.sendCommand(CMD_SEL_DEV, 0, 2);
  pinMode(switchPin, INPUT_PULLUP);  // Use internal pull-up resistor for the switch
  pinMode(ledPin, OUTPUT);
  pinMode(magPin, INPUT_PULLUP);
  Serial.begin(9600);
  bulletCount = bulletRefresh;
  alpha4.writeDigitAscii(0,'0');
  alpha4.writeDigitAscii(1,'3');
  alpha4.writeDigitAscii(2,'0');
  alpha4.writeDigitAscii(3,'0');
  alpha4.writeDisplay();
}

// Function to simulate a single blaster shot
void blasterShot() {
  mp3.play();
  digitalWrite(ledPin, HIGH);
  delay(50);
  bulletCount -= 1;
  digitalWrite(ledPin, LOW);
}

// Function to simulate continuous blaster shots
void constantShot() {
  digitalWrite(ledPin, HIGH);
  delay(fireSpeed);
  bulletCount -= 1;
  digitalWrite(ledPin, LOW);
  delay(fireSpeed);
}

// Function to split the digits of a number
int splitDigits(int input) {
  dig1 = '0' + (input / 100);
  dig2 = '0' + ((input / 10) % 10);
  dig3 = '0' + (input % 10);
}

// Function to reload the blaster
void reload() {
  bulletCount = bulletRefresh;
}

void loop() {
  // Clear the display and reset states
  alpha4.clear(); 
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
    lastSwitchStateMag == LOW; // Note: This line should be an assignment (=) not a comparison (==)
  }

  if ((millis() - lastDebounceTimeSw) > debounceDelay) {
    if (readingSw != switchState) {
      switchState = readingSw;

      // Trigger a blaster shot if conditions are met
      if (switchState == HIGH && isHolding == false && bulletCount > 0 && isMagazine && millis() > 500) {
        blasterShot();
      }
    }
  }

  // Continuous blaster shot when the switch is held
  if (lastSwitchStateSw == LOW && readingSw == LOW && millis() - timeLastPressedSw > holdTime && bulletCount > 0 && isMagazine) {
    isHolding = true;
    constantShot();
  }

  // Update and display bullet count
  splitDigits(bulletCount);
  Serial.println(dig1);
  Serial.println(dig2);
  Serial.println(dig3);
  alpha4.writeDigitAscii(1, dig1);
  alpha4.writeDigitAscii(2, dig2);
  alpha4.writeDigitAscii(3, dig3); 
  lastSwitchStateSw = readingSw;
  lastSwitchStateMag = readingMag;
  alpha4.writeDisplay();
}