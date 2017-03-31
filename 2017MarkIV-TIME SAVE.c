#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  liftStop,       sensorTouch)
#pragma config(Sensor, dgtl2,  leftClaw,       sensorDigitalOut)
#pragma config(Sensor, dgtl3,  rightClaw,      sensorDigitalOut)
#pragma config(Sensor, dgtl9,  leftEnc,        sensorQuadEncoder)
#pragma config(Sensor, dgtl11, rightEnc,       sensorQuadEncoder)
#pragma config(Motor,  port1,           rightWheel1,   tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           rightWheel2,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           leftWheel1,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           leftLift1,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           rightLift1,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           rightLift2,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           rightLift3,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           leftLift2,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           leftLift3,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          leftWheel2,    tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JonLib2/JonLib2.h"

/*///////////////////////////////////////////////////////////
/////____________/\\\\\____/\\\\\\\\\_____              /////
///// ________/\\\\////___/\\\///////\\\___             /////
/////  _____/\\\///_______\///______\//\\\__            /////
/////   ___/\\\\\\\\\\\______________/\\\/___           /////
/////    __/\\\\///////\\\_________/\\\//_____          /////
/////     _\/\\\______\//\\\_____/\\\//________         /////
/////      _\//\\\______/\\\____/\\\/___________        /////
/////       __\///\\\\\\\\\/____/\\\\\\\\\\\\\\\_       /////
/////        ____\/////////_____\///////////////__      /////
///// 2017 Mark V Robot                                 /////
///// Main Code                                         /////
///// Authors: Jonathan Damico (jj_damico@yahoo.com)    /////
///// Since: March 24, 2017                             /////
*////////////////////////////////////////////////////////////

//required by JonLib2
//sets speed of left wheels on robot
void setLeftWheelSpeed (int speed) {
	motor[leftWheel1] = speed;
	motor[leftWheel2] = speed;
}

//required by JonLib2
//sets speed of right wheels on robot
void setRightWheelSpeed (int speed) {
	motor[rightWheel1] = speed;
	motor[rightWheel2] = speed;
}

//required by JonLib2
//takes number of inches for the robot to travel
//returns encoder ticks for right drivebase to reach that distance
int rightEncoderCurve (int target) {
	return 27.2*target;
}

//required by JonLib2
//takes number of inches for robot to travel
//returns encoder ticks for left drivebase to reach that distance
int leftEncoderCurve (int target) {
	return 27.6*target;
}

//required by JonLib2
//takes degrees for robot to travel
//returns gyro value for degrees
//90 degrees is a right turn
int gyroCurve (int target) {
	return -10.5*target;
}

//sets all lift motors to power
void lift (int power) {
	motor[leftLift1]  = power;
	motor[rightLift1] = power;
	motor[leftLift2]  = power;
	motor[rightLift2] = power;
	motor[leftLift3]  = power;
	motor[rightLift3] = power;
}

//task to test all motors with differient button inputs
task motorPortTest() {
	while(true) {
		motor[port1] 	= vexRT(Btn7U)*127; //rightDrive1 R
		motor[port2] 	= vexRT(Btn7D)*127; //rightDrive2 R
		motor[port3] 	= vexRT(Btn7L)*127; //leftDrive1
		motor[port4] 	= vexRT(Btn7R)*127; //leftLift1
		motor[port5] 	= vexRT(Btn8U)*127; //rightLift1 R
		motor[port6] 	= vexRT(Btn8D)*127; //rightLift2
		motor[port7] 	= vexRT(Btn8L)*127; //rightLift3 R
		motor[port8] 	= vexRT(Btn8R)*127; //leftLift2 R
		motor[port9] 	= vexRT(Btn5U)*127; //leftLift3
		motor[port10]	= vexRT(Btn6U)*127; //leftDrive2
	}
}

void pre_auton() { }


void leftClose() {
	SensorValue[leftClaw] = 0;
}

void rightClose() {
	SensorValue[rightClaw] = 0;
}

void leftOpen() {
	SensorValue[leftClaw] = 1;
}

void rightOpen () {
	SensorValue[rightClaw] = 1;
}

void close () {
	SensorValue[rightClaw] = 0;
	SensorValue[leftClaw] = 0;
}

void open () {
	SensorValue[rightClaw] = 1;
	SensorValue[leftClaw] = 1;
}

#define LIFT_RAISE 2000
#define LIFT_DUMP 3000
#define LIFT_SLOW 2200
#define LIFT_DOWN -1

