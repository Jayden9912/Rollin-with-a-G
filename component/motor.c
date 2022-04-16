#pragma config(Motor,  port2,           leftMotor,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           rightMotor,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           roller,        tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           servo,         tmotorServoStandard, openLoop)


/*
void moveMotor(float leftMotorSpeed, float rightMotorSpeed, char dir, int time){
switch (dir){
case 'r':
motor[rightMotor] = (int)(rightMotorSpeed*127);
motor[leftMotor] = (int)(leftMotorSpeed*127);
sleep(time);
break;
case 'f':
motor[leftMotor] = - (int)(leftMotorSpeed*127);
motor[rightMotor] = (int)(rightMotorSpeed*127);
sleep(time);
break;
case 'b':
motor[leftMotor] = (int)(leftMotorSpeed*127);
motor[rightMotor] = - (int)(rightMotorSpeed*127);
sleep(time);
break;
case 'l':
motor[leftMotor] = - (int)(leftMotorSpeed*127);
motor[rightMotor] = - (int)(rightMotorSpeed*127);
sleep(time);
break;
}
}
*/

int motorValue;

task main()
{
	/*
	int time = 10;
	moveMotor(0.5,0.5,'l',1*1000);
	moveMotor(0.5,0.5,'r',1*1000);
	*/
	motorValue = motor[servo];
	motor[servo] = 100;
	sleep(1000);
	motor[servo] = 0;
	sleep(500);
	motor[servo] = -100;
	sleep(500);
	motor[servo] = 0;
	sleep(500);
}
