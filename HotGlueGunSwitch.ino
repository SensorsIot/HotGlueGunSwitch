/*
  HotGlueSwitch

  This timer turns an Hot Glue Gun or any other appliance on for a time (timer2) and switches it off afterwards.
  For Hot Glue guns a "readiness warning can be built in: after a time timer1  a beeper announces for READY_DURATION, that the temperature should be ok to start working.
  Before the timer2 switches the gun off it warns for WARNING_DURATION. During this time, pressing the button extends the time for another timer2 duration.

  It runs on an ATTINY85

  This example code is in the public domain.

  published August, 1st 2015 by Andreas Spiess
 */
 
#include <avr/io.h>
#include <EEPROM.h>


#define SW_OPEN     HIGH
#define SW_CLOSED   LOW
#define OUT_ON		HIGH
#define OUT_OFF		LOW

#define FACTOR 1000


// Pin1 RESET  #define DEBUG1 PB5 // Pin1, nur mit Fuse change

#define SWITCH PB3   // Pin2

#define LED    PB4    // Pin3
// Pin4 GND
#define PROGRAMMER PB0    // Pin5
#define BUZZER PB1   // Pin6
#define RELAIS PB2    // Pin7
// Pin8 VCC

const int timer2 = 600;   // 600 sec = 10min
const int timer1 = 140; // 140 sec (until glue is hot enough;
const int WARNING_DURATION = 60;  // 60sec;
const int READY_DURATION = 3;   // 3 sec

byte status = 1;
long timeStat;

void setup() {
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(PROGRAMMER, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAIS, OUTPUT);
  digitalWrite(SWITCH, HIGH); //
  digitalWrite(LED, OUT_OFF); //
  digitalWrite(BUZZER, OUT_OFF); //
  digitalWrite(RELAIS, OUT_ON); //
  timeStat = millis();
  delay(200);

}


void loop () {
  switch (status) {

    case 1:
      // Appliance on
      digitalWrite(RELAIS, OUT_ON);
      digitalWrite(LED, OUT_ON);
      if (timeOut(timeStat, timer1))  status = 5; // Appliance ready
      break;

    case 2:
      // Warning
      beep(200);
      smartDelay(2000);
      if (keyPressed())  status = 3;
      if (timeOut(timeStat, timer2)) status = 4;  // Appliance off
      break;

    case 3:
      // Time Extension
      timeStat = millis();   // reset timer to start duration over
      for (int ii = 0; ii < 3; ii++) {   ///three beeps for confirmation
        beep(100);
        delay(100);
      }
      timeStat = millis();
      status = 6;
      break;

    case 4:
      // Appliance off
      digitalWrite(RELAIS, OUT_OFF);
      digitalWrite(LED, OUT_OFF);
      digitalWrite(BUZZER, OUT_OFF);
      while (1 == 1);  // wait forever
      break;

    case 5:
      // Appliance ready
      digitalWrite(RELAIS, OUT_ON);
      digitalWrite(LED, OUT_ON);
      beep(200);
      smartDelay(100);
      if (timeOut(timeStat, (timer1 + READY_DURATION))) status = 6;
      if (keyPressed()) status = 3;   // time extension
      break;

    case 6:
      // Status after Appliance ready: Appliance still on, but no more warning for readiness
      digitalWrite(RELAIS, OUT_ON);
      digitalWrite(LED, OUT_ON);
      if (timeOut(timeStat, timer2 - WARNING_DURATION)) status = 2;
      if (keyPressed()) status = 3;   // time extension
      break;

    default:  // should never happen...
      status = 1;
      break;
  }
  delay(100);
}

boolean timeOut(long zeitEin, long dauer) {
  boolean timeOut = false;
  if (millis() - zeitEin >= dauer * FACTOR) {
    timeOut = true;
  }
  return timeOut;
}


boolean keyPressed() {
  bool tt = false;
  if (digitalRead(SWITCH) == SW_CLOSED) {
    tt = true;
    while (digitalRead(SWITCH) == SW_CLOSED) {
      delay(50);
    }
  }
  return tt;
}

void smartDelay(int delay) {
  unsigned long start = millis();
  while (millis() - start < delay) {
    if (digitalRead(SWITCH) == SW_CLOSED)  break;
  }
}


void beep(int dauer) {
  digitalWrite(BUZZER, OUT_ON);
  digitalWrite(LED, OUT_OFF);
  smartDelay(dauer);
  digitalWrite(BUZZER, OUT_OFF);
  digitalWrite(LED, OUT_ON);
}


