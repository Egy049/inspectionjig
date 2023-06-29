// ##################################################################################################################################################//
// All Serial.print are for debugging purposses , to have a better view of the output, comment out the output you dont need                          //
// ##################################################################################################################################################//
#include <RTClib.h>
#include <Wire.h>
#include <TFT_HX8357_Due.h>  // Hardware-specific library
#include <avr/dtostrf.h>
#include "HX711.h"
#include "MST.h"
#include "clock.h"
#include "calender.h"
#include "Free_Fonts.h"
#include "crossmark.h"
#include "checkMark.h"
#include <SPI.h>
#include <SdFat.h>
SdFat SD;
File myFile;
const char* fileName;
HX711 scale;
RTC_PCF8563 rtc;
TFT_HX8357_Due tft = TFT_HX8357_Due();  // Invoke custom library
// #########################################################################//
//                              All function                                //
// #########################################################################//
void LCLoop();                                                                                 //Function for the loadcell to gether messurement.
void MotorLoop();                                                                              //Function for step motor to run.
void Startuploop();                                                                            //Function for step motor to run for startup.
void LCCalib();                                                                                //Function for loadcell calibration.
void topbar();                                                                                 //Funtion for the topbar of the screen.
void drawIcon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height);  //Function for drawing all the icons.
void erase();                                                                                  // function to erase what is on the screen and reset some things for drawing.
// #########################################################################//
//              All the function in Main_screen.ino                         //
// #########################################################################//

void ScreenForCalibration();  //Function to confirm the calibration sequence.
void ScreenForMessurement();  //Function to confirm the messurement sequence.
void waitScreen();            //Function to show a warning screen when messurement is running.
void errorScreen();           //Function to show a warning screen when a messurement is invalid.
void savingScreen();          //Function to confrim the if the user wants to save the messurement.
// #########################################################################//
//              All the function in maintanance__screen.ino                 //
// #########################################################################//
void maintenance();        //Function for the maintenance screen
void messurementBounds();  //Function for the screen to chose which messurement bounds the users wants to change.
void TravelOffsets();      //Function for the screen which travel offsets the user wants to change.
void Tr_Offsets();         //Function to show the users the travel offsets value on screen to then be change.
void MTime();              //Function to show the users the Time value on screen to then be change.
void MDate();              //Function to show the users the Date value on screen to then be change.
void Bounds();             //Function to show the users the messurement bounds value on screen to then be change.
// #########################################################################//
//              All the function in measurment__screen.ino                  //
// #########################################################################//
void drawGraphLineForward();   //Function to draw the graphline for forward graph.
void drawGraphForward();       //Function to draw the graph for forward messurement
void drawGraphLinebackward();  //Function to draw the graphline for backward graph.
void drawGraphBackward();      //Function to draw the graph for backward messurement.
void messurementValue();       //Function to show the messurement value and the peak value on the screen.
// #########################################################################//
//              All the function in button.ino                              //
// #########################################################################//
bool checkButton();  //Function for checkbutton debouncer.
bool upButton();     //Function for upbutton debouncer.
bool downButton();   //Function for downbutton debouncer.
// #########################################################################//
//              All the function in SdCardFunctions.ino                     //
// #########################################################################//
void SdCardWrite();                    //Function to write the messurement datas to the SD card.
void SdCardRead();                     //Function to read from the SD card.
void deleteLine(uint16_t lineNumber);  //Function to delete a line on a text file in the SD card
void openFile();                       //Function to open a text file in the SD card.
void seekToBeginning();                //Function to seek a value on a text file in a SD card.
String seekAndReadLastLine();          //Function to seek the last line on a text file in a SD card.
// #########################################################################//
//              All the function in clockCount.ino                          //
// #########################################################################//
void clockCount();  //Function for time rollover. ex. min to hour, hour to day and day to month.
uint8_t maxDay();   //Function to detect the max day of a month.
// #########################################################################



// #########################################################################//
//                               Pinout                                     //
// #########################################################################//
// HX711Y
const int LOADCELL_DOUT_PIN_X = 13;  //16;  //loadcell data out pin.
const int LOADCELL_SCK_PIN_X = 12;   //17;   //loadcell sck  pin.
const int LOADCELL_DOUT_PIN_Y = 13;
const int LOADCELL_SCK_PIN_Y = 12;
//MotordriveX
const int dirPin_Y = 2;   // Direction.
const int stepPin_Y = 3;  // Step.
const int enPin_Y = 6;    // Enable.
//MotordriveY//NEED CHANGE.
const int dirPin_X = 2;   //9;   // Direction.
const int stepPin_X = 3;  //10;  // Step.
const int enPin_X = 6;    //11;    // Enable.

// Define some steppers and the pins the will use.

//button on screen
const int buttonPin1 = A8;  // the Checkbutton pin.
const int buttonPin2 = A7;  // the Downbutton pin.
const int buttonPin3 = A6;  // the Upbutton pin.
const int buttonPin4 = A9;  // the startbutton pin.
//Limit switches
const int S1 = 15;  //Limit switch X-axis underneath the arm.
const int S2 = 14;  //Limit switch X-axis back.
const int S3 = 7;   //Limit switch Y-axis underneath the arm.
const int S4 = 8;   //Limit switch Y-axis back.
//buzzer
const int buzzerPin = 4;
// #########################################################################
//Variables
// #########################################################################
//button
int lastButtonState1 = HIGH;  // last button state for debouncer.
int lastButtonState2 = HIGH;
int lastButtonState3 = HIGH;
int lastButtonState4 = 0;
//button timer
unsigned long startTime = 0;       // Variable to store the start time.
unsigned long debounceDelay = 10;  // the debounce time; increase if the output flickers.
unsigned long holdTime = 5000;     // Hold time in milliseconds.
//motor
bool hasRun = false;       //Startup sequence flag.
bool forward = false;      //flag to detect if the motor should move forward.
bool backward = false;     //flag to detect if the motor should move backward.
int StateM = 0;            //State of the motor i.e. forward, bakcward, wait.
int enable;                //enable pin on the motor drive. This variable is used so a function can be used for both motor.
int diraction;             //diraction pin on the motor drive. This variable is used so a function can be used for both motor.
int steps;                 //Steps pin on the motor drive. This variable is used so a function can be used for both motor.
int Distance;              //Times the motor needs to travel. This variable is used so a function can be used for both motor.
int Distance_half;         //Half the times the motor needs to travel. This variable is used so a function can be used for both motor.
int TravelOffsetForward;   //Travel offset for forward movemement for motor. This variable is used so a function can be used for both motor.
int TravelOffsetBackward;  //Travel offset for backward movemement for motor. This variable is used so a function can be used for both motor.
int TravelOffset;          //Used to change the travel offset. This variable is used so a function can be used for both motor.

