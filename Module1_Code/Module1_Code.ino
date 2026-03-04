#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

// ===== Servo Setup =====
Servo servo1;
Servo servo2;

// ===== Ultrasonic Sensor 1 (Lid Detection) =====
const int trigPin1 = 6;
const int echoPin1 = 5;

// ===== Ultrasonic Sensor 2 (Trash Level) =====
const int trigPin2 = A0;
const int echoPin2 = A1;

// ===== LED Pins =====
const int greenLED = 2;
const int redLED = A2;

// ===== 🔔 Buzzer Pin =====
const int buzzerPin = 7;

// ===== LCD Setup =====
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// ===== DFPlayer Mini Setup =====
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// ===== Trash Level Constants =====
const int MAX_DISTANCE = 26;
const int MIN_DISTANCE = 3;

bool dfPlayerOK = false;
bool lidOpen = false;
int lastLevelRange = -1;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  servo1.attach(3);
  servo2.attach(4);

  // Start with lid closed
  servo1.write(90);
  servo2.write(90);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");
  delay(2000);

  // DFPlayer Setup
  mySoftwareSerial.begin(9600);
  if (myDFPlayer.begin(mySoftwareSerial)) {
    dfPlayerOK = true;
    myDFPlayer.volume(30);
    lcd.clear();
    lcd.print("DFPlayer OK");
    delay(1000);
  } else {
    dfPlayerOK = false;
    lcd.clear();
    lcd.print("DFPlayer Error");
    delay(2000);
  }

  lcd.clear();
  lcd.print("Smart Waste Bin");
  if (dfPlayerOK) myDFPlayer.play(1);
  delay(3000);
  lcd.clear();

  lcd.print("  BSIT - 4A");
  if (dfPlayerOK) myDFPlayer.play(2);
  delay(3000);
  lcd.clear();
}

void loop() {

  // ===== Read Ultrasonic 1 (Lid) =====
  long duration1 = measureDistance(trigPin1, echoPin1);
  int distance1 = (duration1 / 2) / 29.1;

  if (distance1 == 0) distance1 = 999;   // prevent false triggers

  // ===== Read Ultrasonic 2 (Trash Level) =====
  long duration2 = measureDistance(trigPin2, echoPin2);
  int distance2 = (duration2 / 2) / 29.1;

  int fillPercent = map(distance2, MAX_DISTANCE, MIN_DISTANCE, 0, 100);
  fillPercent = constrain(fillPercent, 0, 100);

  lcd.clear();

  // ===== LID CONTROL =====
  if (distance1 <= 10 && !lidOpen) {
    openLid();
  } else if (distance1 > 10 && lidOpen) {
    closeLid();
  }

  // ===== Trash Level Display =====
  int levelRange;
  if (fillPercent >= 100) levelRange = 5;
  else if (fillPercent >= 75) levelRange = 4;
  else if (fillPercent >= 50) levelRange = 3;
  else if (fillPercent >= 30) levelRange = 2;
  else if (fillPercent >= 10) levelRange = 1;
  else levelRange = 0;

  lcd.setCursor(0, 1);
  switch (levelRange) {
    case 5: lcd.print("100% Trash Full"); break;
    case 4: lcd.print("75% Trash Full "); break;
    case 3: lcd.print("50% Trash Full "); break;
    case 2: lcd.print("30% Trash Full "); break;
    case 1: lcd.print("10% Trash Full "); break;
    case 0: lcd.print("   Bin Empty!  "); break;
  }

  // ===== Buzzer =====
  if (levelRange == 5) digitalWrite(buzzerPin, HIGH);
  else digitalWrite(buzzerPin, LOW);

  // ===== DFPlayer =====
  if (levelRange != lastLevelRange && dfPlayerOK) {
    switch (levelRange) {
      case 5: myDFPlayer.play(5); break;
      case 4: myDFPlayer.play(6); break;
      case 3: myDFPlayer.play(7); break;
      case 2: myDFPlayer.play(8); break;
      case 1: myDFPlayer.play(9); break;
      case 0: myDFPlayer.play(10); break;
    }
    lastLevelRange = levelRange;
  }

  delay(300);
}

// ===== Measure Distance =====
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH, 20000); // timeout added
}

// ===== SERVO CONTROL =====
void openLid() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("      OPEN");

  if (dfPlayerOK) myDFPlayer.play(3);

  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);

  // Move servos to OPEN position
  servo1.write(0);
  servo2.write(0);
  delay(3000);

  lidOpen = true;
}

void closeLid() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("      CLOSE");

  if (dfPlayerOK) myDFPlayer.play(4);

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);

  // Move servos to CLOSED position
  servo1.write(90);
  servo2.write(90);
  delay(2000);

  lidOpen = false;
}