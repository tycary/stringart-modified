#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // Creating motorshield object

// Stepper Motor Object
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *motor2 = AFMS.getStepper(200, 2);

// General Variables
String instructions;
int instr, curPos, baudrate, initial;
float gearRatio;

void setup()
{
  Serial.begin(115200); // Common rates: 9600, 19200, 38400, *115200*, 230400
  AFMS.begin();
  // steps per second; Max is 200sps (1 rps)
  motor1->setSpeed(200);
  motor2->setSpeed(200);
  instr = 0;
  gearRatio = 1;
  initial = 0;
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
        moveMotor(0, curPos, gearRatio);
      }
      else if (instr < 200)
      {
        moveMotor(instr, curPos, gearRatio); // Move wheel to position
        curPos = instr;
        motor2->step(int(50 * gearRatio), BACKWARD, SINGLE); // Move threader down
        moveMotor(instr + 1, curPos, gearRatio);
        curPos = instr;
        motor2->step(int(50 * gearRatio), FORWARD, SINGLE); // Move threader up
      }
    }

    while (instr == 404)
    {
      delay(1);
    }
  }
}

// Moves stepper to target position
void moveMotor(int target, int cur, float ratio)
{
  int difference = target - cur;
  if (difference > 100)
  {
    motor1->step(int((difference - 100) * ratio), BACKWARD, SINGLE);
  }
  else if (difference < -100)
  {
    motor1->step(int(abs(difference + 100) * ratio), FORWARD, SINGLE);
  }
  else if (difference > 0)
  {
    motor1->step(int(difference * ratio), FORWARD, SINGLE);
  }
  else if (difference < 0)
  {
    motor1->step(int(abs(difference) * ratio), BACKWARD, SINGLE);
  }
  delay(10);
}
