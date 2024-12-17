void EnableStepperMotorPower() {
  digitalWrite(en_X, LOW);
  digitalWrite(en_Y, LOW);
}

void DisableStepperMotorPower() {
  digitalWrite(en_X, HIGH);
  digitalWrite(en_Y, HIGH);
}

// HOMING
void AutoHome() {
  /*stepper_X.setCurrentPosition(0);
  stepper_Y.setCurrentPosition(0);
  return;*/
  
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Laser Engraver");
  lcd.setCursor(0, 2);
  lcd.print("   Homing X Axis    ");
  //HomeAxis(stepper_X, endX);
  stepper_X.setSpeed(-3000);  // Set speed to move toward the endstop (negative direction)
  while (digitalRead(endX) == HIGH) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_X.runSpeed();  // Keep moving until the endstop is triggered
  }

  stepper_X.stop();  // Stop the motor once the endstop is hit
  stepper_X.setCurrentPosition(0);  // Set current position as 0 (home position)

  // Optionally move away from the endstop and re-home for better accuracy
  stepper_X.setSpeed(1000);  // Move away slowly
  stepper_X.move(400);  // Move a small amount away from the endstop
  while (stepper_X.distanceToGo() != 0) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_X.run();  // Run until we've moved a small distance away
  }

  // Re-home at slower speed
  stepper_X.setSpeed(-500);  // Slow speed toward endstop
  while (digitalRead(endX) == HIGH) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_X.runSpeed();  // Re-home to improve accuracy
  }

  stepper_X.stop();  // Stop motor at home position
  stepper_X.setCurrentPosition(0);  // Reset position to 0 again (accurate home position)
  //Serial.println(stepper.currentPosition());
  // END X HOMING
  lcd.setCursor(0, 2);
  lcd.print("   Homing Y Axis    ");
  //HomeAxis(stepper_Y, endY);
  stepper_Y.setSpeed(-3000);  // Set speed to move toward the endstop (negative direction)
  while (digitalRead(endY1) == HIGH || digitalRead(endY2) == HIGH) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_Y.runSpeed();  // Keep moving until the endstop is triggered
  }

  stepper_Y.stop();  // Stop the motor once the endstop is hit
  stepper_Y.setCurrentPosition(0);  // Set current position as 0 (home position)

  // Optionally move away from the endstop and re-home for better accuracy
  stepper_Y.setSpeed(1000);  // Move away slowly
  stepper_Y.move(400);  // Move a small amount away from the endstop
  while (stepper_Y.distanceToGo() != 0) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_Y.run();  // Run until we've moved a small distance away
  }

  // Re-home at slower speed
  stepper_Y.setSpeed(-500);  // Slow speed toward endstop
  while (digitalRead(endY1) == HIGH || digitalRead(endY2) == HIGH) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }
    stepper_Y.runSpeed();  // Re-home to improve accuracy
  }
  GoTo(0,-50); // pour bien axer les axes Y

  stepper_Y.stop();  // Stop motor at home position
  stepper_Y.setCurrentPosition(0);  // Reset position to 0 again (accurate home position)
}
