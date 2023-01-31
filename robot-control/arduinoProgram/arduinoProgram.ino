#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); //Creating motorshield object

//Stepper Motor Object
Adafruit_StepperMotor *motor1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *motor2 = AFMS.getStepper(200, 2);

//General Variables
String instructions;
int instr, curPos, baudrate;


void setup() { 
  Serial.begin(115200); //Common rates: 9600, 19200, 38400, *115200*, 230400
  AFMS.begin();
  //steps per second; Max is 200sps (1 rps)
  myMotor1->setSpeed(200); 
  myMotor2->setSpeed(200);
  instr = 0;
}

void loop() {
  while (Serial.available() == 0){
    delay(1); //Pause to read
    while (Serial.available() > 0){
      instructions = Serial.readStringUntil('\n');

      instr = instructions.toInt();
      
      if (instr == 404){ //Exit
       break;
      }
      else if (instr == 401){ //Pause
       motor1->release();
       motor2->release();
      }
      else if (instr == 402){ //Home
        moveMotor(0, curPos, motor1);
        moveMotor(0, curPos, motor2);
      }
      else {
        moveMotor(instr, curPos, motor1);
        moveMotor(instr, curPos, motor2);
        curPos = instr;
      }
    }
    
    while (instr == 404){
      delay(1);
    }
  }
}

//Moves stepper to target position
void moveMotor(int target, int cur, Adafruit_StepperMotor *curMotor){
  int difference = target - cur;
  if (difference > 0) {
    curMotor->step(difference, FORWARD, SINGLE);
  }
  else if (difference < 0){
    curMotor->step(abs(difference), BACKWARD, SINGLE);
  }
  delay(10);
}
