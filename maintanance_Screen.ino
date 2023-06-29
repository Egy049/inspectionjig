// #########################################################################
//  maintanance screen
// #########################################################################

void maintenance() {
  int ypos = tft.height() / 8;
  tft.setFreeFont(FF26);
  if (state == 10) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  ypos += tft.drawString("Time  ", 0, 50, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 11) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  ypos += tft.drawString("Date   ", 0, 100, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 12) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  ypos += tft.drawString("Travel offset   ", 0, 150, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 13) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Messurement bounds   ", 0, 200, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 14) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Recalibrate   ", 0, 250, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 15) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Exit   ", 0, 300, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(GLCD);
}

void messurementBounds() {
  tft.setFreeFont(FF26);
  if (state == 40) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Upper bounds Y axis", 0, 40, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 41) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Lower bounds Y axis", 0, 80, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 42) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Upper bounds X axis", 0, 120, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 43) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Lower bounds X axis", 0, 160, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (state == 45) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Back", 0, 280, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(GLCD);
}
void TravelOffsets() {
  tft.setFreeFont(FF26);
  if (state == 52) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("travel offset forward Y axis", 0, 40, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 53) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("travel offset backward Y axis", 0, 80, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 54) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("travel offset forward X axis", 0, 120, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 55) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("travel offset backward X axis", 0, 160, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  if (state == 56) {
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  }
  tft.drawString("Back", 0, 280, 1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setFreeFont(GLCD);
}

void Tr_Offsets() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // Update digital time
  int xpos = (tft.width() / 2) - 80;
  int ypos = 125;

  if ((!checkButton() || !upButton() || !downButton())) {  // Redraw when button is pushed
    if (TravelOffset >= 1000) {
      xpos += tft.drawNumber(TravelOffset, xpos, ypos, 7);
    } else {
      xpos += tft.drawChar('0', xpos, ypos, 7);
      if (TravelOffset >= 100) {
        xpos += tft.drawNumber(TravelOffset, xpos, ypos, 7);
      } else {
        xpos += tft.drawChar('0', xpos, ypos, 7);
        if (TravelOffset >= 10) {
          xpos += tft.drawNumber(TravelOffset, xpos, ypos, 7);
        } else {
          xpos += tft.drawChar('0', xpos, ypos, 7);
          xpos += tft.drawNumber(TravelOffset, xpos, ypos, 7);
        }
      }
    }
  }
  if (draw) {
    tft.setFreeFont(FF26);
    if (forward) {
      if (Y_As) {
        tft.drawString("Y-Axis", 240 - (tft.textWidth("Forward travel", 1)), 300, 1);
      } else if (X_As) {
        tft.drawString("X-Axis   ", 240 - (tft.textWidth("Forward travel", 1)), 300, 1);
      }
      tft.drawString("Forward travel offset", 200, 300, 1);
    } else if (backward) {
      if (Y_As) {
        tft.drawString("Y-Axis", 240 - (tft.textWidth("Backward travel", 1)), 300, 1);
      } else if (X_As) {
        tft.drawString("X-Axis", 240 - (tft.textWidth("Backward travel", 1)), 300, 1);
      }
      tft.drawString("Backward travel offset", 200, 300, 1);
    }
  }
  draw = false;
}

void MTime() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  DateTime now = rtc.now();
  hh = now.hour(), mm = now.minute(), ss = now.second(), yy = now.year() - 2000, MM = now.month(), dd = now.day();
  // Update digital time
  int xpos = (tft.width() / 2) - 80;
  int ypos = 125;
  if (targetTime < millis()) {
    clockCount();
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    if ((omm != mm) || (!checkButton() || !upButton() || !downButton())) {  // Redraw time every minute or when button is pushed
      omm = mm;
      // Draw hours and minutes
      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 7);  // Add hours leading zero for 24 hr clock
      xpos += tft.drawNumber(hh, xpos, ypos, 7);              // Draw hours
      xpos += tft.drawChar(':', xpos + 5, ypos - 5, 7);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 7);  // Add minutes leading zero
      xpos += tft.drawNumber(mm, xpos, ypos, 7);              // Draw minutes
    }
  }
  tft.setFreeFont(GLCD);
}

void MDate() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  DateTime now = rtc.now();
  hh = now.hour(), mm = now.minute(), ss = now.second(), yy = now.year() - 2000, MM = now.month(), dd = now.day();
  if (targetTime < millis()) {
    clockCount();
    // Set next update for 1 second later
    targetTime = millis() + 1000;
    // Update digital time
    int xpos = (tft.width() / 4) - 20;
    int ypos = 125;
    if ((!checkButton() || !upButton() || !downButton())) {   // Redraw Date every minute or when button is pushed
      if (dd < 10) xpos += tft.drawChar('0', xpos, ypos, 7);  // Add day leading zero
      xpos += tft.drawNumber(dd, xpos, ypos, 7) + 10;         // Draw day
      tft.setFreeFont(FF28);
      xpos += tft.drawChar('/', xpos, ypos + 40, 1) + 10;
      tft.setFreeFont(GLCD);
      if (MM < 10) xpos += tft.drawChar('0', xpos, ypos, 7);  // Add months leading zero
      xpos += tft.drawNumber(MM, xpos, ypos, 7) + 10;
      tft.setFreeFont(FF28);
      xpos += tft.drawChar('/', xpos, ypos + 40, 1) + 10;
      tft.setFreeFont(GLCD);
      if (yy < 10) xpos += tft.drawChar('0', xpos, ypos, 7);  // Add months leading zero
      xpos += tft.drawNumber(yy, xpos, ypos, 7);
    }
  }
  tft.setFreeFont(GLCD);
}


void Bounds() {
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int xpos = (tft.width() / 2) - 50;
  int ypos = 125;
  if ((!checkButton() || !upButton() || !downButton())) {  // Redraw when button is pushed
    tft.drawFloat(bounds, 2, xpos, ypos, 7);               // Draw bound
  }
  if (draw) {
    tft.setFreeFont(FF26);
    // draw label 
    if (forward) {
      if (Y_As) {
        tft.drawString("Y-Axis   ", 240 - (tft.textWidth("Upper Bound", 1)), 300, 1);
      } else if (X_As) {
        tft.drawString("X-Axis   ", 240 - (tft.textWidth("Upper Bound", 1)), 300, 1);
      }
      tft.drawCentreString("Upper Bound", 240, 300, 1);
    } else if (backward) {
      if (Y_As) {
        tft.drawString("Y-Axis   ", 240 - (tft.textWidth("Lower Bound", 1)), 300, 1);
      } else if (X_As) {
        tft.drawString("X-Axis   ", 240 - (tft.textWidth("Lower Bound", 1)), 300, 1);
      }
      tft.drawCentreString("Lower Bound", 240, 300, 1);
    }

    tft.setFreeFont(GLCD);
    draw = false;
  }
}
