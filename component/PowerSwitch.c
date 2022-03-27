#pragma config(Sensor, dgtl1, Power_Switch, sensorDigitalIn)

int val;

task main(){
	while(true){
		val = SensorValue(Power_Switch);
	}
}
