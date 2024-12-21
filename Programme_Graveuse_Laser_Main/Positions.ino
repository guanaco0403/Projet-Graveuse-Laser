/*
 * Gestion de la gravure, position des points pour les lettres et toute logique autour de la gravure
 */

// Classe pour 1 instruction pour les moteurs et le laser (position X, posistion Y et si on active le laser ou non)
class Instruction {
  public:
    int x;
    int y;
    bool engrave;
};

// Tableau d'instruction pour chaque point de chaque lettre
Instruction Lettres_IESN[] = {
  // I
  {0, 500, false},
  {1200, 500, true},
  {1200, 700, true},
  {700, 700, true},
  {700, 1900, true},
  {1200, 1900, true},
  {1200, 2100, true},
  {0, 2100, true},
  {0, 1900, true},
  {500, 1900, true},
  {500, 700, true},
  {0, 700, true},
  {0, 500, true},

  // E
  {1600, 500, false},
  {2800, 500, true},
  {2800, 700, true},
  {1800, 700, true},
  {1800, 1200, true},
  {2400, 1200, true},
  {2400, 1400, true},
  {1800, 1400, true},
  {1800, 1900, true},
  {2800, 1900, true},
  {2800, 2100, true},
  {1600, 2100, true},
  {1600, 500, true},

  // S
  {3500, 500, false},
  {4400, 500, true},
  {4400, 700, true},
  {3600, 700, true},
  {3400, 900, true},
  {3400, 1100, true},
  {3600, 1200, true},
  {4200, 1200, true},
  {4400, 1400, true},
  {4400, 1900, true},
  {4100, 2100, true},
  {3200, 2100, true},
  {3200, 1900, true},
  {4000, 1900, true},
  {4200, 1800, true},
  {4200, 1600, true},
  {4000, 1400, true},
  {3400, 1400, true},
  {3200, 1200, true},
  {3200, 700, true},
  {3500, 500, true},

  // N
  {4800, 500, false},
  {5100, 500, true},
  {5800, 1900, true},
  {5800, 500, true},
  {6000, 500, true},
  {6000, 2100, true},
  {5700, 2100, true},
  {5000, 700, true},
  {5000, 2100, true},
  {4800, 2100, true},
  {4800, 500, true},
};

Instruction Ventillo[] = { // 500 = 3cm -> 1cm = 166 -> 4cm = 664 -> 2,5 = 415
  {1000*2.2, 1000*2.2, false},
  {336*2.2, 1000*2.2, true},
  {-79*2.2, 585*2.2, true},
  {-79*2.2, -79*2.2, true},
  {336*2.2, -494*2.2, true},
  {1000*2.2, -494*2.2, true},
  {1415*2.2, -79*2.2, true},
  {1415*2.2, 585*2.2, true},
  {1000*2.2, 1000*2.2, true},
};

/* Fonction de gestion de la gravure principale */
void Grave() {
  int Speed = mainSpeed * 40; // Enregistrement de la vitesse de gravure demandée
  int numInstructions = sizeof(Lettres_IESN) / sizeof(Lettres_IESN[0]); // Calcul du nombre d'instructions total
  int i = 0; // Variable locale qui enregistre l'index de l'isntruction actuelle

  int offsetX = 2800; // Offset de gravure en X
  int offsetY = 7500;  // Offset de gravure en Y

  JobStartSound(); // On joue le son de démarage de gravure
  AutoHome(); // Auto Homing pour la sécurité

  // Impression a l'écran
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Engraving [0%]");
  PrintProgressBar(0, 1);

  int computedTime = ComputeEngravingTime(Speed, numInstructions, 0); // Calcul de l'ETA (temps de gravure estimé)
  lcd.setCursor(10, 2);
  lcd.print("ETA: ");
  lcd.print(SecsToTimeString(computedTime)); // Impression de l'ETA

  long startMillis = millis(); // Enregistrement du temps au démarrage de la gravure pour les statistiques
  long oldUpdate = 0; // Variable locale pour la mise a jour de l'ETA grâce a millis()

  // On parcour toutes les instructions
  while (i < numInstructions) {
    if (SystemState == false) { // On vérifie si on a un arrêt d'urgence
      return;
    }

    // On regarde si l'instruction actuelle nous demande de graver ou non
    if (Lettres_IESN[i].engrave == true) {
      SetLaser(mainLaserPower); // On set la puissance du laser a la puissance demandée
      GoTo_AdaptedSpeed(Lettres_IESN[i].x + offsetX , Lettres_IESN[i].y + offsetY, Speed); // On va au point X,Y sans acceleration
      SetLaser(0); // On Désactive le laser
    }
    else {
      GoTo(Lettres_IESN[i].x + offsetX, Lettres_IESN[i].y + offsetY); // On va au point X,Y demandée avec acceleration
    }
    i++; // On passe a l'instruction suivante
    
    GravingPercent(i, numInstructions); // On Affiche le pourcentage de progression actuel a l'écran
    UpdateLCDPositions(); // On met a jour la position du laser a l'écran

    // Toutes les 5 secondes on met a jour le temps estimé de gravure ETA restant a l'écran
    if (millis() - oldUpdate >= 5000) {
      computedTime = ComputeEngravingTime(Speed, numInstructions, i); // Calcul du temps restant estimé
      lcd.setCursor(15, 2);
      lcd.print(SecsToTimeString(computedTime));
      oldUpdate = millis();
    }
  }
  // Fin des instructions de gravure
  /*if (SystemState == false) { // On Vérifie si on a un arrêt d'urgence
    return;
  }*/

  int engravingTime = (millis() - startMillis) / 1000; // Calcul du temps total de gravure pour les statistiques
  totalEngravingTime = totalEngravingTime + engravingTime;
  totalEngravingCount += 1; // On augmente le nombre de gravures total dans les statistiques
  saveEEPROM_Data(); // On enregistre les statistiques dans L'EEPROM
  GoTo_AdaptedSpeed(0, 0, 2000); // On reviens au point 0,0
  DisplayCurrentScreen(); // On Affiche l'écran actuel
  JobDoneSound(); // On joue le son de fin de gravure
}

/* Fonction pour calculer le temps de gravure estimé ETA */
int ComputeEngravingTime(float Speed, int numInstructions, int currentInstruction) {
  float k = 6.3; // Facteur de relation vitesse/temps
  double computeTime = 0;
  for (int i = currentInstruction; i < numInstructions; i++) {
    int deltaX = abs(Lettres_IESN[i].x - Lettres_IESN[i + 1].x);
    int deltaY = abs(Lettres_IESN[i].y - Lettres_IESN[i + 1].y);
    if (deltaX > deltaY) {
      computeTime += abs((k * deltaX) / (Speed / 10));
    }
    else {
      computeTime += abs((k * deltaY) / (Speed / 10));
    }
  }

  return computeTime / 60;
}
