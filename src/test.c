#pragma config(Sensor, dgtl2, switch_limit, sensorDigitalIn)

int value;

task main(){
    while(true){
        value = SensorValue[switch_limit];
    }
}
