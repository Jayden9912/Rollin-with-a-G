#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)

int left_ir_value, right_ir_value, front_ir_value;
float left_avg, right_avg, front_avg;
int left_drop_edge = 0;
int left_rise_edge = 0;
int right_drop_edge = 0;
int right_rise_edge = 0;
int front_drop_edge = 0;
int front_rise_edge = 0;
int threshold = 35;
int obj_threshold = 33;
int count = 0;
int found = 0;
float interval;
int index;
int diff_threshold = 5;

const int BALL_FOUND = 1;
const int BALL_NOT_FOUND = 0;

const int ball_dist_threshold = 35;
const int obstacle_threshold = 33;
const int back_obs_threshold = 15;
const int home_threshold = 10;

float left_array[] = {0.0, 0.0, 0.0, 0.0, 0.0};
float right_array[] = {0.0, 0.0, 0.0, 0.0, 0.0};
float front_array[] = {0.0, 0.0, 0.0, 0.0, 0.0};

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
    if (distance > 35)
    {
        distance = 99;
    }
    // float distance = (float)SensorValue[sensor->pin];
    return distance;
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
            // move('f', 0, 0);
            return;
        }

        // move('l', 0.2, 0.2); // On the spot turn
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor);
    }

    // proceed to [Collect Ball]
    // move('f', 0, 0);
    found = BALL_FOUND;
    return;
}

task scan()
{
    IRSensor LeftSensor, RightSensor, FrontSensor;
    // initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    // initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    initializeSensor(&LeftSensor, left_ir, 28.76329, -1.2682);
    initializeSensor(&RightSensor, right_ir, 24.02993, -0.9768);
    initializeSensor(&FrontSensor, top_ir, 25.24429, -0.9968);
    clearTimer(T1);
    while (true)
    {
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor);
        if (time1(T1) < 100)
        {
            left_avg += (float)left_ir_value;
            right_avg += (float)right_ir_value;
            front_avg += (float)front_ir_value;
            count += 1;
        }

        else
        {
            for (int i = 0; i < 4; i++)
            {
                left_array[i] = left_array[i + 1];
                right_array[i] = right_array[i + 1];
                front_array[i] = front_array[i + 1];
            }

            left_avg = (float)left_avg / count;
            right_avg = (float)right_avg / count;
            front_avg = (float)front_avg / count;

            if (left_avg > 35)
            {
                left_array[4] = 99;
            }

            else
            {
                left_array[4] = left_avg;
            }

            if (right_avg > 35)
            {
                right_array[4] = 99;
            }

            else
            {
                right_array[4] = right_avg;
            }

            if (front_avg > 35)
            {
                front_array[4] = 99;
            }

            else
            {
                front_array[4] = front_avg;
            }

            left_avg = 0;
            right_avg = 0;
            front_avg = 0;
            count = 0;
            clearTimer(T1);
        }
    }
}

task search()
{
    for(int j=0;j<4;j++)
    {
        float left_diff = left_array[j+1] - left_array[j];
        float right_diff = right_array[j+1] - right_array[j];
        float front_diff = front_array[j+1] - front_array[j];

        if(left_diff > diff_threshold)
        {
            left_rise_edge = 1;
        }

        else if(left_diff < -1*diff_threshold)
        {
            left_drop_edge = 1;
        }
    }
}

task main()
{
    startTask(scan);
    while(true)
    {
        interval = time1(T1);
    }
}
