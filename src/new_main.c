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
#pragma config(Motor, port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor, port3, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor, port4, ball_roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor, port5, ball_servo, tmotorServoStandard, openLoop)

/*  New main code. Following strictly to the flow diagram.
    Actions: [Start], [Spin Search], [Moving]
    Done:

    Flow Diagrams:
    https://lucid.app/lucidchart/46a2da68-9ec0-4c06-89a2-68c7eec17a7a/edit?invitationId=inv_3f5486dd-3901-44fb-8843-1c527fbe8aa9
    https://lucid.app/lucidchart/849bb7eb-3126-4a73-9656-b1b2c805f465/edit?invitationId=inv_d9f0799d-f4b5-4474-a0ef-a861b41ca140

    To-Do:
    1.) Tune right_ir to search tennis (left side accurate from 10-30)
    2.) How much to move for each actions. How many wait1Msec to put
    3.) avoid_line() and avoid_obs()
*/

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

// * Sensors' Value
// int left_ir_value;
// int right_ir_value;
// int front_ir_value;
// int back_ir_value;

typedef struct
{
    int pin;
    float factor;
    float exponent;
} IRSensor;

void initializeSensor(IRSensor *sensor, int pin, float factor, float exponent)
{
    sensor->pin = pin;
    sensor->factor = factor;
    sensor->exponent = exponent;
}

int getIRSensorReading(IRSensor *sensor)
{
    float volt = (float)SensorValue[sensor->pin] / 4096 * 5;
    float distance = sensor->factor * pow(volt, sensor->exponent);
    return distance;
}

int compass()
{
    int val;
    val = SensorValue[compass_MSB] * pow(2, 3) + SensorValue[compass_Bit2] * pow(2, 2) + SensorValue[compass_Bit3] * 2 + SensorValue[compass_LSB];
    switch (val)
    {
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

void move(char dir, float left_speed, float right_speed)
{
    switch (dir)
    {
    case 'f':
        motor[left_motor] = left_speed * 127;
        motor[right_motor] = right_speed * 127;
        // getTime.forward_interval = time;
        break;
    case 'b':
        motor[left_motor] = -left_speed * 127;
        motor[right_motor] = -right_speed * 127;
        // getTime.backward_interval = time;
        break;
    case 'r':
        motor[left_motor] = left_speed * 127;
        motor[right_motor] = -right_speed * 127;
        break;
    case 'l':
        motor[left_motor] = -left_speed * 127;
        motor[right_motor] = right_speed * 127;
        break;
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
    // left_ir_value = getIRSensorReading(LeftSensor);
    // right_ir_value = getIRSensorReading(RightSensor);
    // front_ir_value = getIRSensorReading(FrontSensor);
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
        // left_ir_value = getIRSensorReading(LeftSensor);
        // right_ir_value = getIRSensorReading(RightSensor);
        // front_ir_value = getIRSensorReading(FrontSensor);
    }

    // proceed to [Collect Ball]
    move('f', stop, stop);
    found = BALL_FOUND;
    return;
}

void moving(IRSensor *LeftSensor, IRSensor *RightSensor, IRSensor *FrontSensor)
{
    clearTimer(T4);
    // motor[ball_servo] = reset_speed;
    motor[ball_roller] = -76;
    while (time1(T4) < 1200)
    { // * tested on actual surface
        // left_ir_value = getIRSensorReading(LeftSensor);
        // right_ir_value = getIRSensorReading(RightSensor);
        // front_ir_value = getIRSensorReading(FrontSensor);

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
        if (compass() != 135 && compass() != 90)
        {
            move('r', fast_turn, fast_turn);
        }

        else if (compass() == 135)
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
        // back_ir_value = getIRSensorReading(BackSensor);
        if (getIRSensorReading(BackSensor) < back_obs_threshold)
        {
            move('f', stop, stop);
        }

        // ! Turn off to prevent jerking
        // else if(compass() != 135 && compass() != 90) {
        //     return_prep();
        // }

        else if (SensorValue[avoid_limit_left] == LIMIT_PRESSED)
        {
            move('f', return_ball_left_forward_speed, return_ball_right_forward_speed);
            wait1Msec(600); // 400
            return_prep();
            // move('l', 0.2, 0.2);
            // wait1Msec(300);
        }

        else if (SensorValue[avoid_limit_right] == LIMIT_PRESSED)
        {
            move('f', return_ball_left_forward_speed, return_ball_right_forward_speed);
            wait1Msec(600); // 400
            return_prep();
            // move('r', 0.2, 0.2);
            // wait1Msec(300);
        }

        else if (SensorValue[home_limit_left] == LIMIT_PRESSED && SensorValue[home_limit_right] == LIMIT_PRESSED)
        {
            move('f', stop, stop);
            motor[ball_servo] = catch_speed;
            wait1Msec(500); // * tested on actual surface //150
            motor[ball_servo] = reset_speed;
            wait1Msec(reset_time); // * tested on actual surface //600
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

        else if (getIRSensorReading(LeftSensor) < home_threshold && getIRSensorReading(RightSensor) < home_threshold && (compass() == 225 || compass() == 270 || compass() == 315))
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
            // left_ir_value = getIRSensorReading(LeftSensor);
            // right_ir_value = getIRSensorReading(RightSensor);
            // front_ir_value = getIRSensorReading(FrontSensor);
        }
    }

    move('f', stop, stop);
    motor[ball_roller] = 0;

    clearTimer(T4);
    while (time1(T4) < 80)              // 300
    {                                    // ? need to test again after adding shock absorb
        motor[ball_servo] = catch_speed; //-30
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

task main()
{
    IRSensor LeftSensor, RightSensor, FrontSensor, BackSensor;
    // initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    // initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    initializeSensor(&LeftSensor, left_ir, 28.76329, -1.2682);
    initializeSensor(&RightSensor, right_ir, 24.02993, -0.9768);
    initializeSensor(&FrontSensor, top_ir, 25.24429, -0.9968);
    initializeSensor(&BackSensor, back_ir, 10.570, -0.974);
    start(&LeftSensor, &RightSensor, &FrontSensor);
    while (true)
    {
        if (!found && !collected)
        {
            spin_search(&LeftSensor, &RightSensor, &FrontSensor);
            if (!found)
            {
                moving(&LeftSensor, &RightSensor, &FrontSensor);
            }
        }

        else if (found && !collected)
        {
            collect_ball(&LeftSensor, &RightSensor, &FrontSensor);
        }

        else if (found && collected)
        {
            return_prep();
            return_ball(&BackSensor);
        }

        else
        {
            start(&LeftSensor, &RightSensor, &FrontSensor);
        }
    }
}
