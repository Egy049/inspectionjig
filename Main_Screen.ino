// #########################################################################
//  Setup for Topbar
// #########################################################################

void topbar() {
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  if (targetTime < millis()) {
    clockCount();
    // Set next update for 1 second later
    targetTime = millis() + 1000;
    // Update digital time
    int xpos = (tft.width() / 2) - 40;
    int ypos = 4;                                                           // Top left corner of clock text, about half way down
    if ((omm != mm) || (!checkButton() || !upButton() || !downButton())) {  // Redraw hours and minutes time every minute or when button is pushed
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 4);  // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, 4);              // Draw hours
      xpos += tft.drawChar(':', xpos, ypos, 4);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 4);  // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, 4) + 100;        // Draw minutes
      if (dd < 10) xpos += tft.drawChar('0', xpos, ypos, 4);  // Add day leading zero
      xpos += tft.drawNumber(dd, xpos, ypos, 4);              // Draw day
      xpos += tft.drawChar('/', xpos, ypos, 4);
      if (MM < 10) xpos += tft.drawChar('0', xpos, ypos, 4);  // Add months leading zero
      xpos += tft.drawNumber(MM, xpos, ypos, 4);
      xpos += tft.drawChar('/', xpos, ypos, 4);
      if (yy < 10) xpos += tft.drawChar('0', xpos, ypos, 4);  // Add year leading zero
      xpos += tft.drawNumber(yy, xpos, ypos, 4);              // Draw year
    }
    tft.setFreeFont(FF26);
    tft.drawString("ID", 0, 5, 1);
    tft.drawChar(':', tft.textWidth("ID", 1), 20, 1);
    if (newMessurement || checkButton()) {
      tft.drawNumber(ID, tft.textWidth("ID:  ", 1), 5, 1);
      newMessurement = false;
    }
    tft.drawNumber(ID, tft.textWidth("ID:  ", 1), 5, 1);
  }
  textDate = dd + seperator + MM + seperator + yy;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setFreeFont(GLCD);
}
// #########################################################################
//  Setup for calibration screen
// #########################################################################

void ScreenForCalibration() {
  tft.setFreeFont(FF26); // set font to FreeSansBold12pt7b
  // draw strings to screen.
  tft.drawString("To begin the calibration", 0, 100, 1);
  tft.drawString("Press the      button", 0, 140, 1);
  //draw checkmark
  int temp = tft.textWidth("Press the  ", 1);
  for (int i = 0; i < 3; i++) {
    tft.drawLine(temp + i, 150, temp + 10 + i, 160, TFT_GREEN);
  }
  for (int i = 9; i < 12; i++) {
    tft.drawLine(temp + i, 160, temp + 15 + i, 140, TFT_GREEN);
  }
  //reset font
  tft.setFreeFont(GLCD);
}

// #########################################################################
//  Setup for screen to wait the arm ficture to go down to start the messurement
// #########################################################################
void ScreenForMessurement() {
  tft.setFreeFont(FF26);
  if (Y_As) {
    tft.drawString("To begin the Y measurement, ", 0, 100, 1);
  } else if (X_As) {
    tft.drawString("To begin the X measurement, ", 0, 100, 1);
  }

  tft.drawString("fixture the arm to the vent.", 0, 140, 1);
  tft.drawString("Then press the      button", 0, 180, 1);
  int temp = tft.textWidth("Then press the  ", 1);
  for (int i = 0; i < 3; i++) {
    tft.drawLine(temp + i, 190, temp + 10 + i, 200, TFT_GREEN);
  }
  for (int i = 9; i < 12; i++) {
    tft.drawLine(temp + i, 200, temp + 15 + i, 180, TFT_GREEN);
  }
  tft.setFreeFont(GLCD);
}

void waitScreen() {
  tft.setFreeFont(FF26);
  tft.drawString("Do not block the arm, while it's calibrating!! ", 0, 100, 1);
  tft.setFreeFont(GLCD);
}

void errorScreen() {
  tft.setFreeFont(FF26);
  tft.drawString("Invalid messurement ", 0, 100, 1);
  tft.drawString("Remove the blockage and press ", 0, 120, 1);
  int temp = tft.textWidth("remove the blockage and press  ", 1);
  for (int i = 0; i < 3; i++) {
    tft.drawLine(temp + i, 130, temp + 10 + i, 140, TFT_GREEN);
  }
  for (int i = 9; i < 12; i++) {
    tft.drawLine(temp + i, 140, temp + 15 + i, 120, TFT_GREEN);
  }

  tft.setFreeFont(GLCD);
}

void savingScreen() {
  tft.setFreeFont(FF26);
  tft.drawString("To save the messurement to the SD card   ", 0, 50, 1);
  tft.drawString("press     .", 0, 80, 1);
  int temp = tft.textWidth("press  ", 1);
  for (int i = 0; i < 3; i++) {
    tft.drawLine(temp + i, 90, temp + 10 + i, 100, TFT_GREEN);
  }
  for (int i = 9; i < 12; i++) {
    tft.drawLine(temp + i, 100, temp + 15 + i, 80, TFT_GREEN);
  }
  tft.drawString("to continue without saving the previous ", 0, 130, 1);
  tft.drawString("measurement, press ^ button.   ", 0, 160, 1);

  tft.drawString("to redo the previous measurement, ", 0, 210, 1);
  tft.drawString("press V button. ", 0, 260, 1);
  tft.setFreeFont(GLCD);
}
