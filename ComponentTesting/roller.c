#pragma config(Motor,  port4, roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5,           servo,         tmotorServoStandard, openLoop)


task main()
{
	while (true) {
		motor[roller] = 127;
	}
	//motor[servo] = 127;
	//sleep(10);

}
