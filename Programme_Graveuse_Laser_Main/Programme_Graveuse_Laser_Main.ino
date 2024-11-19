#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>

#define laser 2
#define stepX_dir 22
#define stepX_step 3
#define stepY_dir 23
#define stepY_step 4
#define endX 24
#define endY 25
#define JoyX A2
#define JoyY A3
#define Joy_Switch 26

#define MAX_X 6400 // 400 step = 1cm
#define MAX_Y 6000

AccelStepper stepper_X(AccelStepper::DRIVER, stepX_step, stepX_dir);
AccelStepper stepper_Y(AccelStepper::DRIVER, stepY_step, stepY_dir);

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);

  pinMode(stepX_dir, OUTPUT);
  pinMode(stepX_step, OUTPUT);
  pinMode(stepY_dir, OUTPUT);
  pinMode(stepY_step, OUTPUT);
  pinMode(endX, INPUT_PULLUP);
  pinMode(endY, INPUT_PULLUP);
  pinMode(laser, OUTPUT);
  pinMode(Joy_Switch, INPUT_PULLUP);

  stepper_X.setMaxSpeed(4000);      // Max speed (steps per second)
  stepper_X.setAcceleration(8000);  // Acceleration (steps per second^2)
  stepper_Y.setMaxSpeed(4000);
  stepper_Y.setAcceleration(8000);

  Serial.println("Laser Engraver Starting...");
  Serial.println("Powered by zebi OS 1.0");

  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Laser Engraver");
  lcd.setCursor(4, 2);
  lcd.print("Powered by");
  lcd.setCursor(4, 3);
  lcd.print("zebi OS 1.0");

  delay(2000);

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Laser Engraver");
  // Homing process
  //Serial.println("Homing X Axis");
  lcd.setCursor(0, 2);
  lcd.print("   Homing X Axis    ");
  Home(stepper_X, endX);
  //Serial.println("Homing Y Axis");
  lcd.setCursor(0, 2);
  lcd.print("   Homing Y Axis    ");
  Home(stepper_Y, endY);

  //GoTo(1000, 1000);
  //analogWrite(3, 1);
  //GoTo(6400, 0);
  //while(true){;}
  lcd.setCursor(0, 2);
  lcd.print("    Laser Ready    ");
}

void loop() {
  ManualMotorMove();
  if (digitalRead(Joy_Switch) == LOW) {
    stepper_X.setMaxSpeed(4000);
    stepper_X.setAcceleration(8000);
    stepper_Y.setMaxSpeed(4000);
    stepper_Y.setAcceleration(8000);
    Grave();
  }
  //Serial.println("Starting Engraving Process...");
  //Grave();
  //Serial.println("Engraving Finnished");
  //delay(5000);
}

void Home(AccelStepper stepper, char sensor) {
  stepper.setSpeed(-3000);  // Set speed to move toward the endstop (negative direction)
  while (digitalRead(sensor) == HIGH) {
    stepper.runSpeed();  // Keep moving until the endstop is triggered
  }

  stepper.stop();  // Stop the motor once the endstop is hit
  stepper.setCurrentPosition(0);  // Set current position as 0 (home position)

  // Optionally move away from the endstop and re-home for better accuracy
  stepper.setSpeed(1000);  // Move away slowly
  stepper.move(400);  // Move a small amount away from the endstop
  while (stepper.distanceToGo() != 0) {
    stepper.run();  // Run until we've moved a small distance away
  }

  // Re-home at slower speed
  stepper.setSpeed(-500);  // Slow speed toward endstop
  while (digitalRead(sensor) == HIGH) {
    stepper.runSpeed();  // Re-home to improve accuracy
  }

  stepper.stop();  // Stop motor at home position
  stepper.setCurrentPosition(0);  // Reset position to 0 again (accurate home position)
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
    Serial.println("Laser Power OFF");
  }
  else if (power == 1) {
    analogWrite(laser, 1);
    Serial.println("Laser Power PWM: 1");
  }
  else {
    int pwmValue = map(power, 2, 100, 5, 255);
    analogWrite(laser, pwmValue);
    Serial.print("Laser Power PWM: ");
    Serial.println(pwmValue);
  }
}
