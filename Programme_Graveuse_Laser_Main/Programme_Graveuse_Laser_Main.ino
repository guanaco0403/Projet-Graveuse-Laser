/*
   Projet : Graveuse Laser
   Bachelier en Automatisation - Hénallux (2024-2025)

   Membres de l'équipe :
   - Thomas Karolczak
   - Pieriek Poncelet
   - Benjamin Bol
   - Erkan Selim

   Description :
   Ce code gère les fonctionnalités de la graveuse laser, notamment
   le contrôle des moteurs, le positionnement précis et l'activation
   du laser pour la gravure.

   Licence :
   Ce projet est réalisé dans un cadre éducatif et est sous licence open source.
   Vous êtes libre de le modifier et de le distribuer à condition de conserver cette signature.

   Dernière mise à jour : 19-12-24 (Thomas Karolczak)
*/

#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define laser 9        // Signal PWM Laser
#define stepX_dir 6    // Signal STEP driver moteur X
#define stepX_step 5   // Signal DIRECTION driver moteur X
#define stepY_dir 8    // Signal STEP driver moteur Y
#define stepY_step 7   // Signal DIRECTION driver moteur Y
#define en_X 22        // Signal ENABLE driver moteur X
#define en_Y 23        // Signal ENABLE driver moteur Y
#define endX 26        // Fin de course axe X
#define endY1 27       // Fin de course axe Y (1)
#define endY2 3        // Fin de course axe Y (2)
#define JoyX A9        // Entrée analogique axe X joystick
#define JoyY A8        // Entrée analogique axe Y joystick
#define Joy_Switch 30  // Bouton du joystick
#define ARU 2          // Entrée Bouton arret d'urgence
#define buzzer 4       // Buzzer
#define led_ARU 31     // Led de défaut (ARU)

#define MAX_X 13500    // Nombre de pas MAX pour l'axe X
#define MAX_Y 17000    // Nombre de pas MAX pour l'axe Y

AccelStepper stepper_X(AccelStepper::DRIVER, stepX_step, stepX_dir); // Définition du DRIVER du moteur pas a pas X
AccelStepper stepper_Y(AccelStepper::DRIVER, stepY_step, stepY_dir); // Définition du DRIVER du moteur pas a pas Y

LiquidCrystal_I2C lcd(0x27, 20, 4); // Définition du LCD I²C en 20x4 Charactères

// Donnees Statistiques
int totalEngravingCount = 0;
int totalEngravingTime = 0;
// ====================

// Donnees Parametres
int mainSpeed = 0;
int mainLaserPower = 0;
// ====================

byte currentScreen = 0; // Définnit la page actuelle du menu
byte oldScreen = 255; // Définit la page précédente du menu afin de faire un front montant pour la mise a jour de la page

bool SystemState = true; // Bool qui dit au système si un ARU est en cours

enum JoyInput { // enum qui reprends tout les états du joystick
  NONE,
  UP,
  DOWN,
  RIGHT,
  LEFT,
  PRESS
};

