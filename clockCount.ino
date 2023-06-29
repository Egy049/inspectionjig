void clockCount() {
  if (targetTime < millis()) {
    // Set next update for 1 second later
    targetTime = millis() + 1000;

    // Adjust the time values by adding 1 second
    ss++;             // Advance second
    if (ss == 60) {   // Check for roll-over
      ss = 0;         // Reset seconds to zero
      omm = mm;       // Save last minute time for display update
      mm++;           // Advance minute
      if (mm > 59) {  // Check for roll-over
        mm = 0;
        hh++;           // Advance hour
        if (hh > 23) {  // Check for 24hr roll-over (could roll-over on 13)
          hh = 0;       // 0 for 24 hour clock, set to 1 for 12 hour clock
          dd++;
          if (MM == 2) {
            if ((yy % 400 == 0) || (yy % 4 == 0 && yy % 100 != 0)) {
              if (dd > 29) {
                dd = 1;
                MM++;
              }
            }
            if (dd > 28) {
              dd = 1;
              MM++;
            }
          }
          if (MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10 || MM == 12) {
            if (dd > 31) {
              dd = 1;
              MM++;
              if (MM > 12) {
                MM = 1;
                yy++;
                if (yy > 99) {
                  yy = 0;
                }
              }
            }
          } else {
            if (dd > 30) {
              dd = 1;
              MM++;
            }
          }
        }
      }
    }
  }
}


uint8_t maxDay() {
  if (MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10 || MM == 12) {
    return 31;
  }
  if (MM == 4 || MM == 6 || MM == 9 || MM == 11) {
    return 30;
  }
  if (MM == 2) {
    if ((yy % 400 == 0) || (yy % 4 == 0 && yy % 100 != 0)) {
      return 29;
    } else {
      return 28;
    }
  }
}