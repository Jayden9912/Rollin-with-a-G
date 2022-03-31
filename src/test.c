#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)
#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)

int left_value;
int right_value;
int left_ir_value;
int right_ir_value;

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

task main(){
    IRSensor LeftSensor, RightSensor;
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    initializeSensor(&RightSensor, right_ir, 47.193, -0.699);
    while(true){
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        left_value = SensorValue[left_line];
        right_value = SensorValue[right_line];
    }
}