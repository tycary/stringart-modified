#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); //Creating motorshield object

//Stepper Motor Object
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

//Initiallizing Variables
String instructions;
int instr, curPos, baudrate;


void setup() { 
  Serial.begin(115200); //Common rates: 9600, 19200, 38400, *115200*, 230400
  AFMS.begin();
  myMotor->setSpeed(200); //steps per second; Max is 200 (1 rps)
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
       myMotor->release();
      }
      else if (instr == 402){ //Home
        moveMotor(0, curPos);
      }
      else {
      moveMotor(instr, curPos);
      curPos = instr;
      }
    }
    
    while (instr == 404){
      delay(1);
    }
  }
}

//Moves stepper to target position
void moveMotor(int target, int cur){
  int difference = target - cur;
  if (difference > 0) {
    myMotor->step(difference, FORWARD, SINGLE);
  }
  else if (difference < 0){
    myMotor->step(abs(difference), BACKWARD, SINGLE);
  }
  delay(10);
}
