void Startuploop() {
  Y_As = true;                   // startup sequence start first with Y axis.
  for (int i = 0; i < 2; i++) {  // run the sequence twice, for x and y axis.

    if (Y_As) {  // if Y axis startup the pins will be change for the Y axis.
      enable = enPin_Y;
      diraction = dirPin_Y;
      steps = stepPin_Y;
      Distance = Distance_Y;
      Distance_half = Half_Distance_Y;
      TravelOffsetForward = TravelOffsetForward_Y;
      TravelOffsetBackward = TravelOffsetBackward_Y;
      scale.begin(LOADCELL_DOUT_PIN_Y, LOADCELL_SCK_PIN_Y);
      scale.set_scale(730.36);
    } else if (X_As) {  // if X axis startup the pins will be change for the X axis.
      enable = enPin_X;
      diraction = dirPin_X;
      steps = stepPin_X;
      Distance = Distance_X;
      Distance_half = Half_Distance_X;
      TravelOffsetForward = TravelOffsetForward_X;
      TravelOffsetBackward = TravelOffsetBackward_X;
      scale.begin(LOADCELL_DOUT_PIN_X, LOADCELL_SCK_PIN_X);
      scale.set_scale(730.36);
    }
    digitalWrite(enable, LOW);  // turn on motor

    //motor runs till it hits the button. This is so that all messurement have the same starting point.
    while (digitalRead(S4) == LOW) {
      digitalWrite(diraction, LOW);
      analogWrite(steps, 10);  // analogWrite values from 0 to 255
    }
    float run = millis();                         // millis() need to be save into a local variable every sequence. so itll know the time it start moving
    while (millis() < run + wait && !overload) {  //motor waits foor 1s
      digitalWrite(diraction, HIGH);              //Diraction forward
      analogWrite(steps, 0);                      // analogWrite values from 0 to 255
    }
    run = millis();
    while (millis() < run + (Distance + TravelOffsetForward) && !overload) {  //motor runs forward
      forward = true;
      backward = false;
      digitalWrite(diraction, HIGH);  //Diraction forward
      analogWrite(steps, 10);         // analogWrite values from 0 to 255
      tare = true;
      LCCalib();  //take messurment for forward offset every cycle
    }
    run = millis();
    while (millis() < run + wait && !overload) {  //motor waits foor 1s
      digitalWrite(diraction, HIGH);
      analogWrite(steps, 0);  // analogWrite values from 0 to 255
    }
    run = millis();
    while (millis() < run + Distance && !overload) {  //motor runs backward
      forward = false;
      backward = true;
      digitalWrite(diraction, LOW);  //Diraction backward
      analogWrite(steps, 10);        // analogWrite values from 0 to 255
      LCCalib();                     //take messurment for backward offset every cycle
    }
    run = millis();

    while (millis() < run + wait && !overload) {  //motor waits foor 1s
      digitalWrite(diraction, HIGH);
      analogWrite(steps, 0);  // analogWrite values from 0 to 255
    }
    run = millis();
    while (millis() < run + (Distance_half + TravelOffsetForward) && !overload) {  //motor runs halfstep forward
      digitalWrite(diraction, HIGH);
      analogWrite(steps, 10);  // analogWrite values from 0 to 255
    }
    run = millis();

    while (millis() < run + (wait * 2) && !overload) {  //motor waits foor 2s
      digitalWrite(diraction, HIGH);
      analogWrite(steps, 0);  // analogWrite values from 0 to 255
    }
    if (overload) {
      hasRun = false;
    } else {
      hasRun = true;
    }
    // change the axis for the second sequence.
    Y_As = false;
    X_As = true;
    offsetForward = 0; //reset generic forward offset value.
    offsetBackward = 0;//reset generic backward offset value.
  }
  analogWrite(steps, 0);  // analogWrite values from 0 to 255
}