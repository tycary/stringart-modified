#include <ezButton.h>
#include <AS5600.h>
#include <AccelStepper.h>

// Instruction Key
#define HOME 8000
#define EXIT 8888
#define SETRATIO 8080
#define SETPOSITION 8085
#define PAUSE 8008
#define FORWARD 8001
#define BACKWARD 8002
#define FORWARD360 8003
#define BACKWARD360 8004
#define TENROTATION 8010
#define ROTATIONS 8021
#define HOMETHREADER 8007
#define TESTTHREADER 8006

// Limit switch pin
#define threaderPin 12

// Encoder pin
#define encoderPin 10

// Motor config
#define motorWdirPin 2
#define motorWstepPin 3
#define motorTdirPin 4
#define motorTstepPin 5
#define motorInterfaceType 1
#define SPR 200 // Num of steps per revolution
#define motorWMicroStep 4
#define motorTMicroStep 4
#define MAXSPEED 1      // rps (MAX = 5.5)
#define MAXACCEL 5      // rpss (norm = 20)
#define THREADERSPEED 1 // rps (MAX = 5.5)
#define THREADERACCEL 5 // rpss (norm = 20)
#define THREADERARC 9   // Num of steps to move down

// Disk config
#define MAXNODE 199 // Change for num of nodes - 1
#define MAXNAIL 399 // Change for num of nails * 2 - 1
#define CW 1
#define CCW -1

// Other constants
#define BAUDRATE 115200 // Common rates: 9600, 19200, 38400, *115200*, 230400

// Stepper motor objects
AccelStepper motorW = AccelStepper(motorInterfaceType, motorWstepPin, motorWdirPin);
AccelStepper motorT = AccelStepper(motorInterfaceType, motorTstepPin, motorTdirPin);

// Encoder object
AS5600 as5600; //  use default Wire

// Global Variables
String instructions;
int instr, curPos, baudrate, trueinst, nailos; // NailOffset == boolean is-Odd
float gearRatio;
bool lop = true;

void setup()
{
  delay(500);
  Serial.begin(BAUDRATE);
  Wire.begin();

  // Motor setup
  motorW.setMaxSpeed(MAXSPEED * motorWMicroStep * SPR);
  motorT.setMaxSpeed(THREADERSPEED * motorTMicroStep * SPR);
  motorW.setAcceleration(MAXACCEL * motorWMicroStep * SPR);
  motorT.setAcceleration(THREADERACCEL * motorTMicroStep * SPR);

  homeThreader();

  // Encoder setup
  as5600.begin(encoderPin); //  set direction pin.
  as5600.setDirection(AS5600_CLOCK_WISE);
  as5600.resetPosition();

  instr = 0;
  gearRatio = 1;
  trueinst = 0;
  nailos = 0;

  // handleInstruction(SETRATIO);
  gearRatio = 10 * motorWMicroStep;
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

      if (instr == EXIT)
      { // Exit
        break;
      }
      handleInstruction(instr);
    }
  }
}

void homeThreader()
{
  if (digitalRead(threaderPin) == LOW)
  {
    motorT.move(4 * motorTMicroStep);
    motorT.runToPosition();
  }
  delay(100);
  if (digitalRead(threaderPin) == LOW)
  {
    motorT.move(4 * motorTMicroStep);
    motorT.runToPosition();
  }

  // Slow speed down
  motorT.setMaxSpeed(0.1 * motorTMicroStep * SPR);
  motorT.setAcceleration(5 * motorTMicroStep * SPR);
  delay(100);
  while (digitalRead(threaderPin) == HIGH)
  {
    motorT.move(1 * -1);
    motorT.runToPosition();
    delay(50);
  }

  motorT.move(2 * motorTMicroStep);
  motorT.runToPosition();

  // Set norm speed
  motorT.setMaxSpeed(THREADERSPEED * motorTMicroStep * SPR);
  motorT.setAcceleration(THREADERACCEL * motorTMicroStep * SPR);
}

void handleInstruction(int instr)
{
  switch (instr)
  {
  case HOME:
    curPos = 0;
    motorW.moveTo(0);
    motorW.runToPosition();
    break;
  case PAUSE:
    break;
  case HOMETHREADER:
    homeThreader();
    break;
  case SETPOSITION:
    curPos = 0;
    as5600.resetPosition();
    break;
  case SETRATIO:
    lop = true;
    while (lop)
    {           // Get gear ratio
      delay(1); // Pause to read
      while (Serial.available() > 0)
      {
        instructions = Serial.readStringUntil('\n');

        gearRatio = instructions.toFloat();
        lop = false;
      }
    }
    Serial.println("Ratio Set");
    gearRatio = gearRatio * motorWMicroStep;
    break;
  case FORWARD:
    motorW.move(1 * gearRatio * CW);
    motorW.runToPosition();
    break;
  case BACKWARD:
    motorW.move(1 * gearRatio * CCW);
    motorW.runToPosition();
    break;
  case FORWARD360:
    Serial.println("forward 360");

    motorW.move(200 * gearRatio * CW);
    motorW.runToPosition();
    break;
  case BACKWARD360:
    motorW.move(200 * gearRatio * CCW);
    motorW.runToPosition();
    break;
  case TENROTATION:
    motorW.move(2000 * gearRatio * CW);
    motorW.runToPosition();
    break;
  case ROTATIONS:
    for (int i = 0; i < 20; ++i) // 20 rotations in 1 rotation increments
    {
      motorW.move(200 * gearRatio * CW);
      motorW.runToPosition();
    }
    break;
  case TESTTHREADER:
    Serial.println("testing threader");
    moveThreaderDown();
    delay(3000);
    moveThreaderUp();
    break;
  default:
    if (instr <= MAXNAIL && instr >= 0)
    {
      execute(instr);
    }
    break;
  }
}

void execute(int instr)
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
  moveThreaderDown();
  delay(100); // testing delays
  if (nailos == 0)
  {
    curPos = moveMotor(trueinst - 1, curPos, gearRatio); // Move wheel to next nail
  }
  else
  {
    curPos = moveMotor(trueinst + 1, curPos, gearRatio); // Move wheel to next nail
  }
  delay(100); // testing delays
  moveThreaderUp();
  Serial.println("x");
}

void moveThreaderDown()
{
  Serial.println("threader down");
  motorT.moveTo(THREADERARC * motorTMicroStep); // Move threader down
  motorT.runToPosition();
}
void moveThreaderUp()
{
  Serial.println("threader up");
  motorT.moveTo((THREADERARC - 2) * motorTMicroStep * -1); // Move threader up
  motorT.runToPosition();

  homeThreader();
}

// Moves stepper to target position (node, not nail) and returns target position
int moveMotor(int target, int cur, float ratio)
{
  if (target == MAXNODE + 1)
  {
    target = 0;
  }
  if (target < 0)
  {
    target = MAXNODE;
  }
  int difference = target - cur;
  if (difference > 100)
  {
    motorW.move(int(abs(difference - 200) * ratio * CCW));
    motorW.runToPosition();
  }
  else if (difference < -100)
  {
    motorW.move(int((difference + 200) * ratio * CW));
    motorW.runToPosition();
  }
  else if (difference > 0)
  {
    motorW.move(int(difference * ratio * CW));
    motorW.runToPosition();
  }
  else if (difference < 0)
  {
    motorW.move(int(abs(difference) * ratio * CCW));
    motorW.runToPosition();
  }
  delay(5);
  return target;
}
