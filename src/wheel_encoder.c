#pragma config(Sensor, dgtl3, left_wheel_line, sensorDigitalIn)
#pragma config(Sensor, dgtl4, right_wheel_line, sensorDigitalIn)
#pragma config(Motor,  port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor,  port3, left_motor, tmotorVex393_MC29, openLoop)

#include "angular_velocity_encoder.c"

void move(char dir, float left_speed, float right_speed, int time){
    switch(dir){
        case 'f':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = right_speed*127;
            // wait1Msec(time);
            break;
        case 'b':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = -right_speed*127;
            // wait1Msec(time);
            break;
        case 'r':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = -right_speed*127;
            // wait1Msec(time);
            break;
        case 'l':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = right_speed*127;
            // wait1Msec(time);
            break;
    }
}

task main(){
    startTask(left_wheel_encoder);
    startTask(right_wheel_encoder);
    while(true){
        move('f',0.2,0.2,5);
    }
}