int liftTarget = 0;
int liftDelay = 0;
task autoMoveLift () {
	liftTarget = SensorValue[liftPot];
	while(true) {
		if(liftTarget == -1) {
			delay(liftDelay);
			liftDelay = 0;
			while(!SensorValue[liftStop] && liftTarget == -1) {
				lift(limit(-80,SensorValue[liftPot]>LIFT_SLOW?127:60));
				delay(5);
			}
			lift(-10);
		} else if (liftTarget == LIFT_DUMP) {
			delay(liftDelay);
			liftDelay = 0;
			lift(127);
			while(SensorValue[liftPot]<LIFT_DUMP-300 && liftTarget ==  LIFT_DUMP) { delay(5); }
			lift(0);
			open();
			delay(200);
		} else {
			lift(limit((liftTarget-SensorValue[liftPot])*0.16,SensorValue[liftPot]>LIFT_SLOW?127:60));
		}
		delay(25);
	}
}

#define LIFT_PIPE_UP 3261
#define LIFT_PIPE_DOWN 3298
int liftInchesTarget = -1;
bool runLiftInches = false;
task liftInches() {
	while(true) {
		if(liftInchesTarget!=-1 && runLiftInches) {
			if(SensorValue[liftPot]<2900)
				lift(40);
			if(SensorValue[liftPot]<liftInchesTarget-(40/0.7))
				lift(20);
			else if(SensorValue[liftPot]>liftInchesTarget+(60/0.7))
				lift(-60);
			else if(liftInchesTarget>SensorValue[liftPot])
				lift(0.5*(liftInchesTarget-SensorValue[liftPot]));
			else
				lift(0.7*(liftInchesTarget-SensorValue[liftPot]));
		}
		delay(25);
	}
}

void setLiftTarget (int target, int delayTime) {
	liftDelay = delayTime;
	liftTarget = target;
}

void setLiftTarget (int target) {
	setLiftTarget(target, 0);
}

void autonomousTest () {
	drivebase db;
	drivebase load;
	gyroscope g;
	clearDebugStream();
	startTask(autoMoveLift);
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	resetSensor(gyro); //.17, .5
	setLiftTarget(LIFT_RAISE);
	initPIDGyroscope(g, gyro, 0.25, 0.0001, 0.7, 30, -1, 40);
	//addGyroTargetPIDAutoPointTurn(g, 90);
	setWheelSpeed(0);

	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
	initPIDDrivebase(load, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);

	//addDrivebaseTargetPIDAuto(db, 15, 20);

	int targets[5];
	targets[0] = 48;
	targets[1] = 0;
	targets[2] = 12;
	targets[3] = 36;
	targets[4] = 0;

	for(int i = 0; i<5; i++) {
		setDrivebaseTargetPIDAuto(load, targets[i]);
		delay(200);
	}
}

task cheekyNudge () {
	delay(800);
	rightClose();
	delay(150);
	rightOpen();
}

task dab () {
	//[enc,  L, R]

	int startEnc = SensorValue[rightEnc];

	int values[3][3] = {{300, 1, 0},
	{900, 1, 1},
	{1500, 0, 0}};

	for(int i = 0; i<3; i++) {
		//writeDebugStreamLine("EXPECTED: %d, LEFT: %d, RIGHT %d", startEnc+values[i][0], values[i][1], values[i][2]);
		while(SensorValue[rightEnc]<values[i][0]+startEnc) { delay(5); }
		SensorValue[leftClaw] = values[i][1];
		SensorValue[rightClaw] = values[i][2];
	}
	setLiftTarget(LIFT_RAISE+100);
}

void autoLift() {
	////lift, after being up against walls
	//apply pressure  to walls
	//lift with 70 power
	//wait to lifit in to pipe
	//lift with pos 30 power
	//give full 127 forward
	//give 0 lift power
	//delay(100);
	//start to increment lift power from  0 to -127, 10 power for each 100/th of a  second
	setWheelSpeed(-30);
	lift(30);
	delay(200);
	int lastLiftPotValue;
	do {
		lastLiftPotValue = SensorValue[liftPot];
		delay(200);
	} while(SensorValue[liftPot]>lastLiftPotValue);
	lift(0);
	setWheelSpeed(127);
	delay(100);
	lift(0);
	delay(100);
	int liftPower = -30;
	while(liftPower>-127) {
		lift(liftPower);
		liftPower = limit(liftPower-10, 127);
		delay(75); //COULD MOVE DOWN TO 50
	}
	lift(-127);
	delay(500);
	setWheelSpeed(0);
	while(!SensorValue[liftStop]) { delay(5); }
	lift(-70);
	while(motor[leftLift1]<20) {
		lift(motor[leftLift1]+10);
		delay(75);
	}
	lift(0);
}

