#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define laser 9
#define stepX_dir 6
#define stepX_step 5
#define stepY_dir 8
#define stepY_step 7
#define en_X 22
#define en_Y 23
#define endX 26
#define endY1 27
#define endY2 3
#define JoyX A9
#define JoyY A8
#define Joy_Switch 30
#define ARU 2
#define buzzer 4
#define led_ARU 31

#define MAX_X 13500 // 400 step = 1cm
#define MAX_Y 17000

AccelStepper stepper_X(AccelStepper::DRIVER, stepX_step, stepX_dir);
AccelStepper stepper_Y(AccelStepper::DRIVER, stepY_step, stepY_dir);

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Donnees Statistiques
int totalEngravingCount = 0;
int totalEngravingTime = 0;

byte currentScreen = 0;
byte oldScreen = 255;

int mainSpeed = 0;
int mainLaserPower = 0;

bool SystemState = true;

enum JoyInput {
  NONE,
  UP,
  DOWN,
  RIGHT,
  LEFT,
  PRESS
};

void setup() {
  Serial.begin(9600);

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

  attachInterrupt(digitalPinToInterrupt(ARU), ARU_INTERRUPT , FALLING);

  stepper_X.setMaxSpeed(4000);      // Max speed (steps per second)
  stepper_X.setAcceleration(8000);  // Acceleration (steps per second^2)
  stepper_Y.setMaxSpeed(4000);
  stepper_Y.setAcceleration(8000);

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Laser Engraver");
  
  BootSound();
  GetEEPROM_Data();

  if (digitalRead(ARU) == LOW) {
    ARU_INTERRUPT();
  } else {
    EnableStepperMotorPower();
    AutoHome();
  }
}

void loop() {
  if (currentScreen != oldScreen) {
    DisplayCurrentScreen();
    oldScreen = currentScreen;
  }

  if (currentScreen == 40) {
    ManualMotorMove();
  }
  else if (currentScreen == 10) {
    //SettingsPage();
    /*JoyInput input = CheckInputs();
    if (input == PRESS) {
      currentScreen = 0;
    }*/
  }
  else if (currentScreen == 100) {
    CheckARU_ACK();
  }

  if (currentScreen <= 3) {
    JoyInput input = CheckInputs();
    if (input != NONE) {
      switch (input) {
        case PRESS:
          if (currentScreen == 0) {
            stepper_X.setMaxSpeed(4000);
            stepper_X.setAcceleration(8000);
            stepper_Y.setMaxSpeed(4000);
            stepper_Y.setAcceleration(8000);
            Grave();
          } else if (currentScreen == 2) {
            currentScreen = 40;
            delay(300);
            SetLaser(1);
          } else if (currentScreen == 3) {
            currentScreen = 10;
            delay(300);
          }
          break;
        case RIGHT:
          if (currentScreen < 3) {
            currentScreen++;
          }
          else {
            currentScreen = 0;
          }
          break;

        case LEFT:
          if (currentScreen > 0) {
            currentScreen--;
          }
          else {
            currentScreen = 3;
          }
          break;
      }
    }
  }
}

void GoTo(int X, int Y) {
  stepper_X.moveTo(X);
  stepper_Y.moveTo(Y);
  while (stepper_X.distanceToGo() != 0 || stepper_Y.distanceToGo() != 0) {
    stepper_X.run();
    stepper_Y.run();
  }
}

void GoTo_No_Accel(int X, int Y, int Speed) {
  stepper_X.moveTo(X);
  stepper_Y.moveTo(Y);
  while (stepper_X.distanceToGo() != 0 || stepper_Y.distanceToGo() != 0) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }

    stepper_X.setSpeed(Speed);
    stepper_Y.setSpeed(Speed);
    stepper_X.runSpeedToPosition();
    stepper_Y.runSpeedToPosition();
  }
  stepper_X.stop();
  stepper_Y.stop();
}

void GoTo_AdaptedSpeed(int targetX, int targetY, int baseSpeed) {
  int currentX = stepper_X.currentPosition();
  int currentY = stepper_Y.currentPosition();

  // Calculate distances in X and Y
  int deltaX = targetX - currentX;
  int deltaY = targetY - currentY;

  // Calculate absolute distances
  int absDeltaX = abs(deltaX);
  int absDeltaY = abs(deltaY);

  // Set maximum speed for the longest distance (either X or Y)
  float speedX, speedY;

  // Adjust the speeds based on the ratio of the distances
  if (absDeltaX > absDeltaY) {
    speedX = baseSpeed;
    speedY = baseSpeed * ((float)absDeltaY / absDeltaX);  // Scale Y speed to keep the line straight
  } else {
    speedY = baseSpeed;
    speedX = baseSpeed * ((float)absDeltaX / absDeltaY);  // Scale X speed to keep the line straight
  }

  // Set directions based on the sign of delta
  //stepper_X.setSpeed(deltaX > 0 ? c : -speedX);
  //stepper_Y.setSpeed(deltaY > 0 ? speedY : -speedY);

  // Move to target position with constant speed
  stepper_X.moveTo(targetX);
  stepper_Y.moveTo(targetY);

  // Run both steppers until they reach the target
  while (stepper_X.distanceToGo() != 0 || stepper_Y.distanceToGo() != 0) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }

    stepper_X.setSpeed(speedX);
    stepper_Y.setSpeed(speedY);
    stepper_X.runSpeedToPosition();
    stepper_Y.runSpeedToPosition();
  }
  stepper_X.stop();
  stepper_Y.stop();
}

void SetLaser(byte power) {
  if (power == 0) {
    analogWrite(laser, 0);
  }
  else if (power == 1 && SystemState) {
    analogWrite(laser, 1);
  }
  else if (SystemState) {
    int pwmValue = map(power, 2, 100, 5, 255);
    analogWrite(laser, pwmValue);
  }
}