// motor Y travel time
int Distance_Y = 2600;       //Times of the Y motor to travel the whole rail.
int Half_Distance_Y = 1500;  //Half times of the Y motor to travel the whole rail.
int TravelOffsetForward_Y;   //Travel offset for forward movemement for motor Y.
int TravelOffsetBackward_Y;  //Travel offset for backward movemement for motor Y.
// motor X travel time
int Distance_X = 2600;       //Times of the X motor to travel the whole rail.
int Half_Distance_X = 1500;  //Half times of the X motor to travel the whole rail.
int TravelOffsetForward_X;   //Travel offset for forward movemement for motor X.
int TravelOffsetBackward_X;  //Travel offset for backward movemement for motor X.

int wait = 1000;  //Times for motor to wait when movementis done. used to avoind high peak value in the beggining of the messurement.

//loadcell
float Lcreading;         //The Loadcell reading.
float oldLCreading = 0;  //Old loadcell reading.

float upperbound_X;  //the upperbound of X axis.
float lowerbound_X;  //the lowerbound of X axis.

float upperbound_Y;  //the upperbound of Y axis.
float lowerbound_Y;  //the lowerbound of Y axis.

float upperbound;  //The upperbound of the messurement.This variable is used so a function can be used for both axis.
float lowerbound;  //The lowerbound of the messurement.This variable is used so a function can be used for both axis.
float bounds;      //Used to change the bounds. This variable is used so a function can be used for both axis.

double peakValueForward = 0;   //Store the peak value of the forward messurement.
double peakValueBackward = 0;  //Store the peak value of the backward messurement.

bool valid = false;  //Flag to know if a messurement is within the messurement bound.
//loadcell calibration
long offsetForward = 0;   //The offset Forward of the messurement. This variable is used so a function can be used for both axis.
long offsetBackward = 0;  //The offset backward of the messurement. This variable is used so a function can be used for both axis.

long offsetForward_Y = 0;   //The offset forward of the Y messurement.
long offsetBackward_Y = 0;  //The offset backward of the Y messurement.

long offsetForward_X = 0;   //The offset forward of the X messurement.
long offsetBackward_X = 0;  //The offset backward of the X messurement.

bool overload = false;


//state 0 default                 state 10 select time                 state 20 10 hours     state 30 10 days     state 40 select Upper bounds Y axis   state 46 1N     state 52 select travel offset forward Y axis      state 57 travel offset 1000   state 100 error screen
//state 1 ScreenForCalibration    state 11 select date                 state 21  1 hours     state 31  1 day      state 41 select Lower bounds Y axis   state 47 .1N    state 53 select travel offset backward Y axis     state 58 travel offset  100
//state 2 waitScreen              state 12 select travel offset        state 22 10 minutes   state 32 10 months   state 42 select Upper bounds X axis   state 48 .01N   state 54 select travel offset forward X axis      state 59 travel offset   10
//state 3 ScreenForMessurementY   state 13 select Messurement bound    state 23 20 minutes   state 33  1 month    state 43 select Lower bounds X axis                   state 55 select travel offset backward X axis     state 60 travel offset   1
//state 4 messurement Y           state 14 select Recalibrate                                state 34 10 years    state 45 select Back                                  state 56 select Back
//state 5 Save screen Y           state 15 select Exit                                       state 35  1 years
//state 6 ScreenForMessurementX
//state 7 messurement x
//state 8 Save screen X
int state = 0;


// for time and date
uint32_t targetTime = 0;         // for next 1 second timeout.
uint8_t hh, mm, ss, yy, MM, dd;  //to sace time from RTC.
byte omm = 99;                   // old minute.


//graph
bool peak = false;  //flag to detect peak value.
bool save = false;  //flag to save the start time of a messurement.
double StartT = 0;  //start time of a messurement
bool draw = false;  //flag to indicate if someting need to be draw.
double ox1 = 30;    //x=0 coordinate on the graph.
double oy1 = 130;   //y=0 coordinate on the graph.
double ox2 = 30;    //x=0 coordinate on the graph.
double oy2 = 280;   //y=0 coordinate on the graph.

// icons
#define BUFF_SIZE 64  // size for draw icon.

//sdcard
// all the strings that will be save in the SD card.
String seperator = "/";
String textID;
String textAs;
String textDate;
String textPeakForward;
String textPeakBackward;
String textValidForward;
String textValidBackward;
String direction;
//misc
int reading = 0;              // Value to be displayed
int ID;                       //messurement ID
float pressed;                //For debouncing start button.
float x, y;                   //For X and Y value for the graph.
float previousSave;           //Time delay for saving messurements.
bool newMessurement = false;  // flag to indicate if it is a new messurement.
bool redraw = true;           // flag to indicate if something need to be redrwan
bool X_As = false;            // flag to indicate that the current messurement is the Y axis.
bool Y_As = false;            // flag to indicate that the current messurement is the X axis.
bool deviceState = false;     // flag to start the main loop.
bool tare = false;            // flag to tare



