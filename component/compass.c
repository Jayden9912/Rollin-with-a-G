#pragma config(Sensor, dgtl8, compass_LSB, sensorDigitalIn)
#pragma config(Sensor, dgtl9, compass_Bit3, sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2, sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB, sensorDigitalIn)

int val;

float compass(void){
	int num;
	num = SensorValue[compass_MSB]*pow(2,3) + SensorValue[compass_Bit2]*pow(2,2) + SensorValue[compass_Bit3]*2 + SensorValue[compass_LSB];
	switch(num){
		case 13:
		    return 180; // E
		case 12:
		    return 225; // NE
		case 14:
		    return 270; // N
		case 6:
		    return 315; // NW
		case 7:
		    return 0; // W
		case 3:
		    return 45; // SW
		case 11:
		    return 90; // S
		case 9:
		    return 135; // SE
		default:
		    return -1;
}
}

task main()
{
	while(true) {
        val = compass();
}

}
