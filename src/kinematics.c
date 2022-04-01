#include "utils.c"
#include "rolling.c"
#include "angular_velocity_encoder.c"


/*Differential Drive Robot Equations:
w_left = 1/(time taken for left wheel to complete half a revolution) * PI rad/s
w_right = 1/(time taken for right wheel to complete half a revolution) * PI rad/s
v_X = w_left + w_right * R/2 * cos(heading in rad)
v_Y = w_left + w_right * R/2 * sin(heading in rad)
w = (w_right - w_left) * R/L
where R is the radius of wheel and L is the distance between each wheel */
task kinematics(){
    while(true){
        if (motor[left_motor] > 0){
            odom.w_left = PI/(left_duration);
        }
        else if (motor[left_motor] < 0){
            odom.w_left = -PI/(left_duration);
        }
        else{
            odom.w_left = 0;
        }           
        if (motor[right_motor] > 0){
            odom.w_right = PI/(right_duration);
        }
        else if(motor[right_motor] < 0){
            odom.w_right = -PI/(right_duration);
        }
        else{
            odom.w_right = 0;
        } 
        odom.resultant_vel = (car.turn_radius/2) * (odom.w_left + odom.w_right); // cm/s
        odom.vel_x = odom.resultant_vel * cos(odom.angle); // cm/s
        odom.vel_y = odom.resultant_vel * sin(odom.angle); // cm/s
        odom.resultant_w = (car.turn_radius/car.wheels_length) * (odom.w_right - odom.w_left);
    }
}

task forward_kinematics(){
    float vel_right;
    float prev_angle;
    float spin_angle;
    float interval;
    float icc_x_origin;
    float icc_y_origin;
    float rotate_icc_x;
    float rotate_icc_y;
    while(true){
        if (motor[left_motor] == motor[right_motor]){
            // move straight
            // vel_right = vel_left
            vel_right = odom.resultant_w * (car.turn_radius + 0.5*car.wheels_length); // cm/s
            odom.x = odom.x + vel_right*cos(odom.angle)*getTime.forward_interval;
            odom.y = odom.y + vel_right*sin(odom.angle)*getTime.forward_interval;
        }
        else if ((motor[left_motor] + motor[right_motor] == 0)){
            // rotate in place
            odom.angle = compass() + (2*abs(vel_right)*getTime.rotate_interval)/car.wheels_length;
        }
        else if (motor[left_motor]== 0 && motor[right_motor]==0){
            // stationary
        }
        else{
            // spin at ICC with spin_angle
            prev_angle = odom.angle;
            spin_angle = compass() - prev_angle;
            interval = spin_angle/odom.resultant_w;
            icc_x_origin = car.turn_radius * sin(odom.angle);
            icc_y_origin = car.turn_radius * cos(odom.angle);
            rotate_icc_x = icc_x_origin*cos(spin_angle) + icc_y_origin*sin(spin_angle);
            rotate_icc_y = icc_x_origin*sin(spin_angle) - icc_y_origin*cos(spin_angle);
            odom.icc_x = odom.x - icc_x_origin;
            odom.icc_y = odom.y + icc_y_origin;
            odom.x = rotate_icc_x + odom.icc_x;
            odom.y = rotate_icc_y + odom.icc_y;
            odom.angle = compass() + odom.resultant_w*getTime.rotate_interval;
        }
    }
}