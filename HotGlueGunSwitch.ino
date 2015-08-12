/*
  HotGlueSwitch

  This timer turns an Hot Glue Gun or any other appliance on for a time (timer2) and switches it off afterwards.
  For Hot Glue guns a "readiness warning can be built in: after a time (WARNING_DURATION) a beeper announces, that the temperature should be ok to start working.
  Before the timer1 switches the gun off it warns for one minute. During this time, pressing the button extends the time.

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


// Pin1 RESET  #define DEBUG1 PB5 // Pin1, nur mit Fuse change

#define SWITCH PB3   // Pin2

#define LED    PB4    // Pin3
// Pin4 GND
#define PROGRAMMER PB0    // Pin5
#define BUZZER PB1   // Pin6
#define RELAIS PB2    // Pin7
// Pin8 VCC
/*
#define SWITCH 3
#define LED    4
#define PROGRAMMER 5
#define BUZZER 6
#define RELAIS 7

*/
const long timer2 = 600;
const long timer1 = 140; // 140 sec;
const long WARNING_DURATION = 60000;  // 60sec;
const long READY_DURATION = 3000;
long address1 = 0;
long address2 = 4;

byte status = 1;
long timeStat;

void setup() {
  //  OSCCAL =  0x70; // Fuse 0x71 (4.8 MHz, no divide)
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(PROGRAMMER, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RELAIS, OUTPUT);
  digitalWrite(SWITCH, HIGH); //
  digitalWrite(LED, OUT_OFF); //
  digitalWrite(BUZZER, OUT_OFF); //
  digitalWrite(RELAIS, OUT_ON); //

  timer1 = EEPROMReadlong(address1);
  timer2 = EEPROMReadlong(address2);
  timeStat = millis();
  if (digitalRead(PROGRAMMER)) progTimer();
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
      if (timeOut(timeStat, timer2 )) status = 4;  // Appliance off
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
      if (timeOut(timeStat, ((timer1 + READY_DURATION) * FACTOR))) status = 6;
      if (keyPressed()) status = 3;   // time extension
      break;

    case 6:
      // Status after Appliance ready: Appliance still on, but no more warning for readiness
      digitalWrite(RELAIS, OUT_ON);
      digitalWrite(LED, OUT_ON);
      if (timeOut(timeStat, (timer2 - WARNING_DURATION))) status = 2;
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
  if (millis() - zeitEin >= dauer) {
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

void progTimer() {

  while (!digitalRead(PROGRAMMER)) {
    digitalWrite(LED), !digitalRead(LED)); // wait till jumper closed
    delay(100);
  }
  long timer1 = millis() - timeStat;
  if (timer1 > 5000) EEPROMWritelong(adress1, timer1); // time 1
  else EEPROMWritelong(adress1, 0); // no warning signal

  while (digitalRead(PROGRAMMER)) {
    digitalWrite(LED), !digitalRead(LED)); // wait till jumper open
    delay(100);
  }
  long timer2 = millis() - timeStat;
  EEPROMWritelong(adress2, timer2); // time 2


}

