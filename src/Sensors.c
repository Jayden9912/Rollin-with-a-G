/*
All sensors Initialization
Functions to obtain sensor values
*/

#include "rolling.c"

#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)

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

float getIRSensorReading(IRSensor* sensor){
	float volt = (float)SensorValue[sensor->pin]/4096*5;
	float distance = sensor->factor * pow(volt , sensor->exponent);
	return distance;
}

IRSensor FrontSensor, BackSensor, LeftSensor, RightSensor;

task initialize_IR(){
    initializeSensor(&FrontSensor, top_ir, 27.534, -1.207);
    initializeSensor(&BackSensor, back_ir, 27.534, -1.207);
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 27.534, -1.207);
}