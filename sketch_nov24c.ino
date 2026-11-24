
#define buzzerPin 23

void setup() {
  ledcAttach(buzzerPin, 720, 8);
}

void loop() {

  // Railway bell ringing
  ledcWriteTone(buzzerPin, 720);
  delay(350);

  // Short gap
  ledcWriteTone(buzzerPin, 0);
  delay(100);

}
