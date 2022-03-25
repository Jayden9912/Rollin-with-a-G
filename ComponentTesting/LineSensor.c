#pragma config(Sensor, dgtl11,  LF,         sensorDigitalIn)


int val;
task main(){
	while(true){
		val = SensorValue(LF);
	}
}
