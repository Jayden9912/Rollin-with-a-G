float left_duration;
float right_duration;

/*Differential Drive Robot Equations:
w_left = 1/(time taken for left wheel to complete half a revolution) * PI     rad/s
w_right = 1/(time taken for right wheel to complete half a revolution) * PI   rad/s
velocity_left = (vehicle's wheel radius) * w_left;                            cm/s
velocity_right = (vehicle's wheel radius) * w_right;                          cm/s
resultant_w = velocity_left / ((distance between 2 wheels)/2);                rad/s
*/
task kinematics(){
    while(true){
        left_duration = (float)encoder.left_duration/1000;
        right_duration = (float)encoder.right_duration/1000;
        if (left_duration > 0){
            if (motor[left_motor] > 0){
                odom.w_left = PI/(left_duration);
            }
            else if (motor[left_motor] < 0){
                odom.w_left = -PI/(left_duration);
            }
            else{
                odom.w_left = 0;
            }           
        }
        if (right_duration > 0){
            if (motor[right_motor] > 0){
                odom.w_right = PI/(right_duration);
            }
            else if(motor[right_motor] < 0){
                odom.w_right = -PI/(right_duration);
            }
            else{
                odom.w_right = 0;
            }
        }
        odom.vel_left = car.wheel_radius * odom.w_left;
        odom.vel_right = car.wheel_radius * odom.w_right;
        odom.resultant_w = odom.vel_left/(car.wheels_length/2);
    }
}