void setup() {
  Serial.begin(57600);
  //button on screen

  //initialize the screen
  tft.begin();
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(HX8357_BLACK);
  // MotorY
  pinMode(dirPin_X, OUTPUT);  // sets the pin as output
  pinMode(enPin_X, OUTPUT);   // sets the pin as output
  // MotorX
  pinMode(dirPin_Y, OUTPUT);  // sets the pin as output
  pinMode(enPin_Y, OUTPUT);   // sets the pin as output
  //disable the motor so arm cant move
  digitalWrite(enPin_X, HIGH);
  digitalWrite(enPin_Y, HIGH);
  //Limit switchY setup
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S4, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  //buzer setup
  pinMode(buzzerPin, OUTPUT);  // make
  digitalWrite(buzzerPin, HIGH);
  //Sd card initialization
  if (!SD.begin(5))  // Looks for Sd card through cs pin 5
  {
    Serial.println("initialization failed!");
    tft.setFreeFont(FF26);
    //when a card isnt found itll warn the user, and ask if the users wants to continue.
    tft.drawString("Memory card is not available ", 0, 100, 1);
    tft.drawString("to continue press ", 0, 120, 1);
    int temp = tft.textWidth("to continue press  ", 1);
    for (int i = 0; i < 3; i++) {
      tft.drawLine(temp + i, 130, temp + 10 + i, 140, TFT_GREEN);
    }
    for (int i = 9; i < 12; i++) {
      tft.drawLine(temp + i, 140, temp + 15 + i, 120, TFT_GREEN);
    }
    tft.setFreeFont(GLCD);

    while (1) {
      if (checkButton() == true) {
        break;
      }
    }

  } else {
    // When SD card is found all saved scales values are then saved in locals variable.
    Serial.println("initialization done.");
    fileName = "IDtest.txt";
    ID = seekAndReadLastLine().toInt();
    Serial.println("ID done.");

    fileName = "upperbound_X.txt";
    upperbound_X = seekAndReadLastLine().toFloat();
    Serial.println("Upperbound_X done.");

    fileName = "upperbound_Y.txt";
    upperbound_Y = seekAndReadLastLine().toFloat();
    Serial.println("Upperbound_Y done.");

    fileName = "lowerbound_X.txt";
    lowerbound_X = seekAndReadLastLine().toFloat();
    Serial.println("Lowerbound_X done.");

    fileName = "lowerbound_Y.txt";
    lowerbound_Y = seekAndReadLastLine().toFloat();
    Serial.println("Lowerbound_Y done.");

    fileName = "TravelOffsetForward_Y.txt";
    TravelOffsetForward_Y = seekAndReadLastLine().toInt();
    Serial.println("TravelOffsetForward_Y done.");

    fileName = "TravelOffsetForward_X.txt";
    TravelOffsetForward_X = seekAndReadLastLine().toInt();
    Serial.println("TravelOffsetForward_X done.");

    fileName = "TravelOffsetBackward_X.txt";
    TravelOffsetBackward_X = seekAndReadLastLine().toInt();
    Serial.println("TravelOffsetBackward_X done.");

    fileName = "TravelOffsetBackward_Y.txt";
    TravelOffsetBackward_Y = seekAndReadLastLine().toInt();
    Serial.println("TravelOffsetBackward_Y done.");
  }

  //RTC
  // looks for RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      ;
  }
  if (rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }
  rtc.start();
  DateTime now = rtc.now();  // safe the RTC time now.

  hh = now.hour(), mm = now.minute(), ss = now.second(), yy = now.year() - 2000, MM = now.month(), dd = now.day();  // save all the value of the RTC into local variable
  targetTime = millis() + 1000;
}

