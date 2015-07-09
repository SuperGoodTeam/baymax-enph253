/* ENPH 253 PRELIMINARY CODE
last updated: 070815
version: 0.1

*/

//============== LIBRARIES ==============
#include <phys253.h>
#include <LiquidCrystal.h>
//#include <Servo253.h>

#include <EEPROMex.h>
#include <EEPROMVar.h>

#include "TAPE_FOLLOWING.h"

//============== PINS ==============
//ANALOG IO
#define TAPE_SENSOR_LEFT 0
#define TAPE_SENSOR_RIGHT 1
#define IR_FRONT_1 2
#define IR_FRONT_2 3
#define IR_SIDE 4
#define TAPE_SENSOR_SIDE 5
#define TAPE_SENSOR_FRONT 7

//SERVOS
#define SERVO_FOREARM 0
#define SERVO_BASEARM 1
#define SERVO_BASE 2

//MOTORS
#define MOTOR_LEFT 0
#define MOTOR_RIGHT 1
#define MOTOR_CLAW 2

//DIGITAL IO
//INPUT
#define SWITCH_CLAW_OPEN 4
#define SWITCH_CLAW_CLOSE 5
#define SWITCH_PLUSH_DETECT 6
#define HOOK_ARM_TOUCH_ON 7
#define HOOK_ARM_TOUCH_OFF 8

//OUTPUT
#define HOOK_ARM_RETRACT 15
#define HOOK_ARM_SPRING_UP 14
#define INTAKE_ROLLER_MOTOR 12

//============== DRIVE SETTINGS ==============
//QRD DRIVE
#define QRD_THRESHOLD 200

//IR DRIVE

//ARM CONSTANTS

//SERVO POSITIONS
#define BASESERVO_POSITION_START 90//some number
#define FOREARMSERVO_POSITION_START 90//some number
#define BASE_POSITION_START 90//some number

//FLAGS
boolean upRamp=false;
boolean noSideTape=true;
boolean clawOpen=false;

#define KNOB_MAX 1023

//============== STATES ==============
enum RobotState {
  INITIALISING, PAST_DOOR, FOLLOW_TAPE_1, COLLECT_ITEM_1, FOLLOW_TAPE_2, COLLECT_ITEM_2, COLLECT_ITEM_3, COLLECT_ITEM_4, COLLECT_ITEM_5, COLLECT_ITEM_6, UP_RAMP, PAST_RAMP, IR, ZIPLINE, FINISHED, TEST, SETTINGS, STATE_MENU, TAPE_FOLLOWING
};
enum Plan { 
	FULL, NOT4, NOT5, NUM_PLAN //placeholder for lack of len(enum) function, don't delete
};

//SET STATES
RobotState currentState = INITIALISING;
RobotState lastState = INITIALISING;  

Plan currentPlan=FULL;

//============== SETUP ==============
void setup() {
  #include <phys253setup.txt>  
  
  portMode(0, INPUT) ;
  portMode(1, OUTPUT) ;
  
  LCD.clear();
  LCD.home();

  RCServo0.attach(RCServo0Output) ;
  RCServo1.attach(RCServo1Output) ;
  RCServo2.attach(RCServo2Output) ;

  //initiate servos
  setForearmServo(FOREARMSERVO_POSITION_START);
  setBasearmServo(BASESERVO_POSITION_START);
  setBaseServo(BASE_POSITION_START);
  //other stuff goes here
}

void loop() {
	switch(currentState) {
	//==============
	case INITIALISING:
		start_initialising(); //set plan
		break;
	//==============
	case FOLLOW_TAPE_1:
		motor.speed(MOTOR_LEFT,0); //set motors to 0 at first
		motor.speed(MOTOR_RIGHT,0);
		while(!startbutton()){
			TapeMenu();
		}
		while (!stopbutton() && noSideTape!=true) {
			readFollowTape_1();
			checkSideTape();
		}
		if (noSideTape==true) {
			switchState(COLLECT_ITEM_1);
		}
		break;
	//==============
   case COLLECT_ITEM_1:
		collect_item_1();
		while(noSideTape==true) { //go until you can't see sidetape anymore
			readFollowTape_1();
			checkSideTape();
		}
		switchState(FOLLOW_TAPE_2);
   break;
   //==============
   case FOLLOW_TAPE_2:
		motor.speed(MOTOR_LEFT,0); //set motors to 0 at first
		motor.speed(MOTOR_RIGHT,0);
		while(!startbutton()){
			TapeMenu();
		}
		while (!stopbutton() && noSideTape!=true) {
			readFollowTape_2();
			checkSideTape();
		}
		if (noSideTape==true) {
			switchState(COLLECT_ITEM_2);
		}
		break;
   //==============
   case COLLECT_ITEM_2:
		collect_item_2();
		while(noSideTape==true) { //go until you can't see sidetape anymore
			readFollowTape_2();
			checkSideTape();
		}
		switchState(FOLLOW_TAPE_3);
   break;
   //==============
   case COLLECT_ITEM_3:
   break;
   case COLLECT_ITEM_4:
   break;
   case COLLECT_ITEM_5:
   break;
   case COLLECT_ITEM_6:
   break;
   case UP_RAMP:
   break;
   case PAST_RAMP:
   break;
   case IR:
   break;
   case ZIPLINE:
   break;
   case FINISHED:
   break;
  }
}

