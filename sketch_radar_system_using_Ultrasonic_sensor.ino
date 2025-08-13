// ESP32 radar sweep with HC-SR04 + 9g servo
// Prints: angle,distance_cm per line

#include <ESP32Servo.h>

// -------- Pin config --------
const uint8_t TRIG_PIN  = 5;   // any output-capable GPIO
const uint8_t ECHO_PIN  = 18;  // any input-capable GPIO (use a 5V->3.3V divider!)
const uint8_t SERVO_PIN = 12;  // PWM-capable GPIO

// -------- Sweep and timing --------
const int   MIN_ANGLE    = 15;
const int   MAX_ANGLE    = 165;
const int   STEP_ANGLE   = 1;     // smaller = smoother, slower
const int   SERVO_DWELL  = 15;    // ms to let the horn settle
const long  ECHO_TIMEOUT = 30000; // us; ~5 m max (sound round trip)

// -------- Sampling --------
const int   SAMPLES_PER_READING = 3;   // take a few pings and median them

Servo servo;

// Trigger a single ultrasonic ping; returns distance in cm, or -1 on timeout
int pingOnceCm() {
  // 10 µs trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Use pulseInLong with a timeout so we don’t block forever
  unsigned long duration = pulseInLong(ECHO_PIN, HIGH, ECHO_TIMEOUT);
  if (duration == 0) return -1;  // timeout/no echo

  // Speed of sound ~ 343 m/s -> 0.0343 cm/µs; divide by 2 (round trip)
  float cm = (duration * 0.0343f) * 0.5f;
  if (cm < 2 || cm > 400) return -1; // basic sanity clamp for HC-SR04
  return (int)cm;
}

// Take multiple pings and return the median for stability
int measureDistanceCm() {
  int vals[SAMPLES_PER_READING];
  int count = 0;

  for (int i = 0; i < SAMPLES_PER_READING; i++) {
    int v = pingOnceCm();
    if (v >= 0) {
      vals[count++] = v;
    }
    delay(5); // small gap between pings
  }

  if (count == 0) return -1;

  // simple insertion sort for tiny arrays
  for (int i = 1; i < count; i++) {
    int key = vals[i], j = i - 1;
    while (j >= 0 && vals[j] > key) { vals[j+1] = vals[j]; j--; }
    vals[j+1] = key;
  }
  // median (or middle of what we got)
  return vals[count / 2];
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  Serial.begin(115200);

  // Initialize servo (50 Hz) and attach to PWM pin
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
  // Typical servo pulse width range: 500–2400 µs
  servo.attach(SERVO_PIN, 500, 2400);

  // Move to a known start position
  servo.write(MIN_ANGLE);
  delay(300);
}

void printReading(int angle, int distanceCm) {
  // CSV per line so it’s easy to plot/parse
  Serial.print(angle);
  Serial.print(",");
  Serial.println(distanceCm >= 0 ? distanceCm : -1); // -1 means no echo
}

void sweep(int startAngle, int endAngle, int step) {
  if (step == 0) return;

  if ((step > 0 && startAngle > endAngle) ||
      (step < 0 && startAngle < endAngle)) {
    step = -step; // auto-correct direction
  }

  for (int a = startAngle; (step > 0) ? (a <= endAngle) : (a >= endAngle); a += step) {
    servo.write(a);
    delay(SERVO_DWELL); // let the horn settle before pinging
    int d = measureDistanceCm();
    printReading(a, d);
    yield(); // be nice to the RTOS
  }
}

void loop() {
  sweep(MIN_ANGLE, MAX_ANGLE, STEP_ANGLE);   // left -> right
  sweep(MAX_ANGLE, MIN_ANGLE, STEP_ANGLE);   // right -> left
}
