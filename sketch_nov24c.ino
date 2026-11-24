#include <LiquidCrystal_I2C.h>

#include <ESP32Servo.h>

#include<Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);

Servo myservo;
int servo1 = 12;

int IR1 = 4;
int IR2 = 23;
int ir1,ir2;

int buz = 33;

int ledR = 32 , ledG = 14;

int trig = 5 , echo = 18;

int dis;
long dur;



void setup() {
  // put your setup code here, to run once:
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);           
	myservo.attach(servo1, 1000, 2000);

  pinMode(IR1,INPUT);
  pinMode(IR2,INPUT);

  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  pinMode(buz,OUTPUT);

  pinMode(ledR,OUTPUT);
  pinMode(ledG,OUTPUT);

  lcd.init();
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);
}

void loop() {
  ir1 = digitalRead(IR1);
  ir2 = digitalRead(IR2);
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  dur = pulseIn(echo,HIGH);
  dis = dur * 0.034 / 2;
  if(ir1==0&&ir2==1){
    myservo.write(0);
    delay(100);
    lcd.setCursor(6,0);
    lcd.print("Train arrived");
    digitalWrite(ledR,HIGH);
    digitalWrite(ledG,LOW);
    digitalWrite(buz,HIGH);
    delay(100);
    digitalWrite(buz,LOW);
    delay(100);

  }
  if(ir1==1&&ir2==0){
    myservo.write(180);
    delay(100);
        lcd.setCursor(6,0);
    lcd.print("byeeeeee...!");
    digitalWrite(ledG,HIGH);
    digitalWrite(ledR,LOW);
    digitalWrite(buz,LOW);
    delay(100);
  }
}
