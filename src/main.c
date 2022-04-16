#pragma config(Sensor, in2, right_ir, sensorAnalog)
#pragma config(Sensor, in3, left_ir, sensorAnalog)
#pragma config(Sensor, in4, back_ir, sensorAnalog)
#pragma config(Sensor, in5, top_ir, sensorAnalog)
#pragma config(Sensor, in6, home_limit_left, sensorAnalog)
#pragma config(Sensor, in7, home_limit_right, sensorAnalog)
#pragma config(Sensor, in8, avoid_limit_left, sensorAnalog)
#pragma config(Sensor, dgtl1, avoid_limit_right, sensorDigitalIn)
#pragma config(Sensor, dgtl5, ball_limit, sensorDigitalIn)
#pragma config(Sensor, dgtl6, left_line, sensorDigitalIn)
#pragma config(Sensor, dgtl7, right_line, sensorDigitalIn)
#pragma config(Sensor, dgtl8, compass_LSB, sensorDigitalIn)
#pragma config(Sensor, dgtl9, compass_Bit3, sensorDigitalIn)
#pragma config(Sensor, dgtl10, compass_Bit2, sensorDigitalIn)
#pragma config(Sensor, dgtl11, compass_MSB, sensorDigitalIn)
#pragma config(Motor, port2, right_motor, tmotorVex393_MC29, reversed)
#pragma config(Motor, port3, left_motor, tmotorVex393_MC29, openLoop)
#pragma config(Motor, port4, ball_roller, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor, port5, ball_servo, tmotorServoStandard, openLoop)

#include "utils.c"

task main()
{
    init();
    while (true)
    {
        if (!found && !collected)
        {
            spin_search(&LeftSensor, &RightSensor, &FrontSensor);
            if (!found)
            {
                moving(&LeftSensor, &RightSensor, &FrontSensor);
            }
        }
        else if (found && !collected)
        {
            collect_ball(&LeftSensor, &RightSensor, &FrontSensor);
        }
        else if (found && collected)
        {
            return_prep();
            return_ball(&BackSensor);
        }
        else
        {
            start(&LeftSensor, &RightSensor, &FrontSensor);
        }
    }
}
