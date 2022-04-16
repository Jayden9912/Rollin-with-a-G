#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)


int val_left;
int val_right;
task main(){
	while(true){
		val_left = SensorValue(left_line);
		val_right = SensorValue(right_line);
	}
}
