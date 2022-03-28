#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, in6, inlimit_left, sensorAnalog)
#pragma config(Sensor, in7, inlimit_right, sensorAnalog)
#pragma config(Sensor, in8, back_limit_3, sensorAnalog)
#pragma config(Sensor, dgtl1, back_limit_4,   sensorDigitalIn)
#pragma config(Sensor, dgtl2, switch, sensorDigitalIn)
#pragma config(Sensor, dgtl3, left_wheel_line, sensorDigitalIn)
#pragma config(Sensor, dgtl4, right_wheel_line, sensorDigitalIn)
#pragma config(Sensor, dgtl5, ball_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)
#pragma config(Sensor, dgtl8, compass_LSB, sensorDigitalIn)
#pragma config(Sensor, dgtl9, compass_Bit3, sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2, sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB, sensorDigitalIn)
#pragma config(Motor,  port2, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3, right_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4, roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5, servo, tmotorServoStandard, openLoop)

#include "angular_velocity_encoder.c"
#include "kinematics.c"
#include "utils.c"

task main(){
    init();
    startTask(left_wheel_encoder);
    startTask(right_wheel_encoder);
    startTask(kinematics);
    startTask(forward_kinematics);
}
