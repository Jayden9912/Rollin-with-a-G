#include "utils.c"
// #include "rolling.c"
// #include "angular_velocity_encoder.c"

float left_duration;
float right_duration;
// float forward_duration;
// float left_distance;
// float changing_orientation;

/*Differential Drive Robot Equations:
w_left = 1/(time taken for left wheel to complete half a revolution) * PI rad/s
w_right = 1/(time taken for right wheel to complete half a revolution) * PI rad/s
v_X = w_left + w_right * R/2 * cos(heading in rad)
v_Y = w_left + w_right * R/2 * sin(heading in rad)
w = (w_right - w_left) * R/L
where R is the radius of wheel and L is the distance between each wheel */
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
        // odom.resultant_vel = (car.turn_radius/2) * (odom.w_left + odom.w_right); // cm/s
        // odom.vel_x = odom.resultant_vel * cos(odom.angle); // cm/s
        // odom.vel_y = odom.resultant_vel * sin(odom.angle); // cm/s
        // odom.resultant_w = (car.turn_radius/car.wheels_length) * (odom.w_right - odom.w_left);
        odom.vel_left = car.wheel_radius * odom.w_left; // cm/s
        odom.vel_right = car.wheel_radius * odom.w_right; // cm/s
        odom.resultant_w = odom.vel_left/(car.wheels_length/2);
    }
}

// task forward_kinematics(){
//     // float vel_right;
//     // float prev_angle;
//     // float spin_angle;
//     // float interval;
//     // float icc_x_origin;
//     // float icc_y_origin;
//     // float rotate_icc_x;
//     // float rotate_icc_y;
//     while(true){
//         if (motor[left_motor] == motor[right_motor]){
//             // move straight
//             // vel_right = vel_left
//             forward_duration = (float)getTime.forward_interval/1000;
//             odom.x += odom.vel_left*cos(odom.orientation)*forward_duration;
//             odom.y += odom.vel_left*sin(odom.orientation)*forward_duration;
//         }
//         else if ((motor[left_motor] + motor[right_motor] == 0)){
//             // rotate in place
//             if (left_count != -1){
//                 left_distance = left_count * PI;
//                 changing_orientation = left_distance/(car.wheels_length/2);
//                 odom.orientation += changing_orientation;
//             }
//                 // odom.orientation += (2*abs(vel_right)*getTime.rotate_interval)/car.wheels_length;
//         }
//         // else if (motor[left_motor]== 0 && motor[right_motor]==0){
//         //     // stationary
//         // }
//         // else{
//         //     // spin at ICC with spin_angle
//         //     prev_angle = odom.angle;
//         //     spin_angle = compass() - prev_angle;
//         //     interval = spin_angle/odom.resultant_w;
//         //     icc_x_origin = car.turn_radius * sin(odom.angle);
//         //     icc_y_origin = car.turn_radius * cos(odom.angle);
//         //     rotate_icc_x = icc_x_origin*cos(spin_angle) + icc_y_origin*sin(spin_angle);
//         //     rotate_icc_y = icc_x_origin*sin(spin_angle) - icc_y_origin*cos(spin_angle);
//         //     odom.icc_x = odom.x - icc_x_origin;
//         //     odom.icc_y = odom.y + icc_y_origin;
//         //     odom.x = rotate_icc_x + odom.icc_x;
//         //     odom.y = rotate_icc_y + odom.icc_y;
//         //     odom.angle = compass() + odom.resultant_w*getTime.rotate_interval;
//         // }
//     }
// }