drivebase db;
void programmingSkils () {
	startTask(autoMoveLift);

	//init everything, consider moving to pre-auton task so everything is  initialised  before skills starts,  extra second or so
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	resetSensor(gyro);

	clearDebugStream();

	gyroscope g;
	gyroscope gLoad;
	drivebase dbLoad;
	initPIDGyroscope(g, gyro, 0.25, 0.0001, 1.5, 30, -1, 40);
	initPIDGyroscope(gLoad, gyro, 0.25, 0.0001, 2.5, 30, -1, 40);
	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
	initPIDDrivebase(dbLoad, leftEnc, rightEnc, gyro, 0.8, 0.0001, 0.7, 30, -1, 20, 0.3);

//	open();

//	delay(400);

//	addDrivebaseTargetPIDAuto(db, 43);

//	close();
//	/*setWheelSpeed(127);
//	delay(350);
//	setLiftTarget(LIFT_RAISE);
//	delay(200);
//	setWheelSpeed(0);
//	delay(150);
//	setDrivebaseTargetPIDAuto(db, 12, 11);
//*/
//	setWheelSpeed(127);
//	delay(550);
//	setLiftTarget(LIFT_RAISE);
//	delay(150);
//	setWheelSpeed(0);
//	delay(100);
//	setDrivebaseTargetPIDAuto(db, 10, 5);

//	setGyroTargetPIDAutoRightSwingTurn(gLoad, 90);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	addDrivebaseTargetPIDAuto(dbLoad,  -41);
//	setLiftTarget(LIFT_DUMP);
//	while(!SensorValue[leftClaw]) { delay(5); }
//	setWheelSpeed(-30);

//	liftDelay = 0;
//	liftTarget = -1;
//	delay(300);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);

//	addDrivebaseTargetPIDAuto(db, 32,37);
//	delay(200);
//	close();
//	setLiftTarget(LIFT_RAISE);
//	addDrivebaseTargetPIDAuto(db,  -44);
//	setLiftTarget(LIFT_DUMP);
//	while(!SensorValue[leftClaw]) { delay(5); }
//	setWheelSpeed(-30);
//	delay(100);

//	resetSensor(gyro);
//	setLiftTarget(-1, 0);
//	while(!SensorValue[liftPot]>LIFT_RAISE) {
//		liftDelay = 0;
//		liftTarget = -1;
//		delay(5);
//	}
//	addGyroTargetPIDAutoPointTurn(g, 80);
//	delay(500);
//	rightClose();
//	setGyroTargetPIDAutoPointTurn(g, 8);
//	rightOpen();
//	delay(300);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	addDrivebaseTargetPIDAuto(db, 42);
//	delay(500);
//	close();
//	setLiftTarget(LIFT_RAISE);
//	addDrivebaseTargetPIDAuto(db, -60, -40);
//	setWheelSpeed(-30);
//	setLiftTarget(LIFT_DUMP);
//	delay(700);
//	setWheelSpeed(-30);
//	delay(300);

//	//threestars close + cube
//	setLiftTarget(-1, 0);
//	delay(400);
//	leftClose();
//	while(!SensorValue[liftStop]) { delay(5); }
//	setWheelSpeed(0);
//	resetSensor(gyro);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
//	addGyroTargetPIDAutoRightSwingTurn(g, -60);
//	addGyroTargetPIDAutoLeftSwingTurn(g,  -45);
//	setWheelSpeed(-60);
//	delay(800);
//	setWheelSpeed(-30);
//	delay(700);
//	startTask(dab);
//	resetSensor(gyro);
//	delay(200);
//	addDrivebaseTargetPIDAuto(db, 49);
//	close();
//	setLiftTarget(LIFT_RAISE+100);
//	delay(500);
//	setGyroTargetPIDAutoRightSwingTurn(g, 90);
//	setWheelSpeed(-70);
//	setLiftTarget(LIFT_DUMP);
//	delay(700);
//	setLiftTarget(LIFT_DOWN);
//	delay(200);
//	while(!SensorValue[liftStop]) { delay(5); } //consider adding timeout function in case star prevents limit switch being hit
//	addDrivebaseTargetPIDAuto(db, 1);
//	delay(100);
//	close();
//	setLiftTarget(LIFT_RAISE);
//	setWheelSpeed(-127);
//	delay(400);
//	setLiftTarget(LIFT_DUMP);
//	setWheelSpeed(-30);
//	delay(700);

//	//twostar
//	setLiftTarget(LIFT_DOWN);
//	while(!SensorValue[liftStop]) { delay(5); }
//	initPIDGyroscope(g, gyro, 0.25, 0.0001, 1.5, 30, -1, 40);
//	resetSensor(gyro);
//	addGyroTargetPIDAutoRightSwingTurn(g, -65);
//	addGyroTargetPIDAutoLeftSwingTurn(g,  -35);
//	delay(300);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
//	addDrivebaseTargetPIDAuto(db, 35);
//	leftClose();
//	addGyroTargetPIDAutoLeftSwingTurn(g, 40);
//	open();
//	addGyroTargetPIDAutoLeftSwingTurn(g, 63);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
//	addDrivebaseTargetPIDAuto(db, 30);
//	delay(500);
//	close();
//	setLiftTarget(LIFT_RAISE);
//	delay(100);
//	resetSensor(leftEnc);
//	resetSensor(rightEnc);
//	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
//	addDrivebaseTargetPIDAuto(db, -40);
//	setWheelSpeed(-30);
//	setLiftTarget(LIFT_DUMP);
	delay(700);
	setWheelSpeed(-30);
	delay(500);
	setWheelSpeed(0);
	delay(100);
	lift(-127);
	delay(500);
	lift(0);

	//movetolift
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	initPIDDrivebase(db, leftEnc, rightEnc, gyro, 0.15, 0.0001, 0.7, 30, -1, 20, 0.3);
	addDrivebaseTargetPIDAuto(db, 38, 50);
	setWheelSpeed(30);
	delay(500);
	resetSensor(gyro);
	setWheelSpeed(0);
	delay(200);
	addDrivebaseTargetPIDAuto(db, -1);
	addGyroTargetPIDAutoRightSwingTurn(g, 90+45, 50);

}