void setup() {
  Serial.begin(9600); // Activation du Serial en 9600 Bauds

  // Paramètrage des pins de l'Arduino pour les entrées et sorties
  pinMode(stepX_dir, OUTPUT);
  pinMode(stepX_step, OUTPUT);
  pinMode(stepY_dir, OUTPUT);
  pinMode(stepY_step, OUTPUT);
  pinMode(en_X, OUTPUT);
  pinMode(en_Y, OUTPUT);
  pinMode(endX, INPUT_PULLUP);
  pinMode(endY1, INPUT_PULLUP);
  pinMode(endY2, INPUT_PULLUP);
  pinMode(laser, OUTPUT);
  pinMode(Joy_Switch, INPUT_PULLUP);
  pinMode(ARU, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(led_ARU, OUTPUT);
  // ========================

  attachInterrupt(digitalPinToInterrupt(ARU), ARU_INTERRUPT , FALLING); // On définit que la Pin de l'Arrêt d'urgence est une Interrupt

  stepper_X.setMaxSpeed(4000);      // Vitesse Maximum axe X (pas par secondes)
  stepper_X.setAcceleration(8000);  // Acceleration axe X (pas par secondes^2)
  stepper_Y.setMaxSpeed(4000);      // Vitesse Maximum axe Y (pas par secondes)
  stepper_Y.setAcceleration(8000);  // Acceleration axe Y (pas par secondes^2)

  lcd.init(); // Initialisation du LCD avec redondance pour être sur qu'il est bien initialisé
  lcd.init();
  lcd.backlight(); // Allumage du Backlight du LCD

  CheckResetFactory(); // On vérifie si on doit remètre la machine a l'état d'usine

  // Impression de l'écran de démarrage de la machine
  lcd.setCursor(3, 0);
  lcd.print("Laser Engraver");
  lcd.setCursor(4, 2);
  lcd.print("En hommage a");
  lcd.setCursor(3, 3);
  lcd.print("Jean-Pol Peten");

  BootSound(); // On joue le son de démarrage
  GetEEPROM_Data(); // On récupère les données de L'EEPROM

  // Vérification de l'état de L'arrêt d'urgence
  if (digitalRead(ARU) == LOW) {
    ARU_INTERRUPT();
  } else {
    EnableStepperMotorPower();
    AutoHome();
  }
}

/* Fonction principale d'éxécution */
void loop() {
  // Front montant pour la mise a jour de l'écran sur le LCD si l'écran précedent n'est pas le même que l'actuel
  if (currentScreen != oldScreen) {
    DisplayCurrentScreen(); // Mise a jour de l'écran
    oldScreen = currentScreen;
  }

  // Attente aquitement si on est sur le screen ARU
  if (currentScreen == 100) {
    CheckARU_ACK(); // Si on est en mode Arrêt d'urgence on attend l'aquittement
  }

  // Partie Gestion des menus de l'écran grace a l'appui du joystick ou les movements du joystick
  if (currentScreen <= 3) {
    JoyInput input = CheckInputs(); // on get l'état actuel du joystick
    if (input != NONE) {
      switch (input) {
        case PRESS:
          if (currentScreen == 0) { // Si on est dans le menu principal et qu'on appui sur le bouton du joystick
            // On re-définis les paramètres d'accélération et de vitesses MAX
            stepper_X.setMaxSpeed(4000);
            stepper_X.setAcceleration(8000);
            stepper_Y.setMaxSpeed(4000);
            stepper_Y.setAcceleration(8000);
            
            Grave(); // On appelle la fonction pour démarrer la gravure
          } else if (currentScreen == 2) { // Si on est dans le menu pour le mode manuel et qu'on appui sur le bouton du joystick
            Beep();
            currentScreen = 40; // On passe sur le menu mode manuel
            DisplayCurrentScreen(); // On affiche l'écran actuel
            delay(400); // Pour laisser le temps de relacher le bouton
            ManualMotorMove(); // On appelle la fonction qui gère la logique du mode manuel
          } else if (currentScreen == 3) { // Si on est dans le menu pour les paramètres et qu'on appui sur le bouton du joystick
            Beep();
            currentScreen = 10; // On passe sur le menu paramètres
            DisplayCurrentScreen(); // On affiche l'écran actuel
            SettingsManager(); // On appelle la fonction qui gère la logique des paramètres
          }
          break;
        case RIGHT:
          if (currentScreen < 3) {
            currentScreen++;
          }
          else {
            currentScreen = 0;
          }
          Beep2();
          break;

        case LEFT:
          if (currentScreen > 0) {
            currentScreen--;
          }
          else {
            currentScreen = 3;
          }
          Beep2();
          break;
      }
    }
  }
}

/* Fonction pour déplacer les moteurs en 1 point précis avec acceleration */
void GoTo(int X, int Y) {
  stepper_X.moveTo(X);
  stepper_Y.moveTo(Y);

  // Marche des moteurs jusqu'a la position demandée
  while (stepper_X.distanceToGo() != 0 || stepper_Y.distanceToGo() != 0) {
    stepper_X.run();
    stepper_Y.run();
  }
}

/* Fonction pour déplacer les moteurs en 1 point précis (sans acceleration) en adaptant la vitesse des 2 moteurs pour qu'ils arrivent au point en même temps */
void GoTo_AdaptedSpeed(int targetX, int targetY, int baseSpeed) {
  int currentX = stepper_X.currentPosition(); // Enregistement de la position actuelle X
  int currentY = stepper_Y.currentPosition(); // Enregistement de la position actuelle Y

  // Calcul des distances X et Y
  int deltaX = targetX - currentX;
  int deltaY = targetY - currentY;

  // Transformation des distances en distances absolues
  int absDeltaX = abs(deltaX);
  int absDeltaY = abs(deltaY);

  float speedX, speedY; // Variables de vitesses moteur X et Y

  // Ajustement des vitesses en fonction du rapport des distances
  if (absDeltaX > absDeltaY) {
    speedX = baseSpeed;
    speedY = baseSpeed * ((float)absDeltaY / absDeltaX);  // Scale Y speed to keep the line straight
  } else {
    speedY = baseSpeed;
    speedX = baseSpeed * ((float)absDeltaX / absDeltaY);  // Scale X speed to keep the line straight
  }

  // Déplacement vers la position cible à vitesse constante
  stepper_X.moveTo(targetX);
  stepper_Y.moveTo(targetY);

  // On donnent les vitesses adaptées des moteurs
  stepper_X.setSpeed(speedX);
  stepper_Y.setSpeed(speedY);

  // Fonctionnement les deux moteurs pas à pas jusqu'à ce qu'ils atteignent la cible
  while (stepper_X.distanceToGo() != 0 || stepper_Y.distanceToGo() != 0) {
    if (SystemState == false) { // Vérification de L'Arrêt d'urgence
      return;
    }

    // Marche des moteurs jusqu'a la position demandée a une vitesse constante
    stepper_X.runSpeedToPosition();
    stepper_Y.runSpeedToPosition();
  }
  stepper_X.stop();
  stepper_Y.stop();
}

/* Fonction qui gère le Laser avec une entrée de Puissance en pourcent */
void SetLaser(byte power) {
  // Si la puissance = 0% alors on coupe le laser
  if (power == 0) {
    analogWrite(laser, 0);
  }
  // Si la puissance = 1% alors on met le laser a 1 sur 255, la ou l'on peut même regarder le laser sans lunette car il est innofensif
  else if (power == 1 && SystemState) {
    analogWrite(laser, 1);
  }
  // Sinon on fait varier la puissance du laser de 2% et 100% en signal PWM (5 - 255)
  else if (SystemState) {
    int pwmValue = map(power, 2, 100, 5, 255);
    analogWrite(laser, pwmValue);
  }
}
