// #########################################################################
// Draw graph forward
// #########################################################################

void drawGraphLineForward() {
  if ((draw == true)) {
    double xlo = 0;
    double xhi = 28;
    double gx = 30;
    double ylo = 0;
    double yhi = 8;
    double gy = 130;
    double w = 140;
    double h = 80;
    if (save == false) {
      StartT = millis();
      save = true;
    }

    x = ((millis() - StartT) / 100);

    y = Lcreading;
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    tft.drawLine(ox1, oy1, x, y, TFT_WHITE);
    ox1 = x;
    oy1 = y;
    draw = false;
  }
}
void drawGraphForward() {

  int VDotTop = 50;
  int Vtop = 8;

  int HDotLow = 30;
  int Hlow = 0;
  int i = 2;
  int a = 2;

  int ypos = 80;

  tft.fillRect(0, (tft.height() / 2), 480, 5, TFT_WHITE);

  //vertical axis y
  tft.drawString("Force(N)", 30, 40, 1);
  while (VDotTop != 150) {
    tft.drawPixel(32, VDotTop, TFT_WHITE);
    if (a % 2 == 0) {
      tft.drawNumber(Vtop, 10, VDotTop - 3, 1);
      Vtop -= 2;
    }
    a++;
    VDotTop += 10;
  }

  //horizontal axis y
  tft.drawString("Time(100Ms)", 80, 150, 1);
  while (HDotLow != 210) {
    tft.drawPixel(HDotLow, 132, TFT_WHITE);
    if (i % 2 == 0) {
      if (Hlow == 0) {
        tft.drawNumber(Hlow, HDotLow - 10, 135, 1);
      } else {
        tft.drawNumber(Hlow, HDotLow - 2, 135, 1);
      }

      Hlow += 7;
    }
    i++;
    HDotLow += 20;
  }

  tft.setFreeFont(FF2);
  tft.drawString("Forward", 195 + tft.textWidth("Peak force", 1), 30, 1);
  tft.setFreeFont(GLCD);
  tft.drawFastVLine(30, 50, 100, TFT_WHITE);
  tft.drawFastHLine(30, 130, 160, TFT_WHITE);

  /////////////////////////////////////////////

  tft.drawString("Peak Force :", 210, 70, 2);
  tft.drawString("Force      :", 210, 110, 2);
}
// #########################################################################
//  Graph Arm X
// #########################################################################
void drawGraphLinebackward() {
  if ((draw == true)) {
    double xlo = 0;
    double xhi = 28;
    double gx = 30;
    double ylo = 0;
    double yhi = 8;
    double gy = 280;
    double w = 140;
    double h = 80;
    if (save == false) {
      StartT = millis();
      save = true;
    }

    x = ((millis() - StartT) / 100);
    y = Lcreading;
    Serial.println(y);
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    tft.drawLine(ox2, oy2, x, y, TFT_WHITE);
    ox2 = x;
    oy2 = y;
    draw = false;
  }
}
void drawGraphBackward() {

  int VDotTop = 200;
  int Vtop = 8;

  int HDotLow = 30;
  int Hlow = 0;
  int i = 2;
  int a = 2;

  int ypos = 230;

  //vertical axis X
  tft.drawString("Force(N)", 30, 190, 1);
  while (VDotTop != 300) {
    tft.drawPixel(32, VDotTop, TFT_WHITE);
    if (a % 2 == 0) {
      tft.drawNumber(Vtop, 10, VDotTop - 3, 1);
      Vtop -= 2;
    }
    a++;
    VDotTop += 10;
  }

  //horizontal axis X
  tft.drawString("Time(100Ms)", 80, 300, 1);
  while (HDotLow != 210) {
    tft.drawPixel(HDotLow, 282, TFT_WHITE);
    if (i % 2 == 0) {
      if (Hlow == 0) {
        tft.drawNumber(Hlow, HDotLow - 10, 285, 1);
      } else {
        tft.drawNumber(Hlow, HDotLow - 2, 285, 1);
      }

      Hlow += 7;
    }
    i++;
    HDotLow += 20;
  }

  tft.setFreeFont(FF2);
  tft.drawString("Backward", 195 + tft.textWidth("Peak force", 1), 180, 1);
  tft.setFreeFont(GLCD);
  tft.drawFastVLine(30, 200, 100, TFT_WHITE);
  tft.drawFastHLine(30, 280, 160, TFT_WHITE);
  /////////////////////////////////////////////

  tft.drawString("Peak Force :", 210, 220, 2);
  tft.drawString("Force      :", 210, 260, 2);
}

void messurementValue() {
  float oldLcreading = 0;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if ((oldLcreading != Lcreading) || (checkButton() == true)) {
    oldLcreading = Lcreading;
    redraw = true;
  }
  tft.setFreeFont(GLCD);
  if (redraw == true) {
    if (forward) {
      tft.drawFloat(oldLcreading, 1, tft.textWidth("Force      : ", 1) + 210, 110, 2);
    }
    if (backward) {
      tft.drawFloat(oldLcreading, 1, tft.textWidth("Force      : ", 1) + 210, 260, 2);
    }
    redraw = false;
  }

  if (peak) {
    if (forward) {
      tft.drawFloat(peakValueForward, 2, tft.textWidth("Peak Force : ", 1) + 210, 70, 2);
    }
    if (backward) {
      tft.drawFloat(peakValueBackward, 2, tft.textWidth("Peak Force : ", 1) + 210, 220, 2);
    }
    peak=false;
  }
}