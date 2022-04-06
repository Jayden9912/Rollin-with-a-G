#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in6, home_limit_left, sensorAnalog)
#pragma config(Sensor, in7, home_limit_right, sensorAnalog)
#pragma config(Sensor, in8, avoid_limit_left, sensorAnalog)
#pragma config(Sensor, dgtl1, avoid_limit_right, sensorDigitalIn)
#pragma config(Sensor, dgtl8, compass_LSB, sensorDigitalIn)
#pragma config(Sensor, dgtl9, compass_Bit3, sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2, sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB, sensorDigitalIn)
#pragma config(Motor,  port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor,  port3, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5, ball_servo, tmotorServoStandard, openLoop)

const int LIMIT_PRESSED = 0;
const int back_obs_threshold = 15;

int back_ir_value;
int value;  // for debug purpose only, can delete

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

void return_prep() {
    while(true) {
        value = compass();
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
            value = 999;
            return;
        }

        // else {
        //     if(compass() != 90) {
        //         move('r', 0.2, 0.2);
        //     }

        //     else {
        //         move('l', 0.2, 0.2);
        //         wait1Msec(300);
        //         move('f', 0, 0);
        //         return 0;
        //     }
        // }
    }
}

// TODO: Test drop ball and return to base condition time
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
            wait1Msec(150);     // ? need test
            motor[ball_servo] = 20;
            wait1Msec(600);     // ? need test
            return;
        }

        else {
            move('b', 0.5, 0.5);
        }
    }
}

task main() {
    IRSensor BackSensor;
    initializeSensor(&BackSensor, back_ir, 10.570, -0.974);
    return_prep();
    return_ball(&BackSensor);
}
