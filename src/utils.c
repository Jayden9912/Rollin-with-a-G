// * Condition parameters
const int BALL_FOUND = 1;
const int BALL_NOT_FOUND = 0;
const int LIMIT_PRESSED = 0;
const int LIMIT_NOT_PRESSED = 1;
const int BALL_COLLECTED = 1;
const int BALL_NOT_COLLECTED = 0;
const int LINE_NOT_DETECTED = 0;
const int LEFT_LINE_DETECTED = 1;
const int RIGHT_LINE_DETECTED = 2;

// * Thresholds
const int ball_dist_threshold = 35;
const int obstacle_threshold = 31;
const int back_obs_threshold = 15;
const int home_threshold = 12;

// * Wheel Speed
const float stop = 0;
const float slow_turn = 0.2;
const float fast_turn = 0.3;
const float spin_search_speed = 0.2;

const float left_wheel_forward_speed = 0.8;
const float right_wheel_forward_speed = 0.94;

const float return_ball_left_forward_speed = 0.3;
const float return_ball_right_forward_speed = 0.44;

const float left_wheel_backward_speed = 0.4;
const float right_wheel_backward_speed = 0.43;

const float avoid_obs_left_backward_speed = 0.4;
const float avoid_obs_right_backward_speed = 0.43;

// * Tennis holder
const int reset_speed = 27;
const int catch_speed = -60;
const int reset_time = 1500;

// * Current Action/ State
int found = 0;
int collected = 0;

// * Parameters to update the odometry in forward_pose() and rotate_pose()
float forward_duration;
float vel_left;
float ori;
float left_distance;
float changing_orientation;

typedef struct{
    int left_duration;
    int right_duration;
    int forward_interval;
    int left_count;
} Encoder;
Encoder encoder;

typedef struct{
    float wheels_length;
    float wheel_radius;
    float wheel_half_rev_dist;
} Car;
Car car;

typedef struct{
    float icc_x;
    float icc_y;
    float x;
    float y;
    float vel_x;
    float vel_y;
    float vel_left;
    float vel_right;
    float resultant_vel;
    float w_left;
    float w_right;
    float resultant_w;
    float compass_angle;
    float orientation;
} Odom;
Odom odom;


typedef struct{
	int pin;
	float factor;
	float exponent;
} IRSensor;
IRSensor FrontSensor, BackSensor, LeftSensor, RightSensor;

void initializeSensor(IRSensor* sensor, int pin, float factor, float exponent){
	sensor->pin = pin;
	sensor->factor = factor;
	sensor->exponent = exponent;
}

