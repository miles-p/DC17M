const int switchPin = 4;             // Pin connected to the switch
const int ledPin = 6;                // Pin connected to the LED
const int magPin = 10;
int switchState = 0;                 // Variable to store the current state of the switch
int lastMagState;
int lastSwitchStateSw = 0;             // Variable to store the previous state of the switch
int lastSwitchStateMag = 0;
unsigned long lastDebounceTimeSw = 0;  // Last time the switch state changed
unsigned long lastDebounceTimeMag = 0;
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
bool isHolding;
bool isMagazine;
unsigned long timeLastPressedSw;  // needs to be long otherwise the millis function overloads and funky shit starts happening (this happens as the time hits the 16 bit int def. When it goes to long, this limit is raised to 32 bit)
int holdTime = 200;
int bulletRefresh = 300;
int bulletCount;
int fireSpeed = 40;
char dig1;
char dig2;
char dig3;

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  alpha4.begin(0x70);  // pass in the address
  pinMode(switchPin, INPUT_PULLUP);  // Use internal pull-up resistor
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

void blasterShot() {
  // Simulate the blaster shot
  digitalWrite(ledPin, HIGH);
  delay(50);  // Adjust the delay for the LED on time to control shot duration
  bulletCount -= 1;
  digitalWrite(ledPin, LOW);
}
void constantShot() {
  digitalWrite(ledPin, HIGH);
  delay(fireSpeed);  // Adjust the delay for the LED on time to control shot duration
  bulletCount -= 1;
  digitalWrite(ledPin, LOW);
  delay(fireSpeed);
}
int splitDigits(int input) {
  //Serial.println(bulletCount / 100); // 1nd Digit
  //Serial.println((bulletCount / 10) % 10); // 2nd Digit
  //Serial.println(bulletCount % 10); // 3rd Digit
  dig1 = '0' + (input / 100);
  dig2 = '0' + ((input / 10) % 10);
  dig3 = '0' + (input % 10);
}
void reload() {
  bulletCount = bulletRefresh;
}

void loop() {
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
    //Serial.println("Finally high "+millis());
    if (readingSw == HIGH) {
      isHolding = false;
    }
  }

  if (readingMag == HIGH) {
    isMagazine = false;
    bulletCount = 0;
  } else {
    isMagazine = true;
  }

if (readingMag == LOW && lastSwitchStateMag == HIGH) {
  reload();
  lastSwitchStateMag == LOW;
}

  if ((millis() - lastDebounceTimeSw) > debounceDelay) {
    // If the switch state has changed, update the last switch state
    if (readingSw != switchState) {
      switchState = readingSw;

      // If the switch is pressed, simulate blaster shots
      if (switchState == HIGH && isHolding == false && bulletCount > 0 && isMagazine && millis() > 500) {
        // Perform single shot action here
        blasterShot();
      }
    }
  }
  if (lastSwitchStateSw == LOW && readingSw == LOW && millis() - timeLastPressedSw > holdTime && bulletCount > 0 && isMagazine) {
    isHolding = true;
    constantShot();
  }
  splitDigits(bulletCount);
  Serial.println(dig1);
  Serial.println(dig2);
  Serial.println(dig3);
  alpha4.writeDigitAscii(1,dig1);
  alpha4.writeDigitAscii(2,dig2);
  alpha4.writeDigitAscii(3,dig3); 
  lastSwitchStateSw = readingSw;
  lastSwitchStateMag = readingMag;
  alpha4.writeDisplay();
}
