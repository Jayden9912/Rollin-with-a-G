int check;
int check_search_ball;
int check_detect_line;
float HOME_HEADING = 90;

// TODO: Define/const
// #define LINE_NOT_DETECTED = 0;
// #define LEFT_LINE_DETECTED = 1;
// #define RIGHT_LINE_DETECTED = 2;

float ball_dist_threshold = 17.7;
float gap_ball_threshold = 5;
// float avoid_dist_threshold;
const int BALL_NOT_FOUND = 0;
const int BALL_FOUND = 1;
const int LINE_NOT_DETECTED = 0;
// int LINE_DETECTED = 1;
const int LEFT_LINE_DETECTED = 1;
const int RIGHT_LINE_DETECTED = 2;
const int OBS_NOT_DETECTED = 0;
const int OBS_DETECTED = 1;
int LIMIT_PRESSED = 0;
int LIMIT_NOT_PRESSED = 1;

typedef struct{
    float wheels_length;
    float turn_radius;
} Car;
Car car;

typedef struct{
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
} Odom;
Odom odom;


typedef struct{
	int pin;
	float factor;
	float exponent;
} IRSensor;
IRSensor FrontSensor, BackSensor, LeftSensor, RightSensor;

// typedef struct{
//     int forward_interval;
//     int backward_interval;
// } GetTime;
// GetTime getTime;

void initializeSensor(IRSensor* sensor, int pin, float factor, float exponent){
	sensor->pin = pin;
	sensor->factor = factor;
	sensor->exponent = exponent;
}

float compass(){
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

void init(){
    odom.x = 60; // cm
    odom.y = 24; // cm
    odom.angle = compass();
    car.wheels_length = 25.5; // cm
    car.turn_radius = 3.5; // cm
    initializeSensor(&FrontSensor, top_ir, 27.534, -1.207);
    initializeSensor(&BackSensor, back_ir, 27.534, -1.207);
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 27.534, -1.207);
}

float getIRSensorReading(IRSensor* sensor){
	float volt = (float)SensorValue[sensor->pin]/4096*5;
	float distance = sensor->factor * pow(volt , sensor->exponent);
	return distance;
}

int detect_car(){ // front and back sensor only
    float front_distance = getIRSensorReading(FrontSensor);
    float back_distance = getIRSensorReading(BackSensor);
    if (front_distance<10 && back_distance<10){
        return OBS_DETECTED;
    }
    return OBS_NOT_DETECTED;
}

int detect_line(){
    // Changes added by Soon Hing
    if (SensorValue[right_line]!=0) {
        return RIGHT_LINE_DETECTED;
    }

    else if (SensorValue[left_line]!=0) {
        return LEFT_LINE_DETECTED
    }

    else {
        return LINE_NOT_DETECTED;
    }
    // if (SensorValue[right_line]==0 || SensorValue[left_line]==0){
    //     return LINE_DETECTED; 
    // }
    // return LINE_NOT_DETECTED;
}

int search_ball(){
    if (getIRSensorReading(LeftSensor)<=ball_dist_threshold && getIRSensorReading(RightSensor)<=ball_dist_threshold){
        return BALL_FOUND;
    }
    return BALL_NOT_FOUND;
}

void release_ball(){
    motor[ball_servo] = 127;
}

void reverse_to_home(){
    while(SensorValue[home_limit_left]==LIMIT_NOT_PRESSED && SensorValue[home_limit_right]==LIMIT_NOT_PRESSED){
        move('b', 0.75, 0.75);
    }
    move('b', 0, 0);
    release_ball();
}

void home(){ // calibrate HOME and compass direction
    float current_heading = compass();
    // facing direction
    // front
    if (compass() == HOME_HEADING){ // S
        reverse_to_home();
    }
    else if (compass()==45 && compass()==0 && compass()==315 && compass()==270){ // SW, W, NW, N
        while(compass() != HOME_HEADING){
            move('l', 0.5, 0.5);
        }
    }
    else if (compass()==135 && compass()==180 && compass()==225){ // SE, E, NE
        while(compass() != HOME_HEADING){
            move('r', 0.5, 0.5);
        }
    }
}

void find_ball_stop(){
    check_search_ball = search_ball();
    check_detect_line = detect_line();
    while(search_ball()==BALL_NOT_FOUND && detect_line()==LINE_NOT_DETECTED){
        check = 100;
        move('l', 0.5, 0.5);
        // wait1Msec(3000);
        // move('r', 0.5, 0.5);
        // wait1Msec(3000);
    }
    check = 0;
    move('f', 0, 0);
}

void search_collect_home(){
    move('f', 1, 1);
    wait1Msec(3000);
    move('r', 0.5, 0.5);
    wait1Msec(2000);
    while(search_ball()==BALL_NOT_FOUND && detect_line()==LINE_NOT_DETECTED){
        move('l', 0.5, 0.5);
        // wait1Msec(3000);
        // move('r', 0.5, 0.5);
        // wait1Msec(3000);
    }
    move('f', 0, 0);
    while(SensorValue[right_ir]>gap_ball_threshold && SensorValue[left_ir]>gap_ball_threshold 
            && SensorValue[ball_limit]==LIMIT_NOT_PRESSED){
        move('f', 1, 1);
        motor[ball_roller] = 127;
        }
    home();
}
