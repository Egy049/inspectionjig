long forwardReading = 0;
long backwardReading = 0;

void LCCalib() {
  /*
  Serial.print("offsetforward: ");
  Serial.println(offsetForward);
  Serial.print("offsetbackwards: ");
  Serial.println(offsetBackward);
  */
  if (tare) {
    scale.tare();  // start tare the loadcell so messurement start at 0.
    tare = false;
  }
  //start measuring forward friction and other offset that could impact the messurement.
  if (forward == true) {
    long forwardReading = scale.get_units(2);  // messurement get save in local variable
    if (forwardReading < 0) {
      forwardReading = -forwardReading;  //make negative messurement positive, this is due to it sometime getting negative value but it is still a valid offsit that needs to be accounted.
    }
    /*
    Serial.print("readin: ");
    Serial.println(forwardReading);
    Serial.println(backwardReading);
    */
    if (forwardReading > offsetForward) {
      offsetForward = forwardReading;  // keep the highest offset.
    }
    if ((offsetForward > 500)) {
      overload = true;  // overload detection, 500 is chosen because it has been seen that the offset can get no bigger then 300g. so there's a huge probibility that ther is something wrong with the friction if the offset is over 500g.
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //start measuring backward friction and other offset that could impact the messurement.
  if (backward == true) {
    long backwardReading = scale.get_units(2);  // messurement get save in local variable
    /*
    Serial.print("readin: ");
    Serial.println(forwardReading);
    Serial.println(backwardReading);
    */
    if (backwardReading > 0) {
      backwardReading = -backwardReading;  //make negative messurement positive, this is due to it sometime getting negative value but it is still a valid offsit that needs to be accounted.
    }
    if (backwardReading < offsetBackward) {
      offsetBackward = backwardReading;  // keep the highest offset.
    }
    if (offsetBackward < -500) {
      overload = true;  // overload detection, 500 is chosen because it has been seen that the offset can get no bigger then 300g. so there's a huge probibility that ther is something wrong with the friction if the offset is over 500g
    }
  }
  if (Y_As) {
    // if startup sequence is on the y axis itll save the value on local varible for the y axis.
    offsetForward_Y = offsetForward;
    offsetBackward_Y = offsetBackward;
  } else if (X_As) {
    // if startup sequence is on the X axis itll save the value on local varible for the X axis.
    offsetForward_X = offsetForward;
    offsetBackward_X = offsetBackward;
  }
}
