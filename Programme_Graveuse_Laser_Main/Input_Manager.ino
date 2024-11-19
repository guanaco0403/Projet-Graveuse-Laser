const int deadzone = 50;
int joystickXVal, joystickYVal;
const int MaxSpeed = 4000;

void ManualMotorMove() {
  joystickXVal = analogRead(JoyX) - 512;
  joystickYVal = analogRead(JoyY) - 512;

  // Adjust X motor speed based on joystick input
  int speedX;
  if (abs(joystickXVal) > deadzone) {
    // Map the joystick value to a range for speed
    speedX = map(joystickXVal, -512, 512, -MaxSpeed, MaxSpeed);
    stepper_X.setSpeed(speedX);
  } else {
    // Stop the motor if within the deadzone
    stepper_X.setSpeed(0);
  }

  // Adjust Y motor speed based on joystick input
  int speedY;
  if (abs(joystickYVal) > deadzone) {
    // Map the joystick value to a range for speed
    speedY = map(joystickYVal, -512, 512, -MaxSpeed, MaxSpeed);
    stepper_Y.setSpeed(speedY);
  } else {
    // Stop the motor if within the deadzone
    stepper_Y.setSpeed(0);
  }

  // Run the motors at the set speeds
  if (speedX > 0) {
    if (stepper_X.currentPosition() < MAX_X && stepper_X.currentPosition() >= 0) {
      stepper_X.runSpeed();
    }
  } else {
    if (stepper_X.currentPosition() <= MAX_X && stepper_X.currentPosition() > 0) {
      stepper_X.runSpeed();
    }
  }

  if (speedY > 0) {
    if (stepper_Y.currentPosition() < MAX_Y && stepper_Y.currentPosition() >= 0) {
      stepper_Y.runSpeed();
    }
  } else {
    if (stepper_Y.currentPosition() <= MAX_Y && stepper_Y.currentPosition() > 0) {
      stepper_Y.runSpeed();
    }
  }
}
