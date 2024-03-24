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

// Limit switch pin
#define threaderPin 8

// Encoder pin
#define encoderPin 10

// Motor config
#define motor1dirPin 2
#define motor1stepPin 3
#define motor2dirPin 4
#define motor2stepPin 5
#define motorInterfaceType 1
#define SPR 200  // Num of steps per revolution
#define motor1MicroStep 4
#define motor2MicroStep 4
#define MAXSPEED 1  // rps (MAX = 5.5)
#define MAXACCEL 5  // rpss (norm = 20)

// Disk config
#define MAXNODE 199  // Change for num of nodes - 1
#define MAXNAIL 399  // Change for num of nails * 2 - 1
#define CW 1
#define CCW -1

// Other constants
#define BAUDRATE 115200  // Common rates: 9600, 19200, 38400, *115200*, 230400

// Stepper motor objects
AccelStepper motor1 = AccelStepper(motorInterfaceType, motor1stepPin, motor1dirPin);
AccelStepper motor2 = AccelStepper(motorInterfaceType, motor2stepPin, motor2dirPin);

// Encoder object
AS5600 as5600;  //  use default Wire

// Limit switch
ezButton limitSwitch(threaderPin);  // create ezButton object that attach to the pin;

// Global Variables
String instructions;
int instr, curPos, baudrate, trueinst, nailos;  // NailOffset == boolean is-Odd
float gearRatio;

void setup() {
  Serial.begin(BAUDRATE);  // Common rates: 9600, 19200, 38400, *115200*, 230400
  motor1.setMaxSpeed(MAXSPEED * motor1MicroStep * SPR);
  motor2.setMaxSpeed(0.1 * motor2MicroStep * SPR);
  motor1.setAcceleration(MAXACCEL * motor1MicroStep * SPR);
  motor2.setAcceleration(20 * motor2MicroStep * SPR);
  motor2.move(-8 * motor2MicroStep);
  motor2.runToPosition();

  limitSwitch.setDebounceTime(50);  // set debounce time to 50 millisecond

  as5600.begin(encoderPin);  //  set direction pin.
  as5600.setDirection(AS5600_CLOCK_WISE);
  as5600.resetPosition();

  instr = 0;
  gearRatio = 1;
  trueinst = 0;
  nailos = 0;

  bool initial = true;

  while (initial) {  // Get gear ratio
    delay(1);        // Pause to read
    while (Serial.available() > 0) {
      instructions = Serial.readStringUntil('\n');

      gearRatio = instructions.toFloat();
      initial = false;
    }
  }

  gearRatio = gearRatio * motor1MicroStep;
}

void loop() {
  limitSwitch.loop();  // MUST call the loop() function first
  while (Serial.available() == 0) {
    delay(1);  // Pause to read
    while (Serial.available() > 0) {
      instructions = Serial.readStringUntil('\n');
      instr = instructions.toInt();

      if (instr == EXIT) {  // Exit
        break;
      }
      handleInstruction(instr);
    }
  }
}

void handleInstruction(int instr) {
  switch (instr) {
    case HOME:
      curPos = 0;
      motor1.moveTo(0);
      motor1.runToPosition();
      break;
    case PAUSE:
      break;
    case SETPOSITION:
      curPos = 0;
      as5600.resetPosition();
      break;
    case SETRATIO:
      bool loop = true;
      while (loop) {  // Get gear ratio
        delay(1);     // Pause to read
        while (Serial.available() > 0) {
          instructions = Serial.readStringUntil('\n');

          gearRatio = instructions.toFloat();
          loop = false;
        }
      }
      gearRatio = gearRatio * motor1MicroStep;
      break;
    case FORWARD:
      motor1.move(1 * gearRatio * CW);
      motor1.runToPosition();
      break;
    case BACKWARD:
      motor1.move(1 * gearRatio * CCW);
      motor1.runToPosition();
      break;
    case FORWARD360:
      motor1.move(200 * gearRatio * CW);
      motor1.runToPosition();
      break;
    case BACKWARD360:
      motor1.move(200 * gearRatio * CCW);
      motor1.runToPosition();
      break;
    case TENROTATION:
      motor1.move(2000 * gearRatio * CW);
      motor1.runToPosition();
      break;
    case ROTATIONS:
      for (int i = 0; i < 20; ++i)  // 20 rotations in 1 rotation increments
      {
        motor1.move(200 * gearRatio * CW);
        motor1.runToPosition();
      }
      break;
    default:
      if (instr <= MAXNAIL && instr >= 0) {
        execute(instr);
      }
      break;
  }
}

void execute(int instr) {
  if (instr % 2 == 0) {
    trueinst = instr / 2;
    nailos = 0;
  } else {
    trueinst = (instr - 1) / 2;
    nailos = 1;
  }
  curPos = moveMotor(trueinst, curPos, gearRatio);  // Move wheel to position
  motor2.moveTo(8 * motor2MicroStep);               // Move threader down
  motor2.runToPosition();
  delay(100);  // testing delays
  if (nailos == 0) {
    curPos = moveMotor(trueinst - 1, curPos, gearRatio);  // Move wheel to next nail
  } else {
    curPos = moveMotor(trueinst + 1, curPos, gearRatio);  // Move wheel to next nail
  }
  delay(100);        // testing delays
  motor2.moveTo(0);  // Move threader up
  motor2.runToPosition();
  Serial.println("x");
}

// Moves stepper to target position (node, not nail) and returns target position
int moveMotor(int target, int cur, float ratio) {
  if (target == MAXNODE + 1) {
    target = 0;
  }
  if (target < 0) {
    target = MAXNODE;
  }
  int difference = target - cur;
  if (difference > 100) {
    motor1.move(int(abs(difference - 200) * ratio * CCW));
    motor1.runToPosition();
  } else if (difference < -100) {
    motor1.move(int((difference + 200) * ratio * CW));
    motor1.runToPosition();
  } else if (difference > 0) {
    motor1.move(int(difference * ratio * CW));
    motor1.runToPosition();
  } else if (difference < 0) {
    motor1.move(int(abs(difference) * ratio * CCW));
    motor1.runToPosition();
  }
  delay(5);
  return target;
}
