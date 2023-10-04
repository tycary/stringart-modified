#include <AccelStepper.h>

#define motor1dirPin 2
#define motor1stepPin 3
#define motor2dirPin 4
#define motor2stepPin 5
#define motorInterfaceType 1

// Stepper Motor Objects
AccelStepper motor1 = AccelStepper(motorInterfaceType, motor1stepPin, motor1dirPin);
AccelStepper motor2 = AccelStepper(motorInterfaceType, motor2stepPin, motor2dirPin);

// General Variables
String instructions;
int instr, curPos, baudrate, initial, trueinst, nailos; // NailOffset == boolean is-Odd
float gearRatio;

#define MAXNAIL 399 // Change for num of nodes * 2 - 1
#define CW BACKWARD
#define CCW FORWARD

void setup()
{
  Serial.begin(115200); // Common rates: 9600, 19200, 38400, *115200*, 230400
  AFMS.begin();
  // steps per second; Max is 500sps (2.5 rps)
  motor1.setSpeed(500);
  motor2.setSpeed(6);
  motor1.setAcceleration(250);
  motor2.setAcceleration(100);
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
      }
      else if (instr == 403)
      { // Backward 360
        motor1.moveTo(0);
      }
      else if (instr == 405)
      { // Forward 360
        motor1.moveTo(200 * gearRatio);
      }
      else if (instr == 406)
      { // Test Pattern
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
        curPos = calcTarget(trueinst, curPos, gearRatio); // Move wheel to position
        motor1.moveTo(curPos * gearRatio);
        motor2.moveTo(11); // Move threader down
        delay(100);        // testing delays
        if (nailos == 0)
        {
          curPos = calcTarget(trueinst + 1, curPos, gearRatio); // Move wheel to next nail
          motor1.moveTo(curPos * gearRatio);
        }
        else
        {
          curPos = calcTarget(trueinst - 1, curPos, gearRatio); // Move wheel to next nail
          motor1.moveTo(curPos * gearRatio);
        }
        delay(100);       // testing delays
        motor2.moveTo(0); // Move threader up
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
int calcTarget(int target, int cur, float ratio)
{
  if (target == 199 + 1)
  {
    target = 0;
  }
  if (target < 0)
  {
    target = 199;
  }
  return target;
}