//============== STATE FUNCTIONS ==============
void switchState(byte byteRobotState){
	RobotState newState=(RobotState)byteRobotState;
	exitState(currentState);
	LCDStateChangePrint(byteRobotState);
	setupState(newState); //simple for now
	currentState=newState;
}

void setupState(byte byteRobotState)
{
  RobotState state = (RobotState)byteRobotState;
  switch (state)
  {
  case PAST_DOOR:
    break;
  case FOLLOW_TAPE_1:
	start_followtape_1();
	break;
  case COLLECT_ITEM_1:
	start_collect_item_1();
	break;
  case FOLLOW_TAPE_2:
	start_followtape_2();
	break;
  case COLLECT_ITEM_2:
	start_collect_item_2();
	break;
  case COLLECT_ITEM_3:
	break;
  case COLLECT_ITEM_4:
	break;
  case COLLECT_ITEM_5:
	break;
  case COLLECT_ITEM_6:
	break;
  case UP_RAMP:
    break;
  case PAST_RAMP:
    break;
  case IR:
    break;
  case ZIPLINE:
    break;
  case FINISHED:
    break;
  case TEST:
    break;
  case SETTINGS:
    break;
  }
}

void exitState(byte byteRobotState)
{
  RobotState state = (RobotState)byteRobotState;
  switch (state)
  {
  case INITIALISING:
	break;
  case PAST_DOOR:
    break;
  case FOLLOW_TAPE_1: //nothing, really
    break;
  case COLLECT_ITEM_1:
	exit_collect_item_1();
	break;
  case FOLLOW_TAPE_2: //nothing, really
    break;
  case COLLECT_ITEM_2:
	break;
  case COLLECT_ITEM_3:
	break;
  case COLLECT_ITEM_4:
	break;
  case COLLECT_ITEM_5:
	break;
  case COLLECT_ITEM_6:
	break;
  case UP_RAMP:
    break;
  case PAST_RAMP:
    break;
  case IR:
    break;
  case ZIPLINE:
    break;
  case FINISHED:
    break;
  case TEST:
    break;
  case SETTINGS:
    break;
  }
}

//============== nice print/string functions ==============
String byteStateToString(byte byteRobotState) {
	RobotState stringRobotState = (RobotState)byteRobotState;
	switch(stringRobotState) {
	case INITIALISING:
		return("INITIALISING");
	case PAST_DOOR:
		return("PAST DOOR");
	case FOLLOW_TAPE_1:
		return("FOLLOW TAPE 1");
	case COLLECT_ITEM_1:
		return("COLLECT ITEM 1");
	case FOLLOW_TAPE_2:
		return("FOLLOW TAPE 2`");
	case COLLECT_ITEM_2:
		return("COLLECT ITEM 2");
	case COLLECT_ITEM_3:
		return("COLLECT ITEM 3");
	case COLLECT_ITEM_4:
		return("COLLECT ITEM 4");
	case COLLECT_ITEM_5:
		return("COLLECT ITEM 5");
	case COLLECT_ITEM_6:
		return("COLLECT ITEM 6");
	case UP_RAMP:
		return("UP RAMP");
	case PAST_RAMP:
		return("PAST RAMP");
	case IR:
		return("IR");
	case ZIPLINE:
		return("ZIPLINE");
	case FINISHED:
		return("FINISHED");
	case TEST:
		return("TESTING");
	case SETTINGS:
		return("SETTINGS");
	case STATE_MENU:
		return("STATE MENU");
	}
}

void LCDStateChangePrint(byte byteRobotState) {
	String stringRobotState=byteStateToString(byteRobotState);
	LCD.clear();
	LCD.home();
	LCD.setCursor(0,0);
	LCD.print("SWITCH:");
	LCD.setCursor(0,1);
	LCD.print(stringRobotState);
}

String bytePlanToString(byte byte_Plan){
  Plan stringPlan = (Plan)byte_Plan;
  switch(stringPlan) {
  case FULL:
   return("FULL");
  case NOT4:
    return("ALL BUT 4");
  case NOT5:
    return("ALL BUT 5");
  }
}

//============== SET POSITION/CHECK FUNCTIONS ==============
void setForearmServo(int angle) {
	RCServo0.write(angle);
}

void setBasearmServo(int angle) {
	RCServo1.write(angle);
}

void setBaseServo(int angle) {
	RCServo2.write(angle);
}

void checkSideTape() {
	if (analogRead(TAPE_SENSOR_SIDE) < QRD_THRESHOLD) {
		noSideTape=false;
	}
	else {
		noSideTape=true;
	}
}

void checkClawOpen(){
 if (digitalRead(SWITCH_CLAW_OPEN)){
    clawOpen=true;
   } 
   else {
    clawOpen=false; 
   } 
}