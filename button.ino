// #########################################################################
//  Variable button
// #########################################################################
int buttonState1 = 0;     // variable for reading the pushbutton status
int buttonState2 = 0;     // variable for reading the Downbutton status
int buttonState3 = 0;     // variable for reading the Upbutton status


//Debounce time  and hold time button
unsigned long lastDebounceTime1 = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTime2 = 0;  // the last time the output pin was toggled
unsigned long lastDebounceTime3 = 0;  // the last time the output pin was toggled


// #########################################################################
//  Setup for check button
// #########################################################################
bool checkButton() {
  int reading1 = analogRead(buttonPin1);
  bool check = false;
  if (reading1 != lastButtonState1) {
    lastDebounceTime1 = millis();
  }
  if ((millis() - lastDebounceTime1) > debounceDelay) {

    if (reading1 != buttonState1) {
      buttonState1 = reading1;

      if (buttonState1 == LOW) {
        Serial.print("checkbutton");
        Serial.println(checkButton());
        check = true;
      }
    }
  }
  lastButtonState1 = reading1;
  return check;
}
// #########################################################################
//  Setup for up button
// #########################################################################

bool upButton() {
  int reading3 = analogRead(buttonPin3);
  bool up = false;
  if (reading3 != lastButtonState3) {
    lastDebounceTime3 = millis();
  }

  if ((millis() - lastDebounceTime3) > debounceDelay) {

    if (reading3 != buttonState3) {
      buttonState3 = reading3;

      if (buttonState3 == LOW) {
        up = true;
        Serial.print("upButton");
        Serial.println(upButton());
      }
    }
  }
  lastButtonState3 = reading3;
  return up;
}

// #########################################################################
//  Setup for down button
// #########################################################################

bool downButton() {
  int reading2 = analogRead(buttonPin2);
  bool down = false;
  if (reading2 != lastButtonState2) {
    lastDebounceTime2 = millis();
  }
  if ((millis() - lastDebounceTime2) > debounceDelay) {

    if (reading2 != buttonState2) {
      buttonState2 = reading2;

      if (buttonState2 == LOW) {
        down = true;
        Serial.print("downButton");
        Serial.println(downButton());
      }
    }
  }
  lastButtonState2 = reading2;
  return down;
}
