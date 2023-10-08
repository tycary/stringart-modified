#include <AccelStepper.h>

#define motor1dirPin 2
#define motor1stepPin 3
#define motor2dirPin 4
#define motor2stepPin 5
#define motorInterfaceType 1
#define SPR 200 // Num of steps per revolution
#define motor1MicroStep 4
#define motor2MicroStep 4
#define MAXSPEED 5.5 // rps (MAX = 5.5)
#define MAXACCEL 20  // rpss

#define MAXNAIL 399 // Change for num of nodes * 2 - 1
#define CW 1
#define CCW -1

// Stepper Motor Objects
AccelStepper motor1 = AccelStepper(motorInterfaceType, motor1stepPin, motor1dirPin);
AccelStepper motor2 = AccelStepper(motorInterfaceType, motor2stepPin, motor2dirPin);

// General Variables
String instructions;
int instr, curPos, baudrate, initial, trueinst, nailos; // NailOffset == boolean is-Odd
float gearRatio;

void setup()
{
  Serial.begin(115200); // Common rates: 9600, 19200, 38400, *115200*, 230400
  motor1.setMaxSpeed(MAXSPEED * motor1MicroStep * SPR);
  motor2.setMaxSpeed(6 * motor2MicroStep * SPR);
  motor1.setAcceleration(MAXACCEL * motor1MicroStep * SPR);
  motor2.setAcceleration(20 * motor2MicroStep * SPR);
  motor2.move(5 * motor2MicroStep);
  motor2.runToPosition();
  instr = 0;
  gearRatio = 1;
  initial = 0;
  trueinst = 0;
  nailos = 0;

  while (initial == 0)
  {           // Get gear ratio
    delay(1); // Pause to read
    while (Serial.available() > 0)
    {
      instructions = Serial.readStringUntil('\n');

      gearRatio = instructions.toFloat();
      initial = 1;
    }
  }
  gearRatio = gearRatio * motor1MicroStep;
}

void loop()
{
  while (Serial.available() == 0)
  {
    delay(1); // Pause to read
    while (Serial.available() > 0)
    {
      instructions = Serial.readStringUntil('\n');
      instr = instructions.toInt();

      if (instr == 404)
      { // Exit
        break;
      }
      else if (instr == 401)
      { // Pause
      }
      else if (instr == 402)
      { // Home
        curPos = 0;
        motor1.moveTo(0);
        motor1.runToPosition();
      }
      else if (instr == 403)
      { // Backward 360
        motor1.move(200 * gearRatio * CCW);
        motor1.runToPosition();
      }
      else if (instr == 405)
      { // Forward 360
        motor1.move(200 * gearRatio * CW);
        motor1.runToPosition();
      }
      else if (instr == 406)
      { // 10 rotations
        motor1.move(2000 * gearRatio * CW);
        motor1.runToPosition();
      }
      else if (instr == 407)
      {
        for (int i = 0; 0 < 20; ++i) // 20 rotations in 1 rotation increments
        {
          motor1.move(200 * gearRatio * CW);
          motor1.runToPosition();
        }
      }
      else if (instr == 408)
      {
        for (int i = 0; 0 < 20; ++i) // 20 rotations in 1 rotation increments
        {
          motor1.move(200 * gearRatio * CW);
          motor1.runToPosition();
        }
        for (int i = 0; 0 < 20; ++i) // 20 backwards rotations in 1 rotation increments
        {
          motor1.move(200 * gearRatio * CCW);
          motor1.runToPosition();
        }
      }
      else if (instr <= MAXNAIL && instr >= 0)
      {
        if (instr % 2 == 0)
        {
          trueinst = instr / 2;
          nailos = 0;
        }
        else
        {
          trueinst = (instr + 1) / 2;
          nailos = 1;
        }
        curPos = moveMotor(trueinst, curPos, gearRatio); // Move wheel to position
        motor2.moveTo(11 * motor2MicroStep);             // Move threader down
        motor2.runToPosition();
        delay(100); // testing delays
        if (nailos == 0)
        {
          curPos = moveMotor(trueinst + 1, curPos, gearRatio); // Move wheel to next nail
        }
        else
        {
          curPos = moveMotor(trueinst - 1, curPos, gearRatio); // Move wheel to next nail
        }
        delay(100);       // testing delays
        motor2.moveTo(0); // Move threader up
        motor2.runToPosition();
        Serial.println("x");
      }
    }

    while (instr == 404)
    {
      delay(1);
    }
  }
}

// Moves stepper to target position and returns target
int moveMotor(int target, int cur, float ratio)
{
  if (target == 199 + 1)
  {
    target = 0;
  }
  if (target < 0)
  {
    target = 199;
  }
  int difference = target - cur;
  if (difference > 100)
  {
    motor1.move(int(abs(difference - 200) * ratio * CCW));
    motor1.runToPosition();
  }
  else if (difference < -100)
  {
    motor1.move(int((difference + 200) * ratio * CW));
    motor1.runToPosition();
  }
  else if (difference > 0)
  {
    motor1.move(int(difference * ratio * CW));
    motor1.runToPosition();
  }
  else if (difference < 0)
  {
    motor1.move(int(abs(difference) * ratio * CCW));
    motor1.runToPosition();
  }
  delay(5);
  return target;
}
