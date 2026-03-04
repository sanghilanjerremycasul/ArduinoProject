#include <SoftwareSerial.h>
SoftwareSerial bt(8, 9); // RX, TX (Bluetooth)

// Ultrasonic pins
#define trigPin 6
#define echoPin 7

// Gas sensor pin
#define gasPin A0

long duration;
int distance;
int gasValue;

void setup() {
  Serial.begin(9600);
  bt.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(gasPin, INPUT);

  Serial.println("System Ready");
}

void loop() {

  // ---------- Ultrasonic Sensor ----------
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // cm

  // ---------- Gas Sensor ----------
  gasValue = analogRead(gasPin);

  // ---------- Send to MIT App Inventor ----------
  bt.print(distance);   // Ultrasonic distance
  bt.print(";");
  bt.print(gasValue);   // Gas sensor value
  bt.println(";");

  // ---------- Serial Monitor ----------
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Gas: ");
  Serial.println(gasValue);

  delay(1000);
}
