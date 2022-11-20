#include <max6675.h>
#include <Arduino.h>
#include <TM1637.h>  //https://github.com/Seeed-Studio/Grove_4Digital_display
#include <Servo.h>



// time
unsigned long previousMillis = 0UL;
unsigned long interval = 1000UL;
bool blink;

// termo
int termoPinSO = 4;
int termoPinCS = 5;
int termoPinSCK = 6;
MAX6675 termo(termoPinSCK, termoPinCS, termoPinSO);


// displayRed
#define displayRedPinCLK 2
#define displayRedPinDIO 3
TM1637 displayRed(displayRedPinCLK, displayRedPinDIO);


// displayWhite
#define displayWhitePinCLK 10
#define displayWhitePinDIO 11
TM1637 displayWhite(displayWhitePinCLK, displayWhitePinDIO);


// poten
#define potenPinOTA A0
//#define potenPinOTB A1 // NOT IN USE


// servo
#define servoPin 9
Servo servo;
int pos = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("Communication has been stared.");

  // servo
  servo.attach(servoPin);
  servo.write(pos);
  delay(1000);
  servo.write(0);

  // temp
  float temp = termo.readCelsius();
  int intTemp = 0;
  int lastTemp = 0;
  Serial.print("temp: ");
  Serial.println(temp);

  // displayRed
  displayRed.init();
  displayRed.set(2);  //set brightness; 0-7
  displayRedNumber(000, 0);


  // displayWhite
  displayWhite.init();
  displayWhite.set(2);  //set brightness; 0-7
  displayWhiteNumber(000, 1);

  // poten
  float poten = analogRead(potenPinOTA);
  int intPoten = 0;
  int prevIntPoten;
  delay(1500);
}

void displayRedNumber(int num, char ch) {
  displayRed.display(3, ch);
  displayRed.display(2, num % 10);
  displayRed.display(1, num / 10 % 10);
  displayRed.display(0, num / 100 % 10);
  //displayRed.displayRed(0, num / 1000 % 10);
}

void displayWhiteNumber(int num, char ch) {
  displayWhite.display(3, ch);
  displayWhite.display(2, num % 10);
  displayWhite.display(1, num / 10 % 10);
  displayWhite.display(0, num / 100 % 10);
  //displayRed.displayRed(0, num / 1000 % 10);
}

int servoPos(int temp, int poten) {
  //angle = 0
  int koef = 20;  //half-range of tolerance for levele of partilly open/close
  if (poten > 250.0) { poten = 250.0; }  //safety limit
  
  if (temp > (poten + koef)) {
    return 90;
  } else if (temp >= poten) {
    return 60;
  } else if (temp > (poten - koef)) {
    return 30;
  } else {
    return 0;
  }
}

/*
8: 250c
6: 180c
3: 100c
0:  10c

settings  temperature (C)
8 250
7 215
6 180
5 153
4 127
3 100
2 70
1 40
0 10

*/


int readPoten() {
  float poten = analogRead(potenPinOTA) / 4; //TESTING!
  int intPoten = (int)(poten + 0.5f);
  intPoten = intPoten - (intPoten % 10);
  if (intPoten <= 0 ) { return 0; }
  return intPoten;
}

void loop() {
  bool flag;  

  // temp
  float temp = termo.readCelsius();
  Serial.print("temp: ");
  Serial.println(temp);
  int intTemp = (int)(temp + 0.5f);
  int lastTemp;
  Serial.println(lastTemp);

  // poten
  int intPoten = 0;
  int poten = 0;
  intPoten = readPoten();

  Serial.print("poten(0-1024): ");
  Serial.println(intPoten);

  // servo
  int pos = servoPos(intTemp, intPoten);
  servo.write(pos);
  Serial.print("pos: ");
  Serial.println(pos);

  // displays
  if (abs(intTemp - lastTemp) <= 5) {
    intTemp = lastTemp;
    flag = 1;
  } else { flag = 0; }
  

  int c;  //DOES NOT WORK
  //bool blink;
  if (blink) {
    blink = false;
    c = 12;
    }
  else {
    blink = true;
    c = 14;
    }

  displayRedNumber(intTemp, 12);
  displayWhiteNumber(readPoten(), 12);
  
  delay(500);

  if (flag) { lastTemp = readPoten(); }
  else { lastTemp = intTemp; }
}
