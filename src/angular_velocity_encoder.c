int left_value;
int left_duration;
int see_left;
int right_value;
int right_duration;
int see_right;
int right_threshold = 500;

task left_wheel_encoder(){
    while(true){
        clearTimer(T1);
        while (SensorValue[left_wheel_line]==0){
        		left_value = 0;
        		left_duration = time1[T1];
        }
        see_left = left_duration;
        left_value = 1;
    }
}

task right_wheel_encoder(){
    while(true){
        clearTimer(T2);
        while (SensorValue[right_wheel_line]==0){
        		right_value = 0;
        		right_duration = time1[T2];
        }
        if (right_duration > right_threshold){
		      see_right = right_duration;
		      right_value = 1;
      }
    }
}