task autonomous () {
	stopTask(liftInches);
	startTask(autoMoveLift);
	//setLiftTarget(LIFT_DOWN);
	//autonomousTest();

	programmingSkils();
}

void usercontrolMain () {
	stopTask(autoMoveLift);
	//startTask(liftInches);
	int power8u = -40;
	while(true) {
		tankDrive(vexRT[Ch3], vexRT[Ch2], 15);

		if(vexRT[Btn8U]) {
			runLiftInches = false;
			lift(power8u);
			power8u=limit(power8u-10,127);
		} else if(vexRT[Btn5U]) {
			runLiftInches  = false;
			lift(127);
		} else if(vexRT[Btn5D] && !SensorValue[liftStop]) {
			runLiftInches  = false;
			if(SensorValue[liftPot]>3100)
				lift(-127);
			else if(SensorValue[liftPot]<LIFT_SLOW)
				lift(-40);
			else
				lift(-80);
		}
		else if(vexRT[Btn7U]) {
			lift(20);
		} else if(vexRT[Btn7D]) {
			lift(-40);
		}
		else if(SensorValue[liftStop] && ((abs(vexRT[Ch3])+abs(vexRT[Ch2])>15))) { //hold down code
			lift(-15);
			runLiftInches = false;
		} else if(SensorValue[liftStop]) {
			lift(0);
			runLiftInches = false;
		}
		else if(SensorValue[liftPot]>3100 && !runLiftInches)
			lift(-15);
		else if(SensorValue[liftPot]<2200 && !runLiftInches)
			lift(15);
		else if(!runLiftInches){
				lift(0);
		}

		if(!vexRT[Btn8U]){
			power8u = -40;
		}

		if(vexRT[Btn8L])
			autoLift();

		//if(vexRT[Btn7U]) {
		//	liftInchesTarget = LIFT_PIPE_UP;
		//	runLiftInches = true;
		//	while(vexRT[Btn7U]) { delay(5); }
		//}

		//if(vexRT[Btn7D]) {
		//	liftInchesTarget = LIFT_PIPE_DOWN;
		//	runLiftInches = true;
		//	while(vexRT[Btn7D]) { delay(5); }
		//}

		if(vexRT[Btn6U]) {
			if(SensorValue[leftClaw])
				close();
			else
				open();
			while(vexRT[Btn6U]) { delay(5); }
		}
	delay(25);
	}
}

task usercontrol() {
	//startTask(motorPortTest);
	usercontrolMain();
}
