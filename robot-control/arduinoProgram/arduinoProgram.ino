#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); //Creating motorshield object

//Stepper Motor Object
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

String instructions;
int inst, curPos;

void setup() {
  Serial.begin(115200);
  AFMS.begin();
  myMotor->setSpeed(200); //steps per second; Max is 200 (1 rps)
  inst = 0;
}

void loop() {
  while (Serial.available() == 0){
    delay(1); //Pause to read
    while (Serial.available() > 0){
      instructions = Serial.readStringUntil('\n');
      //Serial.println(instructions);
      inst = instructions.toInt();
      
      if (inst == 404){ //Exit
       break;
      }
      else if (inst == 401){ //Pause
       myMotor->release();
      }
      else if (inst == 402){ //Home
        moveMotor(0, curPos);
      }
      else {
      moveMotor(inst, curPos);
      curPos = inst;
      }
    }
    
    while (instructions.toInt() == 404){
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
  delay(250);
}
