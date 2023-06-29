float run = 0;  // used to save the time when the motor starts
void MotorLoop() {
  //Enable motor
  while (1) {
    if (Y_As) {  // if Y axis messurement the pins will be change for the Y axis.
      enable = enPin_Y;
      diraction = dirPin_Y;
      steps = stepPin_Y;
      Distance = Distance_Y;
      Distance_half = Half_Distance_Y;
      TravelOffsetForward = TravelOffsetForward_Y;
      TravelOffsetBackward = TravelOffsetBackward_Y;
    } else if (X_As) {  // if X axis messurement the pins will be change for the Y axis.
      enable = enPin_X;
      diraction = dirPin_X;
      steps = stepPin_X;
      Distance = Distance_X;
      Distance_half = Half_Distance_X;
      TravelOffsetForward = TravelOffsetForward_X;
      TravelOffsetBackward = TravelOffsetBackward_X;
    }
    digitalWrite(enable, LOW);
    switch (StateM) {
      default:
        analogWrite(steps, 0);  // analogWrite values from 0 to 255
        break;
      case 1:
        //motor runs till it hits the button. This is so that all messurement have the same starting point.
        run = millis();
        while (1) {
          while (digitalRead(S4) == LOW && !overload) {
            digitalWrite(diraction, LOW);
            analogWrite(steps, 10);  // analogWrite values from 0 to 255
          }
          StateM = 2;
          break;
        }
        break;
      case 2:
        //motor wait
        run = millis();
        while (1) {
          while (millis() < run + wait && !overload) {
            digitalWrite(diraction, HIGH);
            analogWrite(steps, 0);  // analogWrite values from 0 to 255
          }
          StateM = 3;
          break;
        }
        break;
      case 3:
        //motor runs forward
        forward = true;
        backward = false;
        run = millis();
        while (1) {
          while (millis() < run + (Distance_half + TravelOffsetForward) && !overload) {
            digitalWrite(diraction, HIGH);
            analogWrite(steps, 10);            // analogWrite values from 0 to 255
            draw = true;                       // use to draw graph
            if (millis() > 100) { LCLoop(); }  //take messurment every cycle.
          }
          StateM = 4;
          break;
        }
        break;
      case 4:
        //motor wait for 5s
        run = millis();
        while (1) {
          while (millis() < run + (wait * 5) && !overload) {
            digitalWrite(diraction, HIGH);
            analogWrite(steps, 0);  // analogWrite values from 0 to 255
          }
          if (valid) {  // if the messurement is valid, a checkmark will be shown on screen.
            tft.fillRect(350, 50, checkMarkWidth, checkMarkHeight, TFT_BLACK);
            drawIcon(checkMark, 350, 50, checkMarkWidth, checkMarkHeight);
            valid = false;
            textValidForward = "VALID";  //valid will be written on the sd card.
          } else if (!valid) {           // if the messurement is invalid, a crossmark will be shown on screen.
            tft.fillRect(350, 50, crossMarkWidth, crossMarkHeight, TFT_BLACK);
            drawIcon(crossMark, 350, 50, crossMarkWidth, crossMarkHeight);
            valid = false;
            textValidForward = "INVALID";  //invalid will be written on the sd card.
          }
          textPeakForward = "Peak forward: " + String(peakValueForward, 3);//peakforward will be written on the sd card.
          //reset graphline for backward messurement.
          StartT = 0;
          save = false;
          StateM = 5;
          break;
        }
        break;
      case 5:
        //motor runs backward
        forward = false;
        backward = true;
        run = millis();
        while (1) {
          while (millis() < run + (Distance_half + TravelOffsetBackward) && !overload) {
            digitalWrite(diraction, LOW);
            analogWrite(steps, 10);            // analogWrite values from 0 to 255
            draw = true;                       // use to draw graph
            if (millis() > 100) { LCLoop(); }  //take messurment for forward offset every cycle, excluding the first jerk motion
          }
          StateM = 6;
          break;
        }
        break;
      case 6:
        //motor wait
        run = millis();
        while (1) {
          while (millis() < run + wait && !overload) {
            backward = true;
            digitalWrite(diraction, HIGH);
            analogWrite(steps, 0);  // analogWrite values from 0 to 255
          }
          StateM = 7;
          break;
        }
        break;
      case 7:
        //motor runs half way forward
        run = millis();
        while (1) {
          while (millis() < run + (Distance_half + TravelOffsetForward) && !overload) {
            digitalWrite(diraction, HIGH);
            analogWrite(steps, 10);  // analogWrite values from 0 to 255
          }
          if (valid) {
            tft.fillRect(350, 200, checkMarkWidth, checkMarkHeight, TFT_BLACK);
            drawIcon(checkMark, 350, 200, checkMarkWidth, checkMarkHeight);
            valid = false;
            textValidBackward = "VALID";
          } else if (!valid) {
            tft.fillRect(350, 200, crossMarkWidth, crossMarkHeight, TFT_BLACK);
            drawIcon(crossMark, 350, 200, crossMarkWidth, crossMarkHeight);
            valid = false;
            textValidBackward = "INVALID";
          }
          textPeakBackward = "Peak backward: " + String(peakValueBackward, 3);

          tft.setFreeFont(GLCD);
          // when messurement is done, this text will be shown on screen and when button is pressed itll go to the next case.
          tft.drawString("Messurement complete, press     to continue", 200, 320, 2);
          int temp = tft.textWidth("Messurement complete, press  ", 2) + 193;
          for (int i = 0; i < 3; i++) {
            tft.drawLine(temp + i, 310, temp + 10 + i, 320, TFT_GREEN);
          }
          for (int i = 9; i < 12; i++) {
            tft.drawLine(temp + i, 320, temp + 15 + i, 300, TFT_GREEN);
          }
          break;
        }
        StateM = 0;
        break;
    }
    return;
  }
}