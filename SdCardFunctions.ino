// #########################################################################
// function to write messurement to the SD card
// #########################################################################
void SdCardWrite() {
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to .txt ...");
    if (Y_As) {
      myFile.println(textID);
      myFile.println(textDate);
    }
    myFile.println(textAs);
    myFile.println(textValidForward);
    myFile.println(textPeakForward);
    myFile.println(textValidBackward);
    myFile.println(textPeakBackward);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening .txt");
  }
}
// #########################################################################
// function to read data from a file in sd card
// #########################################################################
void SdCardRead() {
  myFile = SD.open(fileName);
  if (myFile) {
    Serial.println(fileName);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening txt");
  }
}
// #########################################################################
//function to delete a line from a textfile
// #########################################################################
void deleteLine(uint16_t lineNumber) {
  openFile();
  seekToBeginning();

  for (uint16_t i = 0; i < lineNumber - 1; i++) {
    if (!myFile.find('\n')) {
      // Handle line not found error
      myFile.close();
      return;
    }
  }

  if (!myFile.truncate(myFile.curPosition())) {
    // Handle file truncate error
  }

  myFile.close();
}
// #########################################################################
// function to open file
// #########################################################################
void openFile() {
  if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
    Serial.println("File open failed.");
    while (1)
      ;
  }
}
// #########################################################################
// function to seek a certian line in the textfile
// #########################################################################
void seekToBeginning() {
  if (!myFile.seekSet(0)) {
    Serial.println("File seek failed.");
    while (1)
      ;
  }
}
// #########################################################################
//function to seek and read the last line in a file
// #########################################################################
String seekAndReadLastLine() {
  openFile();
  seekToBeginning();

  String lastLine = "";
  String currentLine = "";

  while (myFile.available()) {
    char ch = myFile.read();

    if (ch == '\n') {
      lastLine = currentLine;
      currentLine = "";
    } else {
      currentLine += ch;
    }
  }

  myFile.close();
  return lastLine;
}
