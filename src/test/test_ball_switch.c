#pragma config(Sensor, dgtl2, switch_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl5, ball_limit, sensorDigitalIn)
#pragma config(Motor,  port4, ball_roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5, ball_servo, tmotorServoStandard, openLoop)

int val;
int POWER_ON = 0;
int POWER_OFF = 1;

task main(){
    while(true){
	    while(SensorValue[switch_limit]==POWER_ON){
	        motor[ball_servo] = 127;
            val = SensorValue[ball_limit];
	    }
	}
}
