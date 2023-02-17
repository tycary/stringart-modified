#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Creating motorshield object

// Stepper Motor Object
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *motor2 = AFMS.getStepper(200, 1);

// General Variables
String instructions;
int instr, curPos, baudrate, initial, trueinst, nailos; // NailOffset == boolean is-Odd
float gearRatio;

#define MAXNAIL 399

void setup()
{
  Serial.begin(19200); // Common rates: 9600, 19200, 38400, *115200*, 230400
  AFMS.begin();
  // steps per second; Max is 200sps (1 rps)
  motor1->setSpeed(200);
  motor2->setSpeed(200);
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
        motor1->release();
        motor2->release();
      }
      else if (instr == 402)
      { // Home
        curPos = moveMotor(0, curPos, gearRatio);
      }
      else if (instr == 403){
        motor1->step(200 * gearRatio, FORWARD, DOUBLE);
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
          trueinst = (instr - 1) / 2;
          nailos = 1;
        }
        curPos = moveMotor(trueinst, curPos, gearRatio); // Move wheel to position
        motor2->step(int(5), BACKWARD, DOUBLE);         // Move threader down
        delay(500); //testing delays
        if (nailos == 0)
        {
          curPos = moveMotor(trueinst - 1, curPos, gearRatio); // Move wheel to next nail
        }
        else
        {
          curPos = moveMotor(trueinst + 1, curPos, gearRatio); // Move wheel to next nail
        }
        delay(500); //testing delays
        motor2->step(int(5), FORWARD, DOUBLE); // Move threader up
        Serial.println("x");
        delay(100);
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
    motor1->step(int(abs(difference - 200) * ratio), FORWARD, DOUBLE);
  }
  else if (difference < -100)
  {
    motor1->step(int((difference + 200) * ratio), BACKWARD, DOUBLE);
  }
  else if (difference > 0)
  {
    motor1->step(int(difference * ratio), BACKWARD, DOUBLE);
  }
  else if (difference < 0)
  {
    motor1->step(int(abs(difference) * ratio), FORWARD, DOUBLE);
  }
  delay(5);
  return target;
}
