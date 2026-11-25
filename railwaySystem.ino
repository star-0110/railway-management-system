#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo myservo;
int servo1 = 12;

int trig = 5;
int echo = 18;
int d;
long duration;

int IR1 = 4;
int IR2 = 23;
int ir1, ir2;
int buz = 33;
int ledR = 32, ledG = 14;

int c = 0;
int t = 0;

bool buzActive = false;
bool buzToneOn = false;
unsigned long lastBuzMillis = 0;
unsigned long lastLcdMillis = 0;
unsigned long lastUSMillis = 0;

const unsigned long buzOnTime = 350;   // 350 ms ON
const unsigned long buzOffTime = 100;  // 100 ms OFF
const unsigned long lcdInterval = 150; // Update distance every 150 ms
const unsigned long USInterval = 1000; // Update distance every 150 ms

const int freq = 720;
const int resolution = 8;

void buzOn() {
  ledcWriteTone(buz, freq);  // 720 Hz ON
  buzToneOn = true;
}

void buzOff() {
  ledcWriteTone(buz, 0);     // OFF
  buzToneOn = false;
}

void printStatus(const char* msg, int col) {
  lcd.setCursor(0, 0);
  lcd.print("                "); // Clear line 0 smoothly without lcd.clear()
  lcd.setCursor(col, 0);
  lcd.print(msg);
}

void setup() {
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  myservo.setPeriodHertz(50);
  myservo.attach(servo1, 1000, 2000);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);

  ledcAttach(buz, freq, resolution);
  buzOff();

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  printStatus("Welcome", 1);
}

void loop() {
  unsigned long now = millis();
  ir1 = digitalRead(IR1);
  ir2 = digitalRead(IR2);

  // Update distance every 150 ms without delay() or lcd.clear()
  if (now - lastLcdMillis >= lcdInterval) {
    lastLcdMillis = now;

    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);

    duration = pulseIn(echo, HIGH, 25000);
    d = duration * 0.034 / 2;

    lcd.setCursor(2, 1);
    lcd.print("Distance=");
    lcd.print(d);
    lcd.print("   "); // Overwrites old digits like 120 -> 15 cleanly
  }
  if (d < 25) {
    t = 1;

  }


  // 1st IR triggers once (c == 0) -> close gate, show message, start non-blocking buzzer
  if (ir1 == 0 && ir2 == 1 && c == 0 && t == 1) {
   t=0;
    myservo.write(0);
    printStatus("Train arrived", 1);

    digitalWrite(ledR, HIGH);
    digitalWrite(ledG, LOW);

    buzActive = true;
    lastBuzMillis = now;
    buzOn();

    c = 1;
  }

  // 2nd IR triggers -> stop buzzer immediately, open gate, show exit message
  else if (ir1 == 1 && ir2 == 0 && c == 1) {
    buzActive = false;
    buzOff();

    myservo.write(180);
    printStatus("byeeeeee...!", 3);

    digitalWrite(ledG, HIGH);
    digitalWrite(ledR, LOW);

    c = 0;
    t = 0;
  }

  // Toggle 720 Hz ON (350 ms) and OFF (100 ms) without delay()
  if (buzActive) {
    unsigned long interval = buzToneOn ? buzOnTime : buzOffTime;
    if (now - lastBuzMillis >= interval) {
      lastBuzMillis = now;
      if (buzToneOn) {
        buzOff();
      } else {
        buzOn();
      }
    }
  }
}
