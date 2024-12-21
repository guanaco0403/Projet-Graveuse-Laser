/* Fonction pour activer les drivers */
void EnableStepperMotorPower() {
  digitalWrite(en_X, LOW);
  digitalWrite(en_Y, LOW);
}

/* Fonction pour Désactiver les drivers */
void DisableStepperMotorPower() {
  digitalWrite(en_X, HIGH);
  digitalWrite(en_Y, HIGH);
}

/* Fonction de Homing des axes de la machine */
void AutoHome() {
  lcd.clear(); // Effacer l'écran LCD
  lcd.setCursor(3, 0); // Positionner le curseur sur la ligne 1, colonne 3
  lcd.print("Laser Engraver"); // Afficher "Laser Engraver" sur l'écran
  lcd.setCursor(0, 2); // Positionner le curseur sur la ligne 3, colonne 0
  lcd.print("   Homing X Axis    "); // Afficher "Homing X Axis" pour indiquer le calibrage de l'axe X

  // Calibration de l'axe X
  stepper_X.setSpeed(-3000);  // Définir la vitesse pour se déplacer vers l'endstop (direction négative)
  while (digitalRead(endX) == HIGH) { // Boucle jusqu'à ce que l'endstop soit activé
    if (SystemState == false) { // Vérification d'un arrêt d'urgence (ARU)
      return; // Sortie immédiate de la fonction en cas d'ARU
    }
    stepper_X.runSpeed();  // Continuer à se déplacer vers l'endstop
  }

  stepper_X.stop();  // Arrêter le moteur lorsque l'endstop est atteint
  stepper_X.setCurrentPosition(0);  // Définir la position actuelle comme 0 (position de départ)

  // Optionnel : s'éloigner légèrement de l'endstop pour plus de précision
  stepper_X.setSpeed(1000);  // Définir une vitesse lente
  stepper_X.move(400);  // S'éloigner de 400 unités de l'endstop
  while (stepper_X.distanceToGo() != 0) { // Attendre que le mouvement soit terminé
    if (SystemState == false) { // Vérification d'un ARU
      return; // Sortie immédiate en cas d'ARU
    }
    stepper_X.run();  // Effectuer le mouvement
  }

  // Re-calibrage à vitesse lente
  stepper_X.setSpeed(-500);  // Définir une vitesse lente pour revenir vers l'endstop
  while (digitalRead(endX) == HIGH) { // Boucle jusqu'à atteindre l'endstop
    if (SystemState == false) { // Vérification d'un ARU
      return; // Sortie immédiate en cas d'ARU
    }
    stepper_X.runSpeed();  // Continuer à se déplacer vers l'endstop
  }

  stepper_X.stop();  // Arrêter le moteur
  stepper_X.setCurrentPosition(0);  // Redéfinir la position actuelle comme 0 (position exacte de départ)

  // FIN DU CALIBRAGE DE L'AXE X
  lcd.setCursor(0, 2); // Positionner le curseur sur la ligne 3, colonne 0
  lcd.print("   Homing Y Axis    "); // Afficher "Homing Y Axis" pour indiquer le calibrage de l'axe Y

  // Calibration de l'axe Y
  stepper_Y.setSpeed(-3000);  // Définir la vitesse pour se déplacer vers l'endstop (direction négative)
  while (digitalRead(endY1) == HIGH || digitalRead(endY2) == HIGH) { // Boucle jusqu'à ce qu'un des endstops soit activé
    if (SystemState == false) { // Vérification d'un ARU
      return; // Sortie immédiate en cas d'ARU
    }
    stepper_Y.runSpeed();  // Continuer à se déplacer vers l'endstop
  }

  stepper_Y.stop();  // Arrêter le moteur lorsque l'endstop est atteint
  stepper_Y.setCurrentPosition(0);  // Définir la position actuelle comme 0 (position de départ)

  // Optionnel : s'éloigner légèrement de l'endstop pour plus de précision
  stepper_Y.setSpeed(1000);  // Définir une vitesse lente
  stepper_Y.move(400);  // S'éloigner de 400 unités de l'endstop
  while (stepper_Y.distanceToGo() != 0) { // Attendre que le mouvement soit terminé
    if (SystemState == false) { // Vérification d'un ARU
      return; // Sortie immédiate en cas d'ARU
    }
    stepper_Y.run();  // Effectuer le mouvement
  }

  // Re-calibrage à vitesse lente
  stepper_Y.setSpeed(-500);  // Définir une vitesse lente pour revenir vers l'endstop
  while (digitalRead(endY1) == HIGH || digitalRead(endY2) == HIGH) { // Boucle jusqu'à atteindre l'endstop
    if (SystemState == false) { // Vérification d'un ARU
      return; // Sortie immédiate en cas d'ARU
    }
    stepper_Y.runSpeed();  // Continuer à se déplacer vers l'endstop
  }

  GoTo(0, -50); // Ajustement final pour aligner les axes Y correctement

  stepper_Y.stop();  // Arrêter le moteur
  stepper_Y.setCurrentPosition(0);  // Redéfinir la position actuelle comme 0 (position exacte de départ)
}
