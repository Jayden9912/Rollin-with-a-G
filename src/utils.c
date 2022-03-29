float HOME_HEADING = 90;

float ball_dist_threshold = 17.7;
float gap_ball_threshold = 5;
// float avoid_dist_threshold;

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

int detect_line(){
    if (SensorValue[right_line]==0 || SensorValue[left_line]==0){
        return 1; 
    }
    return 0;
}

int search_ball(){
    if (SensorValue[right_ir]<=ball_dist_threshold && SensorValue[left_ir]<=ball_dist_threshold){
        return 1;
    }
    return 0;
}

void release_ball(){
    motor[ball_servo] = 127;
}

void home(){ // calibrate HOME and compass direction
    float current_heading = compass();
    // facing direction
    // front
    if (compass() == HOME_HEADING){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('b', 0.75, 0.75);
        }
        else{
            release_ball();
        }
    }
    // front right
    else if (compass() == 45){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1);
            move('b', 0.75, 0.75);          
        }
        else{
            release_ball();
        }
    }
    // front left
    else if (compass() == 135){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1);
            move('b', 0.75, 0.75);            
        }
        else{
            release_ball();
        }
    }
    // left
    else if (compass() == 180){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1);
            move('b', 0.75, 0.75);            
        }
        else{
            release_ball();
        }
    }
    // left back
    else if (compass() == 225){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('r', 1, 1);
            move('b', 0.75, 0.75);           
        }
        else{
            release_ball();
        }
    }
    // right
    else if (compass() == 0){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1);
            move('b', 0.75, 0.75);           
        }
        else{
            release_ball();
        }
    }
    // right back
    else if (compass() == 315){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1);
            move('b', 0.75, 0.75);           
        }
        else{
            release_ball();
        }
    }
    // back
    else if (compass() == 270){
        if (SensorValue[home_limit_left]==LIMIT_NOT_PRESSED){
            move('l', 1, 1);
            move('b', 0.75, 0.75);           
        }
        else{
            release_ball();
        }
    }
}

void pan_search_collect(){
    move('r', 1, 1);
    while(search_ball()==0 && detect_line()==0){
        move('r', 0.5, 0.5);
        move('1', 0.5, 0.5);
    }
    while(SensorValue[right_ir]>gap_ball_threshold && SensorValue[left_ir]>gap_ball_threshold 
            && SensorValue[ball_limit]==LIMIT_NOT_PRESSED){
        move('f', 1, 1);
        motor[ball_roller] = 127;
        }
    if (SensorValue[ball_limit]==LIMIT_PRESSED){
        home();
    }
}

