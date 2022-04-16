#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)

int leftline;
int rightline;
int left_ir_value;
int right_ir_value;
int top_ir_value;
int found;
const int ball_dist_threshold = 56;

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
    IRSensor LeftSensor, RightSensor, TopSensor;
    initializeSensor(&LeftSensor, left_ir, 27.534, -1.207);
    // initializeSensor(&RightSensor, right_ir, 24.71474, -1.049);
    initializeSensor(&RightSensor, right_ir, 24.25202, -0.9794);
    // initializeSensor(&TopSensor, top_ir, 17.58419, -0.9419);
    initializeSensor(&TopSensor, top_ir, 25.24429, -0.9968);
    while(true){
        left_ir_value = getIRSensorReading(LeftSensor);
        right_ir_value = getIRSensorReading(RightSensor);
        top_ir_value = getIRSensorReading(TopSensor);
        if (left_ir_value<=ball_dist_threshold && right_ir_value<=ball_dist_threshold){
            found = 1;
        }
        else {
            found = 0;
        }
        leftline = SensorValue[left_line];
        rightline = SensorValue[right_line];
        // left_ir_value = SensorValue[left_ir];
        // right_ir_value = SensorValue[right_ir];
        // top_ir_value = SensorValue[top_ir];
    }
}
