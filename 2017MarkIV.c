#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  rightEnc,       sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  leftEnc,        sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  claw,           sensorDigitalOut)
#pragma config(Sensor, dgtl12, liftStop,       sensorTouch)
#pragma config(Motor,  port1,           rightWheel1,   tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           leftWheel1,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           rightWheel2,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           rightLift1,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           leftLift1,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           rightLift2,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           rightLift3,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           leftLift2,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           leftLift3,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          leftWheel2,    tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "JonLib2/JonLib2.h"

#define LIFT_DOWN 3200
int liftOffset = 0;
task calculateLiftOffset() {
	while(true) {
		if(SensorValue[liftStop])
			liftOffset = SensorValue[liftPot]-LIFT_DOWN;
		endTimeSlice();
	}
}

int getLiftPot () {
	return SensorValue[liftPot] + liftOffset;
}

void setLeftWheelSpeed (int speed) {
	motor[leftWheel1] = speed;
	motor[leftWheel2] = speed;
}

void setRightWheelSpeed (int speed) {
	motor[rightWheel1] = speed;
	motor[rightWheel2] = speed;
}

int leftEncoderCurve (int target) {
	return target*27.882;
}

int rightEncoderCurve (int target) {
	return target*23.093;
}

void lift (int power) {
	motor[leftLift1] = power;
	motor[leftLift2] = power;
	motor[leftLift3] = power;
	motor[rightLift1] = power;
	motor[rightLift2] = power;
	motor[rightLift3] = power;
}

task motorPortTest() {
	while(true) {
		motor[port1] 	= vexRT(Btn7U)*127; //rightWheel1
		motor[port2] 	= vexRT(Btn7D)*127; //leftWheel1
		motor[port3] 	= vexRT(Btn7L)*127; //rightWheel2 R
		motor[port4] 	= vexRT(Btn7R)*127; //rightLift1 R
		motor[port5] 	= vexRT(Btn8U)*127; //leftLift1
		motor[port6] 	= vexRT(Btn8D)*127; //rightLift2 R
		motor[port7] 	= vexRT(Btn8L)*127; //rightLift3
		motor[port8] 	= vexRT(Btn8R)*127; //leftLift2 R
		motor[port9] 	= vexRT(Btn5U)*127; //leftLift3
		motor[port10]	= vexRT(Btn6U)*127; //leftDrive2
	}
}

void pre_auton() { }

task autonomous () {
	clearDebugStream();
	drivebase db;
	gyroscope g;
	resetSensor(leftEnc);
	resetSensor(rightEnc);
	resetSensor(gyro); //.17, .5
	//initPIDGyroscope(g, gyro, 0.3, 0.0001, 0.7, 30, -1, 40);
	////addGyroTargetPIDAutoPointTurn(g, 1000);
	//setWheelSpeed(0);
	//initPIDDrivebase(db, leftEnc, rightEnc, 0.2, 0.0, 0.0);
	//addDrivebaseTargetPIDAuto(db, 1000);
	//setDrivebaseTargetPIDAuto(db, 0);
	//setWheelSpeed(0);
}

task usercontrol() {
	startTask(calculateLiftOffset);
	while(true) {
		tankDrive(vexRT[Ch3], vexRT[Ch2], 10);

		if(vexRT[Btn8U])
			lift(-127);
		else if(vexRT[Btn5U])
			lift(127);
		else if(vexRT[Btn5D] && !SensorValue[liftStop]) {
			if(SensorValue[liftPot]>2100)
				lift(-127);
			else if(SensorValue[liftPot]<1600)
				lift(-40);
			else
				lift(-80);
		}
		else if(SensorValue[liftStop]) //hold down code
			lift(-15);
		else if(SensorValue[liftPot]>2300)
			lift(-20);
		else if(SensorValue[liftPot]<1400)
			lift(10);
		else
			lift(0);

		if(vexRT[Btn6U]) {
			SensorValue[claw] = !SensorValue[claw];
			while(vexRT[Btn6U]) { delay(5); }
		}

	}
}