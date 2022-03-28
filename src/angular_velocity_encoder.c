#include "rolling.c"

int left_duration;
int right_duration;
int threshold; //set threshold
task left_wheel_encoder(){
    while(true){
        ClearTimer(T1);
        while (sensorValue[left_wheel_line]==1){
            continue;
        }
        if (time1[T1] >= threshold){
            left_duration = time1[T1]*1000;
        } 
    }
}

task right_wheel_encoder(){
    while(true){
        ClearTimer(T1);
        while (sensorValue[right_wheel_line]==1){
            continue;
        }
        if (time1[T1] >= threshold){
            right_duration = time1[T1]*1000;
        } 
    }
}
