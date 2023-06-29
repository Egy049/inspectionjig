void LCLoop() {
  if (Y_As) {  //if Y axis the pins will be change for the Y axis.
    offsetForward = offsetForward_Y;
    offsetBackward = offsetBackward_Y;
    lowerbound = lowerbound_Y;
    upperbound = upperbound_Y;
    scale.begin(LOADCELL_DOUT_PIN_Y, LOADCELL_SCK_PIN_Y);
    scale.set_scale(730.36);
  } else if (X_As) {  //if X axis the pins will be change for the Y axis.
    offsetForward = offsetForward_X;
    offsetBackward = offsetBackward_X;
    lowerbound = lowerbound_X;
    upperbound = upperbound_X;
    scale.begin(LOADCELL_DOUT_PIN_X, LOADCELL_SCK_PIN_X);
    scale.set_scale(730.36);
  }
  if (forward == true) {                                    // start forward messurement
    Lcreading = scale.get_units() + (offsetForward - 102);  // get reading from the scale and subtract the offset
    if (Lcreading < 0) {
      Lcreading = -Lcreading;  // there cant be negative reading, though it is still a valid reading, so make all negative value positive
    }
    Lcreading = Lcreading * 0.0098;                         //convert value in gram to force
    if ((Lcreading > 8 + upperbound) || (Lcreading < 0)) {  //when the value is 8N over the upperbound the overflow flag will be raised.
      overload = true;
    }
    if (Lcreading >= peakValueForward) {
      peakValueForward = Lcreading;  // saving peak value
      peak = true;
      if ((lowerbound <= peakValueForward) && (peakValueForward <= upperbound)) {
        valid = true;  // if the peakvalue is within the bound, the messurement will be considered valid
      }
    }
    messurementValue();      // call function to show force
    drawGraphLineForward();  // call function to draw graph
  }
  if (backward == true) {                                    // start backward messurement
    Lcreading = scale.get_units() - (offsetBackward + 200);  // get reading from the scale and subtract the offset.
    if (Lcreading < 0) {
      Lcreading = -Lcreading;  // there cant be negative reading, though it is still a valid reading, so make all negative value positive
    }
    Lcreading = Lcreading * 0.0098;                         //convert value in gram to force
    if ((Lcreading > 8 + upperbound) || (Lcreading < 0)) {  //when the value is 8N over the upperbound the overflow flag will be raised.
      overload = true;
    }
    if (Lcreading >= peakValueBackward) {
      peakValueBackward = Lcreading;  // saving peak value
      peak = true;
      if ((lowerbound <= peakValueBackward) && (peakValueBackward <= upperbound)) {
        valid = true;  // if the peakvalue is within the bound, the messurement will be considered valid
      }
    }
    messurementValue();       // call function to show force
    drawGraphLinebackward();  // call function to draw graph
  }
}
