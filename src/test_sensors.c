#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)

int left_ir_value, right_ir_value, front_ir_value;

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

task main()
{
    IRSensor LeftSensor, RightSensor, FrontSensor;
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    initializeSensor(&FrontSensor, top_ir, 25.24429, -0.9968);
    while (true)
    {
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        front_ir_value = getIRSensorReading(FrontSensor);
    }
}