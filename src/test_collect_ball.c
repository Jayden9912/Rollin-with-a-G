#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, dgtl5, ball_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)
#pragma config(Motor,  port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor,  port3, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4, ball_roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port5, ball_servo, tmotorServoStandard, openLoop)

int LIMIT_NOT_PRESSED = 1;

int left_ir_value;
int right_ir_value;
int top_ir_value;

const int BALL_NOT_FOUND = 0;
const int BALL_FOUND = 1;
const int ball_dist_threshold = 56;
const int obstacle_threshold = 30;
int found = 0;
int collected = 0;
int testing = 0;

const int LINE_NOT_DETECTED = 0;
const int LEFT_LINE_DETECTED = 1;
const int RIGHT_LINE_DETECTED = 2;

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
    if (SensorValue[right_line]!=0) {
        return RIGHT_LINE_DETECTED;
    }

    else if (SensorValue[left_line]!=0) {
        return LEFT_LINE_DETECTED;
    }

    else {
        return LINE_NOT_DETECTED;
    }
}

int search_ball(IRSensor* LeftSensor, IRSensor* RightSensor){
    if (getIRSensorReading(LeftSensor)<=ball_dist_threshold && getIRSensorReading(RightSensor)<=ball_dist_threshold){
        return BALL_FOUND;
    }
    return BALL_NOT_FOUND;
}

int spin_search(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* TopSensor) {
    while(true){  // TODO: Add spin termination condition (1: turn one around || 2: found ball (done))
        move('l', 0.3, 0.3);  // On the spot turn
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        top_ir_value = getIRSensorReading(TopSensor);
        if(search_ball(LeftSensor, RightSensor) == BALL_FOUND && top_ir_value > obstacle_threshold) {
            // Proceed to [Collect Ball]
            return BALL_FOUND;
        }
    }
    // proceed to [Moving]
    return BALL_NOT_FOUND;
}

int collect_ball(IRSensor* LeftSensor, IRSensor* RightSensor, IRSensor* TopSensor) {
    while(true) {
        if (SensorValue[ball_limit]!=LIMIT_NOT_PRESSED || collected) {   // limit switch on platform triggered (ball collected)
            collected = 1;
            motor[ball_servo] = -30;
            move('f', 0, 0);
            motor[ball_roller] = 0;
            if(time1[T4] > 300) {
                motor[ball_servo] = 0;
                return 1;
            }
        }

        else {
            motor[ball_roller] = 127;
            move('f', 1, 1);
            left_ir_value = getIRSensorReading(LeftSensor);
            right_ir_value = getIRSensorReading(RightSensor);
            top_ir_value = getIRSensorReading(TopSensor);
            testing = SensorValue[ball_limit];
            clearTimer(T4);
        }
        
        // if (!detect_line() || top_ir_value <= obstacle_threshold ) {
        //     move('f', 0, 0);
        //     // proceed to [Avoid]
        //     return 0;
        // }
    }
}

task main(){
    IRSensor LeftSensor, RightSensor, TopSensor;
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    initializeSensor(&TopSensor, top_ir, 25.24429, -0.9968);
    while(true) {
        if(!found && !collected) {
            found = spin_search(&LeftSensor, &RightSensor, &TopSensor);
        }

        // if(!found && !collected) {
            // proceed to [Moving]
            // Do Nothing for now
        // }

        else if (found && !collected) {
            collected = collect_ball(&LeftSensor, &RightSensor, &TopSensor);
            /* Just to test whether tennis ball drop out
            move('r', 0.2, 0.2);
            wait1Msec(1000);
            move('f', 1, 1);
            wait1Msec(1500);
            move('l', 0.2, 0.2);
            wait1Msec(1000);
            move('b', 1, 1);
            wait1Msec(1000); */
        }

        else {
            // proceed to [Return]
            // Stop for now
            move('f', 0, 0);
        }
    }
}
