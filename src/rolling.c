#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, in6, home_limit_left, sensorAnalog)
#pragma config(Sensor, in7, home_limit_right, sensorAnalog)
#pragma config(Sensor, in8, avoid_limit_left, sensorAnalog)
#pragma config(Sensor, dgtl1, avoid_limit_right, sensorDigitalIn)
#pragma config(Sensor, dgtl2, switch_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl3, left_wheel_line, sensorDigitalIn)
#pragma config(Sensor, dgtl4, right_wheel_line, sensorDigitalIn)
#pragma config(Sensor, dgtl5, ball_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)
#pragma config(Sensor, dgtl8, compass_LSB, sensorDigitalIn)
#pragma config(Sensor, dgtl9, compass_Bit3, sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2, sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB, sensorDigitalIn)
#pragma config(Motor,  port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor,  port3, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4, ball_roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5, ball_servo, tmotorServoStandard, openLoop)

#include "angular_velocity_encoder.c"
#include "kinematics.c"
#include "utils.c"

int POWER_ON = 0;
int POWER_OFF = 1;
int prev_time;

task main(){
    init();
    startTask(left_wheel_encoder);
    startTask(right_wheel_encoder);
    startTask(kinematics);
    // startTask(forward_kinematics);
    clearTimer(T3);
    prev_time = time1[T3];
    while(true) {
        while(SensorValue[switch_limit]==POWER_ON){
            move('f',0.5,0.54);
            // move('l',0.3,0.3);
            // avoid_line();
            // find_ball_stop();
            //search_collect_home();
        }
        getTime.forward_interval = time1[T3] - prev_time;
        forward_pose();
        // rotate_orientation();
        while(SensorValue[switch_limit]==POWER_OFF){
            move('f',0,0);
            clearTimer(T3);
            prev_time = time1[T3];
        }
    }
}