void loop() {
  // DateTime now = rtc.now();
  int oldmd = maxDay();   // detect the month and sace the max day.
  while (!deviceState) {  // when start button is not pressed. the code is stuck in this loop, until the button pressed.
    /*
    Serial.println("off");
    Serial.println("pressed");
    */
    analogRead(buttonPin4);
    // when button is pressed the main loop starts else itll show black screen.
    if (analogRead(A9)) {
      Serial.println("pressed");
      pressed = millis();
      while (millis() < pressed + 500) { ; }
      deviceState = true;
    } else {
      tft.fillScreen(HX8357_BLACK);
    }
  }

  while (deviceState) {
    switch (state) {
        // #########################################################################
        //  Case for main screen
        // #########################################################################
        //Main screen
      default:
        erase();
        Serial.println(state);
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);  //Fill dark grey rectangle on top of the screen
        while (1) {
          int readingup = analogRead(buttonPin3);    // Read the state of the up button
          int readingdown = analogRead(buttonPin2);  // Read the state of the down button
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();                                                                                          //draw all the topbar value on the topbar.
          drawIcon(MST, (tft.width() - MSTWidth) / 2, (tft.height() - MSTHeight) / 2, MSTWidth, MSTHeight);  // draw MST icon.
          analogRead(buttonPin4);
          //Turn off / exit the main loop.
          if (analogRead(A9)) {
            Serial.println("pressed");
            pressed = millis();
            while (millis() < pressed + 500) { ; }
            deviceState = false;
            break;
          }
          if (readingup != lastButtonState3 || readingdown != lastButtonState2) {

            startTime = millis();
          }
          // when up and button is pressed for 5s it will go to the maintanence screen.
          if (readingup == LOW && readingdown == LOW && millis() - startTime >= holdTime) {
            state = 10;
            break;
          } else if (checkButton()) {
            // if checkbutton is pressed and startup loop hasnt been run, it will go to screen for conformation for startup sequence.
            if (hasRun == false) {
              state = 1;
              break;
            } else {
              // if checkbutton is pressed and startup loop has been run, and th Y-axis hasnt been messured, itll go to screen for conformation for Y-Axis messurement.
              if (Y_As) {
                state = 3;
                break;
                // if checkbutton is pressed and startup loop has been run, and th Y-axis has been messured, itll go to screen for conformation for X-Axis messurement.
              } else if (X_As) {
                state = 6;
              }
            }
          }
          lastButtonState3 = readingup;
          lastButtonState2 = readingdown;
        }
        break;

      case 1:  //conformation screen for startup sequence.
        erase();
        ScreenForCalibration();  // load screen conformation for startup sequence.
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        while (1) {
          topbar();
          analogRead(buttonPin1);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }*/
          // when the checkbutton is pressed and the arm is raised itll start the startup sequence.
          if ((checkButton() == true) && (digitalRead(S3) == LOW)) {
            state = 2;
            break;
          }
        }
        break;

      case 2:  //startup sequence
        erase();
        topbar();
        waitScreen();  //load waiting warning screen once startup loop is started
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        while (1) {
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          while (hasRun == false && !overload) {  // while startup is not done and loadcell is not overload, startup will run.
            Startuploop();                        // start the startuploop sequence
          }
          // reset the y and x axis flag.
          Y_As = false;
          X_As = false;
          if (hasRun == true) {
            //when startup is done, the buzzer will make a noise for 500ms.
            float buzz = millis();
            while (millis() < buzz + 500) {
              digitalWrite(buzzerPin, HIGH);
            }
            digitalWrite(buzzerPin, LOW);
          }
          break;
        }
        if (overload == true) {
          //if theres an overload itll go to the warning screen.
          state = 100;
        } else {
          //when startup is finish it goes to the screen conformation for
          state = 3;
        }
        break;
      case 3:  // messurement screen y axis
        erase();
        Y_As = true;  // flag is raised to indicate its Y axis messurement.
        X_As = false;
        ScreenForMessurement();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        //reset peak values.
        peakValueForward = 0;
        peakValueBackward = 0;
        while (1) {
          analogRead(buttonPin1);
          /*
          Serial.print("checkbutton=");
          Serial.println(analogRead(buttonPin1));
          Serial.print("S3=");
          Serial.println(digitalRead(S3));
          */
          topbar();
          if ((digitalRead(S3) == HIGH) && (checkButton() == true)) {  //when checkbutton is pressed and the arm is fixtured to the vent the messurement will start
            state = 4;
            break;
          }
          if ((checkButton() == true)) {  //when checkbutton is pressed but the arm isnt fixtured to the vent itll go back to the main screen.
            state = 0;
            break;
          }
        }
        break;
      case 4:  //Messurement Y axis.
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        drawGraphForward();   // draw graph axis forward.
        drawGraphBackward();  // draw graph axis backward.
        topbar();
        newMessurement = true;                 //indicate new messurement
        StateM = 1;                            // set where the motordrive sequence start
        while (!checkButton() && !overload) {  //after the sequence and the checkbutton is pressed itll go to the saving screen
          analogRead(buttonPin1);
          MotorLoop();  //start y axis messurement sequence.
          topbar();
        }
        analogWrite(stepPin_Y, 0);  // analogWrite values from 0 to 255
        if (overload) {
          state = 100;
        } else {
          state = 5;
        }
        break;

      case 5:  //Saving screen
        erase();
        topbar();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        savingScreen();  //load up saving screen
        while (1) {
          topbar();
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          if (upButton() == true) {  // if upbutton is pressed itll continue to the next state without saving the Y axis messurement.
            state = 6;
            Y_As = false;
            X_As = false;
            break;
          }
          if (checkButton() == true) {  // if checkbutton is pressed itll continue to the next state and save the Y axis messurement.
            state = 6;
            if (ID == 0) {
              textID = "0";
            }
            //write value
            textID = ID;
            textAs = "Y-AXIS";
            fileName = "TEST.txt";
            SdCardWrite();
            SdCardRead();

            Y_As = false;
            X_As = false;
            break;
          }
          if (downButton() == true) {  // if downbutton is pressed itll go back to the privious state  without saving the Y axis messurement.
            state = 3;
            Y_As = false;
            X_As = false;
            break;
          }
        }
        break;
      case 6:  // messurement screen X axis
        erase();
        Y_As = false;
        X_As = true;  // flag is raised to indicate its X axis messurement.
        ScreenForMessurement();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        //reset peak values.
        peakValueForward = 0;
        peakValueBackward = 0;
        while (1) {
          analogRead(buttonPin1);
          /*
          Serial.print("checkbutton=");
          Serial.println(analogRead(buttonPin1));
          Serial.print("S3=");
          Serial.println(digitalRead(S3));
          */
          topbar();
          if ((digitalRead(S3) == HIGH) && (checkButton() == true)) {  //when checkbutton is pressed and the arm is fixtured to the vent the messurement will start
            state = 7;
            break;
          }
          if ((checkButton() == true)) {  //when checkbutton is pressed but the arm isnt fixtured to the vent itll go back to the main screen.
            state = 0;
            break;
          }
        }
        break;
      case 7:  //Messurement X axis.
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        drawGraphForward();
        drawGraphBackward();
        topbar();
        newMessurement = true;
        StateM = 1;
        while (!checkButton() && !overload) {
          analogRead(buttonPin1);
          MotorLoop();  //start X axis messurement sequence.
          topbar();
        }
        analogWrite(stepPin_X, 0);  // analogWrite values from 0 to 255
        if (overload) {
          state = 100;
        } else {
          state = 8;
        }
        break;

      case 8:  //Saving screen
        erase();
        topbar();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        savingScreen();
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          if (upButton() == true) {
            state = 3;
            Y_As = false;
            X_As = false;
            break;
          }
          if (checkButton() == true) {
            state = 3;
            if (ID == 0) {
              textID = "0";
            }
            //write value
            textAs = "X-AXIS";
            fileName = "TEST.txt";
            SdCardWrite();
            SdCardRead();
            //wait
            previousSave = millis();
            while (millis() < previousSave + 500)
              ;
            // write ID
            ID++;  // when messurement is saved messurement ID will be raised. and saved into the ID.txt
            textID = ID;
            fileName = "IDtest.txt";
            myFile = SD.open("IDtest.txt", FILE_WRITE);
            if (myFile) {
              Serial.print("Writing to .txt ...");
              myFile.println(textID);
              // close the file:
              myFile.close();
              Serial.println("done.");
            } else {
              // if the file didn't open, print an error:
              Serial.println("error opening .txt");
            }
            SdCardRead();
            Y_As = false;
            X_As = false;
            break;
          }
          if (downButton() == true) {
            state = 6;
            Y_As = false;
            X_As = false;
            break;
          }
        }
        break;
        // #########################################################################
        //  Case for maintenance screen
        // #########################################################################

      case 10:  //select time
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();  // load maintanence select screen
        drawIcon(CI, tft.width() / 2, tft.height() / 8, CIWidth, CIHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

          if (upButton() == true) {
            state = 15;  // go to exit select
            break;
          }
          if (downButton() == true) {
            state = 11;  // go to date select
            break;
          }
          if (checkButton() == true) {
            state = 20;  // go to time adjustment
            break;
          }
        }
        break;

      case 11:  //select date

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();
        drawIcon(Calender, tft.width() / 2, tft.height() / 8, CalenderWidth, CalenderHeight);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            state = 10;  // go to select time
            break;
          }
          if (downButton() == true) {
            state = 12;  // go to select travel offset
            break;
          }
          if (checkButton() == true) {
            state = 30;  // go to date adjustment
            break;
          }
        }
        break;
      case 12:  //select travel offset

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            state = 11;  // go to date select
            break;
          }
          if (downButton() == true) {
            state = 13;  // go to messurement bonds
            break;
          }
          if (checkButton() == true) {
            state = 52;  //go to travel offset adjustment
            break;
          }
        }
        break;
      case 13:  //select messurement bonds

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            state = 12;  // go to travel offset select
            break;
          }
          if (downButton() == true) {
            state = 14;  // go to recalibrate select
            break;
          }
          if (checkButton() == true) {
            state = 40;  // go to messurement bond adjustment
            break;
          }
        }
        break;
      case 14:  //select Recalibrate

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            state = 13;  //go to messurement bond select
            break;
          }
          if (downButton() == true) {
            state = 15;  // go to exit select
            break;
          }
          if (checkButton() == true) {
            //reset everything to the state before the startup cycle
            hasRun = false;
            offsetForward = 0;
            offsetBackward = 0;
            digitalWrite(enPin_X, HIGH);
            digitalWrite(enPin_Y, HIGH);
            state = 1;
            break;
          }
        }
        break;
      case 15:  // select Exit

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        maintenance();
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            state = 14;  // go to recalibrate select
            break;
          }
          if (downButton() == true) {
            state = 10;  // go to time select
            break;
          }
          if (checkButton() == true) {
            state = 0;  // go to the main screen
            break;
          }
        }
        break;
        // #########################################################################
        //  Case for Adjusting the time screen
        // #########################################################################
      case 20:  // adjust 10 hours

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 165, 175, 1);  // used to highlight which number is beeing change
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MTime();  // call Function to show the users the Time value on screen to then be change.

          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            hh = hh + 10;                                  // increase hour by 10
            if (hh > 23) hh = hh - 20;                     // when hour is over 23 itll cycle bacl
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));  // save it in RTC
            state = 20;                                    // everytime the button is pressed itll reload the number
            break;
          }
          if (downButton() == true) {
            hh = hh - 10;  // decrease hour by 10
            if (hh > 23) {
              if (hh >= 245 && hh <= 249) {
                hh = hh + 10;  // when hour is under 09 to 05 itll be cycle back to the 10s
              }
              hh = hh + 20;  // when hour is under 05  itll be cycle back to the 20s
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));  // save it in RTC
            state = 20;                                    // everytime the button is pressed itll reload the number
            break;
          }
          if (checkButton() == true) {
            state = 21;  // when checkbutton is pressed it will go to the 1's hours
            break;
          }
        }
        break;

      case 21:  // adjust 1 hours

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 200, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MTime();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            hh++;                 // increase the hour by 1
            if (hh > 23) hh = 0;  // when its over 23 hour it will cycle back to 00
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 21;
            break;
          }
          if (downButton() == true) {
            hh--;           // decrease the hour by 1
            if (hh > 24) {  // when its under 00 hour it will cycle back to 23
              hh = 23;
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 21;
            break;
          }
          if (checkButton() == true) {
            state = 22;  // when checkbutton is pressed it will go to the 10's minutes
            break;
          }
        }
        break;
      case 22:  // adjust 10 minutes

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 245, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MTime();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            mm = mm + 10;               // increase the minutes by 10
            if (mm > 59) mm = mm - 60;  // when minutes is over 59 itll go back to the 1's
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 22;
            break;
          }
          if (downButton() == true) {
            mm = mm - 10;  //decrease the minuts by 10
            if (mm > 59) {
              mm = mm + 60;  // when minutes is under 00 itll cycle back to the 50's
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 22;
            break;
          }
          if (checkButton() == true) {
            state = 23;  // go to the 1's minute
            break;
          }
        }
        break;
      case 23:  // adjust 1 minutes

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 275, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MTime();

          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            mm++;                 //increase minute by 1
            if (mm > 59) mm = 0;  // when minute is over 59 itll go back to 00
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 23;
            break;
          }
          if (downButton() == true) {
            mm--;           //decrease minute by 1
            if (mm > 60) {  // when minute is under 00 itll go back to 559
              mm = 59;
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
            state = 23;
            break;
          }
          if (checkButton() == true) {
            state = 10;  // go back to time select
            break;
          }
        }
        break;
        // #########################################################################
        //  Case for Adjusting the date screen
        // #########################################################################
      case 30:  // adjust day by 10
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 105, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();  // call Function to show the users the date value on screen to then be change.
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            if (maxDay() == 28) {
              if (dd <= 18) {
                dd = dd + 10;  // increase date by 10
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if ((dd == 19) || (dd == 20)) {
                dd = dd - 10;  // when day is 19 or 20 itll cycle back to 9 or 10 respectively
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if (dd >= 21) {
                dd = dd - 20;  // when day is over 21 itll go back to the 1's
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              state = 30;
              break;
            }
            if (maxDay() == 29) {
              if (dd <= 19) {
                dd = dd + 10;  // increase date by 10
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if (dd == 20) {
                dd = dd - 10;  // when day is  20 itll cycle back to 10
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if (dd >= 21) {
                dd = dd - 20;  // when day is over 21 itll go back to the 1's
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              state = 30;
              break;
            }
            if (maxDay() == 30) {
              if (dd <= 20) {
                dd = dd + 10;  // increase date by 10
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if (dd >= 21) {
                if (dd == 30) {
                  dd = 1;  // when day is over 30 itll go back to 1
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                } else {
                  dd = dd - 20;  // when day is go over 21 itll go back to the 1's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
                }
                state = 30;
                break;
              }
              state = 30;
              break;
            }
            if (maxDay() == 31) {
              if (dd <= 21) {
                dd = dd + 10;  // increase date by 10
                rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                state = 30;
                break;
              }
              if (dd >= 22) {
                if ((dd == 30) || (dd == 31)) {
                  dd = 1;  // when day is 30 or 30 itll go back to 1
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                } else {
                  dd = dd - 20;  // when day is over 22 itll go back to 1's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
                }
                state = 30;
                break;
              }
              state = 30;
              break;
            }
          }
          if (downButton() == true) {
            if (dd <= 10) {
              if (maxDay() == 29) {
                if (dd < 10) {
                  dd = dd + 20;  // if day is under 10 itll go back to 20's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                if (dd == 10) {
                  dd = 20;  // if day is 10 itll go back to 20
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
                }
                state = 30;
                break;
              }
              if (maxDay() == 28) {
                if (dd < 9) {
                  dd = dd + 20;  // if day is under 9 itll go back to 20's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                if (dd == 10) {
                  dd = 20;  // if day is 10 then itll go to 20
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));
                }
                state = 30;
                break;
              }
              if (maxDay() == 30) {
                if (dd == 10) {
                  dd = maxDay();  // if day is 10 itll go to the maxday
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                if (dd >= 1 && dd < 10) {
                  dd = dd + 20;  //if day is between 1 and 10 itll go back to 20's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                state = 30;
                break;
              }
              if (maxDay() == 31) {
                if (dd == 10) {
                  dd = 30;  // if day is 10 itll go to 30
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                if (dd > 1 && dd < 10) {
                  dd = dd + 20;  //if day is between 1 and 10 itll go back to 20's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                if (dd <= 1) {
                  dd = dd + 30;  // when day in under or equal to 1 itll go to 30's
                  rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

                  state = 30;
                  break;
                }
                state = 30;
                break;
              }
            } else {
              dd = dd - 10;  // decrease day by 10
              rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

              state = 30;
              break;
            }
          }
          if (checkButton() == true) {
            state = 31;  // go to 1's day
            break;
          }
        }
        break;
      case 31:  // adjust day by 1

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF28);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 140, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();  // call Function to show the users the date value on screen to then be change.
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            dd++;  // increase day by 1
            if (dd > maxDay()) {
              dd = 1;  // when dd is over the maxday itll go back to 1
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 31;
            break;
          }
          if (downButton() == true) {
            dd--;
            if (dd < 1) {
              dd = maxDay();  // when dd is less than 1 itll go back to the max day
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 31;
            break;
          }
          if (checkButton() == true) {
            state = 32;  // go to 10's month
            break;
          }
        }
        break;
      case 32:  // adjust month by 10

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 202, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            if (MM > 10) {
              MM = MM - 10;  // when month is over 10 itll go back to 1's
              if (oldmd == dd) {
                dd = maxDay();  // when day is at maxday itll update the day to the new maxday
                oldmd = dd;
              }
              rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

              state = 32;
              break;
            }
            if (MM <= 2) {
              MM = MM + 10;  //when month is less or equal than 2 then iitll go to 10's
              if (oldmd == dd) {
                dd = maxDay();
                oldmd = dd;
              }
              rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

              state = 32;
              break;
            }
          }
          if (downButton() == true) {
            if (MM >= 11) {
              MM = MM - 10;  //when month is greater or equal to 11 itll go back to 1's
              if (oldmd == dd) {
                dd = maxDay();
                oldmd = dd;
              }
              rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

              state = 32;
              break;
            }
            if (MM <= 2) {
              MM = MM + 10;  //when month is less or equal to 2 itll go to the 10's
              if (oldmd == dd) {
                dd = maxDay();
                oldmd = dd;
              }
              rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

              state = 32;
              break;
            }
          }
          if (checkButton() == true) {
            state = 33;
            break;
          }
        }
        break;
      case 33:  // adjust month by 1

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 237, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            MM++;                     //increase months by 1
            if (MM > 12) { MM = 1; }  // when month is over 12 itll go to 1
            if (oldmd == dd) {
              dd = maxDay();
              oldmd = dd;
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 33;
            break;
          }
          if (downButton() == true) {
            MM--;          //decrease month by 1
            if (MM < 1) {  // when month is less then 1 itll go back to 12
              MM = 12;
            }
            if (oldmd == dd) {
              dd = maxDay();
              oldmd = dd;
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 33;
            break;
          }
          if (checkButton() == true) {
            state = 34;  // go to 10 years
            break;
          }
        }
        break;
      case 34:  //adjust year 10's

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 300, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            yy = yy + 10;             //increase year by 10
            if (yy > 99) { yy = 0; }  // when year is over 99 itll fo to 00
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 34;
            break;
          }
          if (downButton() == true) {
            yy = yy - 10;   //decrease year by 10
            if (yy > 99) {  //when year is less then 0 itll go back to 99
              yy = 99;
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 34;
            break;
          }
          if (checkButton() == true) {
            state = 35;
            break;
          }
        }
        break;
      case 35:  //adjust year 1's

        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);

        tft.setFreeFont(FF27);
        tft.drawChar('_', 335, 175, 1);
        tft.setFreeFont(GLCD);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          MDate();
          drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
          if (upButton() == true) {
            yy++;                     //increase year by 1
            if (yy > 99) { yy = 0; }  //when year is over 99 itll go back to 0
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 35;
            break;
          }
          if (downButton() == true) {
            yy--;  //decrease year by 1
            if (yy > 99) {
              yy = 99;  //when year is less then 0 itll go back to 99
            }
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 35;
            break;
          }
          if (checkButton() == true) {
            rtc.adjust(DateTime(yy, MM, dd, hh, mm, ss));

            state = 11;  // go bacck to date select
            break;
          }
        }
        break;
        //#######################################################//
        //             messurement bounds screen SETUP           //
      //#######################################################//
      case 40:  //Upper bounds Y axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        messurementBounds();  // call Function to show the users which messurement bound needs to be change on screen.
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 45;  // go to back
            break;
          }
          if (downButton() == true) {
            state = 41;  // go toLower bounds Y axis
            break;
          }
          if (checkButton() == true) {
            state = 46;   // go to Upper bounds Y axis adjustment
            Y_As = true;  //raise y_axis flag
            X_As = false;
            forward = true;  //raise forward flag, in this case it is used to indicate the upperbound
            backward = false;
            bounds = upperbound_Y;  // load the upperbound of the y_axis to the generic bounds
            break;
          }
        }
        break;
      case 41:  //Lower bounds Y axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        messurementBounds();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();

          if (upButton() == true) {
            state = 40;  // go to Upper bounds Y axis
            break;
          }
          if (downButton() == true) {
            state = 42;  // go to Upper bounds X axis
            break;
          }
          if (checkButton() == true) {
            state = 46;   //go to lower bounds Y axis adjustment
            Y_As = true;  //raise y axis flag
            X_As = false;
            forward = false;
            backward = true;        //raise backward flag, in this case it is used to indicate the lowerbound
            bounds = lowerbound_Y;  // load the lowerbound of the y_axis to the generic bounds

            break;
          }
        }
        break;
      case 42:  //Upper bounds X axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        messurementBounds();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();

          if (upButton() == true) {
            state = 41;  // go to Lower bounds Y axis
            break;
          }
          if (downButton() == true) {
            state = 43;  // go to Lower bounds X axis
            break;
          }
          if (checkButton() == true) {
            state = 46;  //go to upper bounds X axis adjustment
            Y_As = false;
            X_As = true;     //raise X axis flag
            forward = true;  //raise forward flag
            backward = false;
            bounds = upperbound_X;  // load the upperbound of the x_axis to the generic bounds
            break;
          }
        }
        break;

      case 43:  //Lower bounds X axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        messurementBounds();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();

          if (upButton() == true) {
            state = 42;  //  go to Upper bounds X axis
            break;
          }
          if (downButton() == true) {
            state = 45;  // go to Back
            break;
          }
          if (checkButton() == true) {
            state = 46;  //go to Lower bounds X axis adjustment
            Y_As = false;
            X_As = true;  //raise X axis flag
            forward = false;
            backward = true;        //raise backward flag
            bounds = lowerbound_Y;  // load the lowerbound of the y_axis to the generic bounds
            break;
          }
        }
        break;

      case 45:  //Back
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        messurementBounds();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();

          if (upButton() == true) {
            state = 43;  //go to Lower bounds X axis
            break;
          }
          if (downButton() == true) {
            state = 40;  // go to Upper bounds Y axis
            break;
          }
          if (checkButton() == true) {
            state = 13;  // go to messurement bound select
            //reset everything
            Y_As = false;
            X_As = false;
            forward = false;
            backward = false;
            break;
          }
        }
        break;
        //#######################################################//
        //            Messurements bound                          //
        //#######################################################//
      case 46:  //1N
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', (tft.width() / 2) - 45, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Bounds();  // call Function to show the users the bound value on screen to then be change.
          Serial.println(bounds);
          if (upButton() == true) {
            if (bounds >= 9) {
              bounds = bounds - 9;  // when the bound is less or equal to 9 itll go back to 0
            } else {
              bounds++;  // increase bound by 1
            }

            state = 46;
            break;
          }
          if (downButton() == true) {
            if (bounds < 1) {
              bounds = bounds + 9;  //when bound is less than 1 itll go to 9
            } else {
              bounds--;  //decrease bound by 1
            }

            state = 46;
            break;
          }
          if (checkButton() == true) {
            state = 47;  // go to 0.1N
            break;
          }
        }
        break;
      case 47:  //0.1N
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', (tft.width() / 2), 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Bounds();
          Serial.println(bounds);
          if (upButton() == true) {
            if (bounds >= 9.90) {
              bounds = bounds - 9.90;  // when bound is greater or equal to 9.9 itll go back to 0.0
            } else {
              bounds = bounds + 0.10;  // increase bound by 0.1
            }

            state = 47;
            break;
          }
          if (downButton() == true) {
            if (bounds < 0.1) {
              bounds = bounds + 9.9;  // when bound is less than 0.1 itll go back to 9.9
            } else {
              bounds = bounds - 0.10;  // decrease bound by 0.1
            }

            state = 47;
            break;
          }
          if (checkButton() == true) {
            state = 48;  // go to 0.01N
            break;
          }
        }

        break;
      case 48:  //0.01N
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', (tft.width() / 2) + 35, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Bounds();
          Serial.println(bounds);
          if (upButton() == true) {
            if (bounds > 9.98) {
              bounds = 0.00;  // when bound is greater than 9.8 itll go back to 0
            } else {
              bounds = bounds + 0.01;  // increase bound by 0.01
            }
            state = 48;
            break;
          }
          if (downButton() == true) {
            if (bounds < 0.01) {
              bounds = 9.99;  //when bound is less than 0.01 itll go back to 9.99
            } else {
              bounds = bounds - 0.01;  // decrease bound by 0.01
            }

            state = 48;
            break;
          }
          if (checkButton() == true) {
            //save the value to sd card, so it can be used next time the system startup
            if (Y_As) {
              if (forward) {
                state = 40;                                        // go back to Upper bounds Y axis
                upperbound_Y = bounds;                             // save the change bound to its respective variable
                fileName = "upperbound_Y.txt";                     // name the file where the value is save
                myFile = SD.open("upperbound_Y.txt", FILE_WRITE);  //open the file
              } else if (backward) {
                state = 41;  // go back to lower bounds Y axis
                lowerbound_Y = bounds;
                fileName = "lowerbound_Y.txt";
                myFile = SD.open("lowerbound_Y.txt", FILE_WRITE);
              }
            } else if (X_As) {
              if (forward) {
                state = 42;  // go back to Upper bounds x axis
                upperbound_X = bounds;
                fileName = "upperbound_X.txt";
                myFile = SD.open("upperbound_X.txt", FILE_WRITE);
              } else if (backward) {
                state = 43;  // go back to lower bounds x axis
                lowerbound_X = bounds;
                fileName = "lowerbound_X.txt";
                myFile = SD.open("lowerbound_X.txt", FILE_WRITE);
              }
            }
            if (myFile) {
              Serial.print("Writing to .txt ...");
              if (Y_As) {
                if (forward) {
                  myFile.println(upperbound_Y);  // write the value into the textfile
                } else if (backward) {
                  myFile.println(lowerbound_Y);
                }
              } else if (X_As) {
                if (forward) {
                  myFile.println(upperbound_X);

                } else if (backward) {
                  myFile.println(lowerbound_X);
                }
              }

              // close the file:
              myFile.close();
              Serial.println("done.");
            } else {
              // if the file didn't open, print an error:
              Serial.println("error opening .txt");
            }
            SdCardRead();
            break;
          }
        }
        break;

        //#######################################################//
      //             traveloffset screen SETUP                  //
      //#######################################################//
      case 52:  // select travel offset forward Y axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        TravelOffsets();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 56;  // select back
            break;
          }
          if (downButton() == true) {
            state = 53;  //select travel offset backward Y axis
            break;
          }
          if (checkButton() == true) {
            state = 57;   //1000
            Y_As = true;  // raise flag that offest Y need to be change
            X_As = false;
            forward = true;  // raise flag that forward offset need to be change
            backward = false;
            TravelOffset = TravelOffsetForward_Y;  //load value to generic travel offset
            break;
          }
        }
        break;
      case 53:  // select travel offset backward Y axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        TravelOffsets();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);


        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 52;  // select travel offset forward Y axis
            break;
          }
          if (downButton() == true) {
            state = 54;  // select travel offset forward X axis
            break;
          }
          if (checkButton() == true) {
            state = 57;   //1000
            Y_As = true;  // raise flag that offest Y need to be change
            X_As = false;
            forward = false;
            backward = true;                        // raise flag that backward offset need to be change
            TravelOffset = TravelOffsetBackward_Y;  //load value to generic travel offset
            break;
          }
        }
        break;
      case 54:  // select travel offset forward X axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        TravelOffsets();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 53;
            break;
          }
          if (downButton() == true) {
            state = 55;
            break;
          }
          if (checkButton() == true) {
            state = 57;  //1000
            Y_As = false;
            X_As = true;     // raise flag that offest x need to be change
            forward = true;  // raise flag that forward offset need to be change
            backward = false;
            TravelOffset = TravelOffsetForward_X;  //load value to generic travel offset
            break;
          }
        }
        break;
      case 55:  // select travel offset backward x axis
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        TravelOffsets();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 54;
            break;
          }
          if (downButton() == true) {
            state = 56;
            break;
          }
          if (checkButton() == true) {
            state = 57;  //1000
            Y_As = false;
            X_As = true;  // raise flag that offest Y need to be change
            forward = false;
            backward = true;                        // raise flag that backward offset need to be change
            TravelOffset = TravelOffsetBackward_X;  //load value to generic travel offset
            break;
          }
        }
        break;
      case 56:  // select back
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        TravelOffsets();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (upButton() == true) {
            state = 61;
            break;
          }
          if (downButton() == true) {
            state = 60;
            break;
          }
          if (checkButton() == true) {
            state = 12;  //travel offset select
            // flag reset
            Y_As = false;
            X_As = false;
            forward = false;
            backward = false;
            break;
          }
        }
        break;
      //#######################################################//
      //             traveloffset SETUP                        //
      //#######################################################//
      case 57:  //travel offset 1000
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 165, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Tr_Offsets();
          Serial.println(TravelOffset);
          if (upButton() == true) {
            if (TravelOffset >= 9000) {
              TravelOffset = TravelOffset - 9000;  // when offet is over 9000, reset to 100's
            } else {
              TravelOffset = TravelOffset + 1000;  // increase offset by 1000
            }

            state = 57;
            break;
          }
          if (downButton() == true) {
            if (TravelOffset < 1000) {
              TravelOffset = TravelOffset + 9000;  // when offset is less than 1000 go to 9000
            } else {
              TravelOffset = TravelOffset - 1000;  // decrease offset by 1000
            }

            state = 57;
            break;
          }
          if (checkButton() == true) {
            state = 58;
            break;
          }
        }
        break;
      case 58:  //travel offset 100
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 195, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Tr_Offsets();
          Serial.println(TravelOffset);
          if (upButton() == true) {
            if (TravelOffset >= 9900) {
              TravelOffset = TravelOffset - 900;  //when offset is over 9900 go to 9000
            } else {
              TravelOffset = TravelOffset + 100;  //increase offset by 100
            }

            state = 58;
            break;
          }
          if (downButton() == true) {
            if (TravelOffset < 100) {
              TravelOffset = TravelOffset + 900;  // when offset is bellow 100 go to 900
            } else {
              TravelOffset = TravelOffset - 100;  // decrease offset by 100
            }

            state = 58;
            break;
          }
          if (checkButton() == true) {
            state = 59;
            break;
          }
        }
        break;
      case 59:  //travel offset 10
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 225, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Tr_Offsets();
          Serial.println(TravelOffset);
          if (upButton() == true) {
            if (TravelOffset >= 9990) {
              TravelOffset = TravelOffset - 90;  // when offset is over 9990 go to 9900
            } else {
              TravelOffset = TravelOffset + 10;  //increase offset by 10
            }

            state = 59;
            break;
          }
          if (downButton() == true) {
            if (TravelOffset < 10) {
              TravelOffset = TravelOffset + 90;  // when offset is less then 10 go to 90
            } else {
              TravelOffset = TravelOffset - 10;  // decrease offset by 10
            }

            state = 59;
            break;
          }
          if (checkButton() == true) {
            state = 60;
            break;
          }
        }
        break;
      case 60:  //travel offset 1
        erase();
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        tft.setFreeFont(FF27);
        tft.drawChar('_', 255, 175, 1);
        tft.setFreeFont(GLCD);
        draw = true;

        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          Tr_Offsets();
          Serial.println(TravelOffset);
          if (upButton() == true) {
            if (TravelOffset >= 9) {
              TravelOffset = TravelOffset - 9;  // when offset is higher than 9 go to 1
            } else {
              TravelOffset++;  // increase offset by 1
            }

            state = 60;
            break;
          }
          if (downButton() == true) {
            if (TravelOffset < 1) {
              TravelOffset = TravelOffset + 9;  // when offset is lower than 1 go to 9
            } else {
              TravelOffset--;  // decrease offset by 1
            }

            state = 60;
            break;
          }
          if (checkButton() == true) {
            //save the value to sd card, so it can be used next time the system startup
            if (Y_As) {
              if (forward) {
                state = 52;                                                 // go back to travel offset Y axis
                TravelOffsetForward_Y = TravelOffset;                       // save the change bound to its respective variable
                fileName = "TravelOffsetForward_Y.txt";                     // name the file where the value is save
                myFile = SD.open("TravelOffsetForward_Y.txt", FILE_WRITE);  //open the file
              } else if (backward) {
                state = 53;  // go back to backward offset Y axis
                TravelOffsetBackward_Y = TravelOffset;
                fileName = "TravelOffsetBackward_Y.txt";
                myFile = SD.open("TravelOffsetBackward_Y.txt", FILE_WRITE);
              }
            } else if (X_As) {
              if (forward) {
                state = 54;  // go back to forward offset x axis
                TravelOffsetForward_X = TravelOffset;
                fileName = "TravelOffsetForward_X.txt";
                myFile = SD.open("TravelOffsetForward_X.txt", FILE_WRITE);
              } else if (backward) {
                state = 55;  // go back to backward offset x axis
                TravelOffsetBackward_X = TravelOffset;
                fileName = "TravelOffsetBackward_X.txt";
                myFile = SD.open("TravelOffsetBackward_X.txt", FILE_WRITE);
              }
            }
            if (myFile) {
              Serial.print("Writing to .txt ...");
              if (Y_As) {
                if (forward) {
                  myFile.println(TravelOffsetForward_Y);  // write the value into the textfile
                } else if (backward) {
                  myFile.println(TravelOffsetBackward_Y);
                }
              } else if (X_As) {
                if (forward) {
                  myFile.println(TravelOffsetForward_X);

                } else if (backward) {
                  myFile.println(TravelOffsetBackward_X);
                }
              }

              // close the file:
              myFile.close();
              Serial.println("done.");
            } else {
              // if the file didn't open, print an error:
              Serial.println("error opening .txt");
            }
            SdCardRead();

            break;
          }
        }
        break;
      case 100:  //error screen

        erase();
        errorScreen();
        drawIcon(MST, (tft.width() - MSTWidth), (tft.height() - MSTHeight), MSTWidth, MSTHeight);
        tft.fillRect(0, 0, 480, 30, TFT_DARKGREY);
        while (1) {
          analogRead(buttonPin1);
          analogRead(buttonPin2);
          analogRead(buttonPin3);
          /*
          Serial.print("checkbutton=");
          if (checkButton()) {
            Serial.println("true");
          } else {
            Serial.println("False");
          }
          Serial.print("downbutton=");
          Serial.println(analogRead(buttonPin2));
          Serial.print("upbutton=");
          Serial.println(analogRead(buttonPin3));
*/
          topbar();
          if (checkButton() == true) {
            hasRun = false; // reset startup flag
            overload = false; // reset overload flag
            offsetForward = 0;// reset offset forward
            offsetBackward = 0;//reset offset backward
            //turn off motor
            digitalWrite(enPin_X, HIGH);
            digitalWrite(enPin_Y, HIGH);
            state = 0;// go back to main screen
            break;
          }
        }
        break;
    }
  }
}
