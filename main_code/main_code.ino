const int switchPin = 4;  // Pin connected to the switch
const int ledPin = 6;     // Pin connected to the LED

int switchState = 0;      // Variable to store the current state of the switch
int lastSwitchState = 0;  // Variable to store the previous state of the switch
unsigned long lastDebounceTime = 0;  // Last time the switch state changed
unsigned long debounceDelay = 50;    // Debounce time in milliseconds
bool isHolding;
unsigned long timeLastPressed; // needs to be long otherwise the millis function overloads and funky shit stards happening
int holdTime = 250;

void setup() {
  pinMode(switchPin, INPUT_PULLUP);  // Use internal pull-up resistor
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void blasterShot() {
  // Simulate the blaster shot
  digitalWrite(ledPin, HIGH);
  delay(50);  // Adjust the delay for the LED on time to control shot duration
  digitalWrite(ledPin, LOW);
}
void constantShot() {
  digitalWrite(ledPin, HIGH);
  delay(50);  // Adjust the delay for the LED on time to control shot duration
  digitalWrite(ledPin, LOW);
  delay(50);
}

void loop() {
  isHolding = false;
  int reading = digitalRead(switchPin);

  // Debouncing the switch
  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
    if (reading == LOW) {
      timeLastPressed = millis();
    }
      //Serial.println("Finally high "+millis());
    if (reading == HIGH) {
      isHolding = false;
    }
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the switch state has changed, update the last switch state
    if (reading != switchState) {
      switchState = reading;

      // If the switch is pressed, simulate blaster shots
      if (switchState == HIGH && isHolding == false) {
        // Perform single shot action here
        blasterShot();
      }
    }
  }
  if (lastSwitchState == LOW && reading == LOW && millis()-timeLastPressed > holdTime) {
    isHolding = true;
    constantShot();
  }
  // Save the current switch state for comparison in the next iteration
  lastSwitchState = reading;
}
