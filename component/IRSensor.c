#pragma config(Sensor, in8,    topIRSensor,    sensorAnalog)
#pragma config(Sensor, in1,    backIRSensor,   sensorAnalog)
#pragma config(Sensor, in2,    rightIRSensor,  sensorAnalog)
#pragma config(Sensor, in3,    leftIRSensor,   sensorAnalog)
#pragma config(Sensor, dgtl12,  back_limit_switch, sensorDigitalIn)

/*
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

*/
int reading;
task main()
{
	/*
	IRSensor frontSensor;
	initializeSensor(&frontSensor, topIRSensor, 27.534, -1.207);

	while (true) {
		int reading = getIRSensorReading(frontSensor);
		writeDebugStreamLine("Reading: %d", reading);
	}
	*/
	while (true) {
		reading = SensorValue(back_limit_switch);
//writeDebugStreamLine("Reading: %d", reading);

	}

}
