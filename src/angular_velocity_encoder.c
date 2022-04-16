#include "utils.c"

int en_left_duration;
int en_right_duration;
int prev_left_duration;
int prev_right_duration;
int right_count = -1;

task left_wheel_encoder(){  
    // remember left_count+1 cause first time -1 != left_duration, left_count==0
    while(true){
        clearTimer(T1);
        while (SensorValue[left_wheel_line]==0){
        		en_left_duration = time1[T1];
        }
        if (prev_left_duration != en_left_duration){
            encoder.left_count += 1;
        }
        prev_left_duration = en_left_duration;
        encoder.left_duration = prev_left_duration;
    }
}

task right_wheel_encoder(){
    while(true){
        clearTimer(T2);
        while (SensorValue[right_wheel_line]==0){
        		en_right_duration = time1[T2];
        }
        if (prev_right_duration != en_right_duration){
            right_count += 1;
        }
        prev_right_duration = en_right_duration;
        encoder.right_duration = prev_left_duration;
    }
}
