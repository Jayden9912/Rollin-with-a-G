#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, in6, home_limit_left, sensorAnalog)
#pragma config(Sensor, in7, home_limit_right, sensorAnalog)
#pragma config(Sensor, in8, avoid_limit_left, sensorAnalog)
#pragma config(Sensor, dgtl1, avoid_limit_right, sensorDigitalIn)
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

/*  =======================================================
    Stable Main File (tested) 
    - Only basic collect and return
    - Does not have boundary, obstacles and delivery zone
      avoidance.
    ======================================================= */

// * Condition parameters
const int BALL_FOUND = 1;
const int BALL_NOT_FOUND = 0;
const int OBS_DETECTED = 1;
const int OBS_NOT_DETECTED = 0;
const int LIMIT_PRESSED = 0;
const int LIMIT_NOT_PRESSED = 1;
const int BALL_COLLECTED = 1;
const int BALL_NOT_COLLECTED = 0;
const int LINE_NOT_DETECTED = 0;
const int LEFT_LINE_DETECTED = 1;
const int RIGHT_LINE_DETECTED = 2;

const int ball_dist_threshold = 56;
const int obstacle_threshold = 50;
const int back_obs_threshold = 15;

// * Current Action / State
int found = 0;
int collected = 0;

// * Sensors' value;
int left_ir_value;
int right_ir_value;
int front_ir_value;
int back_ir_value;

typedef struct{
	int pin;
	float factor;
	float exponent;
}IRSensor;

void initializeSensor(IRSensor* sensor, int pin, float factor, float exponent){
	sensor->pin = pin;
	sensor->factor = factor;
	sensor->exponent = exponent;
}

int getIRSensorReading(IRSensor* sensor){
	float volt = (float)SensorValue[sensor->pin]/4096*5;
	float distance = sensor->factor * pow(volt , sensor->exponent);
	return distance;
}

int compass(){
	int val;
	val = SensorValue[compass_MSB]*pow(2,3) + SensorValue[compass_Bit2]*pow(2,2) + SensorValue[compass_Bit3]*2 + SensorValue[compass_LSB];
	switch(val){
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

void move(char dir, float left_speed, float right_speed){
    switch(dir){
        case 'f':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = right_speed*127;
            // getTime.forward_interval = time;
            break;
        case 'b':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = -right_speed*127;
            // getTime.backward_interval = time;
            break;
        case 'r':
            motor[left_motor] = left_speed*127;
            motor[right_motor] = -right_speed*127;
            break;
        case 'l':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = right_speed*127;
            break;
    }
}

int detect_line(){
    // Changes added by Soon Hing
    if (SensorValue[right_line]==0) {
        return RIGHT_LINE_DETECTED;
    }

    else if (SensorValue[left_line]==0) {
        return LEFT_LINE_DETECTED;
    }

    else {
        return LINE_NOT_DETECTED;
    }
}

void start() {
    // Move pass line
    move('f', 0.5, 0.5);
    wait1Msec(1200);
    collected = BALL_NOT_COLLECTED;
    // Proceed to [Spin Search]
}

int search_ball(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* FrontSensor){
    if (getIRSensorReading(LeftSensor)<=ball_dist_threshold && getIRSensorReading(RightSensor)<=ball_dist_threshold && getIRSensorReading(FrontSensor)>obstacle_threshold){
        // return BALL_FOUND;
        return BALL_FOUND;
    }
    return BALL_NOT_FOUND;
}

void spin_search(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* FrontSensor) {
    clearTimer(T4);
    while(search_ball(LeftSensor, RightSensor, FrontSensor) == BALL_NOT_FOUND){
        if(time1(T4) > 4500) {      // * tested on actual surface
            // proceed to [Moving]
            move('f', 0, 0);
            return;
        }

        move('l', 0.22, 0.22);  // On the spot turn
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor); 
    }

    // proceed to [Collect Ball]
    move('f', 0, 0);
    found = BALL_FOUND;
    return;
}

void moving(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* FrontSensor) {
    clearTimer(T4);
    while(time1(T4) < 800) {  // * tested on actual surface
        move('f', 1, 1);
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor);

        if(search_ball(LeftSensor, RightSensor, FrontSensor) == BALL_FOUND) {
            // proceed to [Collect Ball] 
            move('f', 0, 0);
            found = BALL_FOUND;
            return;
        }

        // else if(detect_line() != LINE_NOT_DETECTED) {
        //     // proceed to [Avoid]
        //     // stop for now
        //     move('b', 0.5, 0.5);
        //     wait1Msec(800);
        //     return BALL_NOT_FOUND;
        // }

        // else if(front_ir_value < obstacle_threshold) {
        //     // proceed to [Avoid]
        //     // stop for now
        //     move('f', 0, 0);
        // }
    }

    move('f', 0, 0);
    return;
}

