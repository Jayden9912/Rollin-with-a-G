#pragma config(Sensor, dgtl2,  LF,         sensorDigitalIn)

int val;
task main(){
	while(true){
		val = SensorValue(LF);
	}
}
