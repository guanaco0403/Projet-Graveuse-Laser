void BootSound() {
  tone(buzzer, 660, 100);
  delay(150);
  tone(buzzer, 660, 100);
  delay(300);
  tone(buzzer, 660, 100);
  delay(300);
  tone(buzzer, 510, 100);
  delay(100);
  tone(buzzer, 660, 100);
  delay(300);
  tone(buzzer, 770, 100);
  delay(550);
  tone(buzzer, 380, 100);
  delay(200);
}

void JobDoneSound() {
  tone(buzzer, 1000);
  delay(400);
  tone(buzzer, 1200);
  delay(400);
  tone(buzzer, 1800);
  delay(1000);
  noTone(buzzer);
  delay(500);
  tone(buzzer, 1000);
  delay(400);
  tone(buzzer, 1200);
  delay(400);
  tone(buzzer, 1800);
  delay(1000);
  noTone(buzzer);
}