void collect_ball(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* FrontSensor) {
    clearTimer(T4);
    while(SensorValue[ball_limit]==LIMIT_NOT_PRESSED) {
        if(time1(T4) > 2300) {  // * tested on actual surface
            found = BALL_NOT_FOUND;
            motor[ball_roller] = 0;
            move('f', 0, 0);
            return;
        }

        motor[ball_roller] = 127;
        move('f', 1, 1);
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor);
        if(front_ir_value < 30) {   // TODO: Test Stop Condition when enemy car is in front
            // Stop for now
            move('f', 0, 0);
            clearTimer(T4);
        }

        // else if(front_ir_value < obstacle_threshold) {  // obstacle detected
        //     move('f', 0, 0);
        // }
    }

    move('f', 0, 0);
    motor[ball_roller] = 0;

    clearTimer(T4);
    while(time1(T4) < 300) {    // ? need to test again after adding shock absorb
        motor[ball_servo] = -30;
    }

    motor[ball_servo] = 0;
    collected = BALL_COLLECTED;
    return;
}

void return_prep() {
    while(true) {
        if(compass() != 135 && compass() != 90) {
            move('r', 0.3, 0.3);
        }

        else if (compass() == 135) {
            move('r', 0.2, 0.2);
        }

        else {
            move('l', 0.2, 0.2);
            wait1Msec(120);     // * tested on actual surface
            move('f', 0, 0);
            return;
        }
    }
}

void return_ball(IRSensor* BackSensor) {
    while(true) {
        back_ir_value = getIRSensorReading(BackSensor);
        if(back_ir_value < back_obs_threshold) {
            move('f', 0, 0);
        }

        else if(SensorValue[avoid_limit_left] == LIMIT_PRESSED) {
            move('l', 0.2, 0.2);
        }

        else if(SensorValue[avoid_limit_right] == LIMIT_PRESSED) {
            move('r', 0.2, 0.2);
        }

        else if(SensorValue[home_limit_left] == LIMIT_PRESSED && SensorValue[home_limit_right] == LIMIT_PRESSED) {
            move('f', 0, 0);
            motor[ball_servo] = -60;
            wait1Msec(150);     // * tested on actual surface
            motor[ball_servo] = 60;
            wait1Msec(600);     // * tested on actual surface
            found = BALL_NOT_FOUND;
            return;
        }

        else {
            move('b', 0.4, 0.4);
        }
    }
}

task main(){
    IRSensor LeftSensor, RightSensor, FrontSensor, BackSensor;
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    initializeSensor(&FrontSensor, top_ir, 25.24429, -0.9968);
    initializeSensor(&BackSensor, back_ir, 10.570, -0.974);
    // start();
    while(true) {
        if(!found && !collected) {
            spin_search(&LeftSensor, &RightSensor, &FrontSensor);
            if (!found) {
                moving(&LeftSensor, &RightSensor, &FrontSensor);
            }
        }

        else if(found && !collected) {
            collect_ball(&LeftSensor, &RightSensor, &FrontSensor);
        }

        else if(found && collected) {
            return_prep();
            return_ball(&BackSensor);
        }

        else {
            start();
        }
    }
}