#pragma config(Sensor, in6, home_limit_left, sensorAnalog)
#pragma config(Sensor, in7, home_limit_right, sensorAnalog)
#pragma config(Sensor, dgtl8,  compass_LSB,    sensorDigitalIn)
#pragma config(Sensor, dgtl9,  compass_Bit3,   sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2,   sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB,    sensorDigitalIn)
#pragma config(Motor,  port1, right_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port2, left_motor, tmotorVex393_MC29, openLoop)

float HOME_HEADING = 90;
int LIMIT_PRESSED = 0;
int LIMIT_NOT_PRESSED = 1;

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

float compass(void){
	int num;
	num = SensorValue[compass_MSB]*pow(2,3) + SensorValue[compass_Bit2]*pow(2,2) + SensorValue[compass_Bit3]*2 + SensorValue[compass_LSB];
	switch(num){
		case 13:
		    return 180; // E
		case 12:
		    return 225; // NE
		case 14:
		    return 270; // N
		case 6:
		    return 315; // NW
		case 7:
		    return 0; // W
		case 3:
		    return 45; // SW
		case 11:
		    return 90; // S
		case 9:
		    return 135; // SE
		default:
		    return -1;
}
}

void home(){ // calibrate HOME and compass direction
    float current_heading = compass();
    // facing direction
    // front
    if (compass() == HOME){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('b', 0.5, 0.5, 5);
        }
        else{
            release_ball();
        }
    }
    // front right
    else if (compass() == 45){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // front left
    else if (compass() == 135){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // left
    else if (compass() == 180){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // left back
    else if (compass() == 225){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // right
    else if (compass() == 0){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // right back
    else if (compass() == 315){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
    // back
    else if (compass() == 270){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1, 2);
            move('b', 0.75, 0.75, 5);
        }
        else{
            release_ball();
        }
    }
}

task main()
{
	while(true) {
	home();
}

}