float compass(){
	int val;
	val = SensorValue[compass_MSB]*pow(2, 3) + SensorValue[compass_Bit2]*pow(2, 2) + SensorValue[compass_Bit3]*2 + SensorValue[compass_LSB];
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
            break;
        case 'b':
            motor[left_motor] = -left_speed*127;
            motor[right_motor] = -right_speed*127;
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

float getIRSensorReading(IRSensor* sensor){
	float volt = (float)SensorValue[sensor->pin]/4096*5;
	float distance = sensor->factor * pow(volt , sensor->exponent);
	return distance;
}

void forward_pose(){
    // move straight
    // vel_right = vel_left
    forward_duration = (float)encoder.forward_interval/1000;
    vel_left = odom.vel_left;
    ori = odom.orientation;
    odom.x += vel_left*cos(ori)*forward_duration;
    odom.y += vel_left*sin(ori)*forward_duration;
}

void rotate_pose(){
    // rotate in place
    if (encoder.left_count != -1){
        if (odom.orientation > (2*PI)){
            odom.orientation -= 2*PI;
        }
        encoder.left_count += 1;
        left_distance = encoder.left_count * car.wheel_half_rev_dist;
        changing_orientation = left_distance/((car.wheels_length/2)/100);
        odom.orientation += changing_orientation;
    }
}

int detect_line()
{
    if (SensorValue[right_line] == 0)
    {
        return RIGHT_LINE_DETECTED;
    }
    else if (SensorValue[left_line] == 0)
    {
        return LEFT_LINE_DETECTED;
    }
    else
    {
        return LINE_NOT_DETECTED;
    }
}

int search_ball(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    if (getIRSensorReading(LeftSensor) <= ball_dist_threshold && getIRSensorReading(RightSensor) <= ball_dist_threshold && getIRSensorReading(FrontSensor) > obstacle_threshold)
    {
        return BALL_FOUND;
    }
    return BALL_NOT_FOUND;
}

void spin_search(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    clearTimer(T4);
    while (search_ball(LeftSensor, RightSensor, FrontSensor) == BALL_NOT_FOUND)
    {
        if (time1(T4) > 4500)
        { // * tested on actual surface
            // proceed to [Moving]
            move('f', stop, stop);
            return;
        }
        move('l', spin_search_speed, spin_search_speed); // On the spot turn
    }
    // proceed to [Collect Ball]
    move('f', stop, stop);
    found = BALL_FOUND;
    return;
}

void avoid_line()
{
    move('b', left_wheel_backward_speed, right_wheel_backward_speed);
    motor[ball_roller] = 0;
    wait1Msec(400);
    if (detect_line() == LEFT_LINE_DETECTED)
    {
        move('r', fast_turn, fast_turn);
        wait1Msec(600);
    }
    else
    {
        move('l', fast_turn, fast_turn);
        wait1Msec(600);
    }
}

void avoid_obs(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    motor[ball_roller] = 0;
    move('f', stop, stop);
    wait1Msec(300);

    move('r', slow_turn, slow_turn);
    wait1Msec(600);
    move('f', stop, stop);

    if (getIRSensorReading(LeftSensor) > obstacle_threshold && getIRSensorReading(RightSensor) > obstacle_threshold && getIRSensorReading(FrontSensor) > obstacle_threshold)
    {
        return;
    }
    move('l', slow_turn, slow_turn);
    wait1Msec(1200);
    move('f', stop, stop);
    if (getIRSensorReading(LeftSensor) > obstacle_threshold && getIRSensorReading(RightSensor) > obstacle_threshold && getIRSensorReading(FrontSensor) > obstacle_threshold)
    {
        return;
    }
    move('b', avoid_obs_left_backward_speed, avoid_obs_right_backward_speed);
    wait1Msec(400);
}

void moving(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    clearTimer(T4);
    motor[ball_roller] = -76;
    while (time1(T4) < 1200)
    { // * tested on actual surface
        if (search_ball(LeftSensor, RightSensor, FrontSensor) == BALL_FOUND)
        {
            // proceed to [Collect Ball]
            move('f', stop, stop);
            found = BALL_FOUND;
            return;
        }
        else if (detect_line() != LINE_NOT_DETECTED)
        {
            // proceed to [Avoid Line]
            avoid_line();
        }
        else if (getIRSensorReading(FrontSensor) < obstacle_threshold)
        {
            // proceed to [Avoid Obstacle]
            avoid_obs(LeftSensor, RightSensor, FrontSensor);
            clearTimer(T4);
        }
        else
        {
            move('f', left_wheel_forward_speed, right_wheel_forward_speed);
        }
    }
    move('f', stop, stop);
    return;
}

void return_prep()
{
    while (true)
    {
        if (odom.compass_angle != 135 && odom.compass_angle != 90)
        {
            move('r', fast_turn, fast_turn);
        }
        else if (odom.compass_angle == 135)
        {
            move('r', slow_turn, slow_turn);
        }
        else
        {
            move('l', slow_turn, slow_turn);
            wait1Msec(120); // * tested on actual surface
            move('f', stop, stop);
            return;
        }
    }
}

void return_ball(IRSensor *BackSensor)
{
    while (true)
    {
        if (getIRSensorReading(BackSensor) < back_obs_threshold)
        {
            move('f', stop, stop);
        }
        else if (SensorValue[avoid_limit_left] == LIMIT_PRESSED)
        {
            move('f', return_ball_left_forward_speed, return_ball_right_forward_speed);
            wait1Msec(600);
            return_prep();
        }
        else if (SensorValue[avoid_limit_right] == LIMIT_PRESSED)
        {
            move('f', return_ball_left_forward_speed, return_ball_right_forward_speed);
            wait1Msec(600);
            return_prep();
        }
        else if (SensorValue[home_limit_left] == LIMIT_PRESSED && SensorValue[home_limit_right] == LIMIT_PRESSED)
        {
            move('f', stop, stop);
            motor[ball_servo] = catch_speed;
            wait1Msec(500); // * tested on actual surface
            motor[ball_servo] = reset_speed;
            wait1Msec(reset_time); // * tested on actual surface
            motor[ball_servo] = 0;
            found = BALL_NOT_FOUND;
            return;
        }
        else
        {
            move('b', left_wheel_backward_speed, right_wheel_backward_speed);
            motor[ball_roller] = -127;
        }
    }
}

void collect_ball(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    clearTimer(T4);
    while (SensorValue[ball_limit] == LIMIT_NOT_PRESSED)
    {
        if (time1(T4) > 2300)
        { // * tested on actual surface
            found = BALL_NOT_FOUND;
            motor[ball_roller] = 0;
            move('f', stop, stop);
            return;
        }

        else if (getIRSensorReading(LeftSensor) < home_threshold && getIRSensorReading(RightSensor) < home_threshold && (odom.compass_angle == 225 || odom.compass_angle == 270 || odom.compass_angle == 315))
        {
            found = BALL_NOT_FOUND;
            motor[ball_roller] = 0;
            return_prep();
            moving(LeftSensor, RightSensor, FrontSensor);
            moving(LeftSensor, RightSensor, FrontSensor);
            return;
        }

        else if (getIRSensorReading(FrontSensor) < obstacle_threshold)
        { // obstacle detected
            avoid_obs(LeftSensor, RightSensor, FrontSensor);
            clearTimer(T4);
        }

        else if (detect_line() != LINE_NOT_DETECTED)
        {
            // proceed to [Avoid Line]
            avoid_line();
        }

        else
        {
            motor[ball_roller] = 127;
            move('f', left_wheel_forward_speed, right_wheel_forward_speed);
        }
    }

    move('f', stop, stop);
    motor[ball_roller] = 0;

    clearTimer(T4);
    while (time1(T4) < 80) 
    { 
        motor[ball_servo] = catch_speed; 
    }

    motor[ball_servo] = 0;
    collected = BALL_COLLECTED;
    return;
}

void start(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    collected = BALL_NOT_COLLECTED;
    move('f', left_wheel_forward_speed, right_wheel_forward_speed);
    wait1Msec(1200);
    moving(LeftSensor, RightSensor, FrontSensor);
    moving(LeftSensor, RightSensor, FrontSensor);
}

void init(){
    /*
    Uncomment this section to enable encoder and odometry updater
    */
    // encoder.left_duration = 0;                           // ms
    // encoder.right_duration = 0;                          // ms
    // encoder.left_count = -1;
    // car.wheels_length = 25.5;                            // cm
    // car.wheel_radius = 3.5;                              // cm
    // car.wheel_half_rev_dist = (car.wheel_radius/100)*PI; // m/rad
    // odom.x = 80;                                         // cm 
    // odom.y = 13;                                         // cm
    // odom.w_left = 0;                                     // rad/s
    // odom.w_right = 0;                                    // rad/s
    // odom.orientation = 90*PI/180;                        // rad
    // odom.resultant_vel = 0;                              // cm/s
    odom.compass_angle = compass();
    initializeSensor(&LeftSensor, left_ir, 28.76329, -1.2682);
    initializeSensor(&RightSensor, right_ir, 24.02993, -0.9768);
    initializeSensor(&FrontSensor, top_ir, 25.24429, -0.9968);
    initializeSensor(&BackSensor, back_ir, 10.570, -0.974);
    start(&LeftSensor, &RightSensor, &FrontSensor);
}