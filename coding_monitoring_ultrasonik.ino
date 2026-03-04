// -------- Ultrasonic --------
const int trigPin = 12;
const int echoPin = 13;
long duration;
int distanceCm;

// -------- Gas Sensor (AO) --------
const int gasPin = A0;
int gasRaw;
int gasPercent;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // LEVEL
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * 0.034 / 2;
  if (distanceCm > 25) distanceCm = 25;

  // GAS
  gasRaw = analogRead(gasPin);

  // Convert to percentage (0–100)
  gasPercent = map(gasRaw, 0, 1023, 0, 100);
  if (gasPercent > 100) gasPercent = 100;
  if (gasPercent < 0) gasPercent = 0;

  // SEND DATA (SEPARATE LINES)
/*   Serial.print("LEVEL: "); */
  Serial.print(distanceCm);
  Serial.println(" cm");

/*   Serial.print("GAS: "); */
  Serial.print(gasPercent);
  Serial.println(" %");

  delay(2000);
}
