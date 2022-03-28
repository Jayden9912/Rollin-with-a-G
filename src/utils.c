#include "rolling.c"
#include "angular_velocity_encoder.c"
#include "kinematics.c"

float ball_dist_threshold = 17.7;
float gap_ball_threshold = 5;
float avoid_dist_threshold;

typedef struct Car{
    float wheels_length;
    float turn_radius;
} car;

typedef struct Odom{
    float icc_x;
    float icc_y;
    float x;
    float y;
    float vel_x;
    float vel_y;
    float resultant_vel;
    float w_left;
    float w_right;
    float resultant_w;
    float angle;
} odom;

typedef struct getTime{
    int forward_interval;
    int backward_interval;
} getTime;

void init(){
    odom.x = 60; // cm
    odom.y = 24; // cm
    odom.angle = compass();
    car.wheels_length = 25.5; // cm
    car.turn_radius = 3.5; // cm
}

float compass(){
	int val;
	val = SensorValue[compass_MSB]*pow(2,3) + SensorValue[compass_Bit2]*pow(2,2) + SensorValue[compass_Bit3]*2 + SensorValue[compass_LSB];
	switch(val){
        case 13: 
            return 0; // E
        case 12: 
            return 45; // NE
        case 14: 
            return 90; // N
        case 6: 
            return 135; // NW
        case 7: 
            return 180; // W
        case 3: 
            return 225; // SW
        case 11: 
            return 270; // S
        case 9: 
            return 315; // SE
        default:
            return -1;
	}
}

void move(char dir, float left_speed, float right_speed, int time){
    switch(dir){
        case 'f':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = right_speed*127;
            wait1Msec(time);
            getTime.forward_interval = time;
            break;
        case 'b':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = -right_speed*127;
            wait1Msec(time);
            getTime.backward_interval = time;
            break;
        case 'r':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = -right_speed*127;
            wait1Msec(time);
            break;
        case 'l':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = right_speed*127;
            wait1Msec(time);
            break;
    }
}

void detect_line(){
    if (SensorValue[right_line]==0 || SensorValue[left_line]==0){
        return true; 
    }
    return false;
}

void search_ball(){
    if (SensorValue[right_ir]<=ball_dist_threshold && SensorValue[left_ir]<=ball_dist_threshold){
        return true;
    }
    return false;
}

void pan_search_collect(){
    float heading;
    move('r', 1, 1, 2);
    while(!search_ball() && !detect_line()){
        move('r', 0.5, 0.5, 2);
        move('1', 0.5, 0.5, 2);
    }
    while(SensorValue[right_ir]<=gap_ball_threshold && SensorValue[left_ir]<=gap_ball_threshold){
        move('f', 1, 1, 2);
        motor[roller] = 127;
    }

}

