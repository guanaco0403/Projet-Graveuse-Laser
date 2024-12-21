/*
 * Gestion des sons de la machine
 */

/* Son de démarrage mario */
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

/* Son de fin de gravure */
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

/* Son de début de gravure */
void JobStartSound() {
  tone(buzzer, 1200);
  delay(400);
  tone(buzzer, 1800);
  delay(800);
  noTone(buzzer);
}

/* Son de sélection de menus */
void Beep() {
  tone(buzzer, 2000, 100);
}

/* Son de navigation de menus */
void Beep2() {
  tone(buzzer, 100, 25);
}
