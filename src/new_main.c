#include "utils.c"

/*  New main code. Following strictly to the flow diagram.
    Actions: [Start], [Spin Search], [Moving]
    Done:

    Flow Diagrams:
    https://lucid.app/lucidchart/46a2da68-9ec0-4c06-89a2-68c7eec17a7a/edit?invitationId=inv_3f5486dd-3901-44fb-8843-1c527fbe8aa9 
    https://lucid.app/lucidchart/849bb7eb-3126-4a73-9656-b1b2c805f465/edit?invitationId=inv_d9f0799d-f4b5-4474-a0ef-a861b41ca140 

    To-Do:
    1.) Tune right_ir to search tennis (left side accurate from 10-30)
    2.) How much to move for each actions. How many wait1Msec to put
    3.) avoid_line() and avoid_obs()
*/

int state;

void avoid_line() {
    switch(detect_line()) {
        case LEFT_LINE_DETECTED:
            move('r', 0.2, 0.2);

        case RIGHT_LINE_DETECTED:
            move('l', 0.2, 0.2);
    }
    return;
}

void avoid_obs() {
    // Do nothing
    return;
}

void start() {
    // Move pass line
    move('f', 1, 1);
    wait1Msec(3000);
    // Proceed to [Spin Search]
}

int spin_search() {
    while(/*spin_condition*/){  // TODO: Add spin termination condition (1: turn one around || 2: found ball (done))
        move('r', 0.2, 0.2);  // On the spot turn
        if(search_ball() == BALL_FOUND) {
            // Proceed to [Collect Ball]
            return BALL_FOUND;
        }
    }   
    // proceed to [Moving]
    return BALL_NOT_FOUND;
}

int moving() {
    while(/*moving_condition*/){    // TODO: Add moving termination condition (1: Distance || 2: Line detected || 3: Ball detected || 4: obstacle detected)
        if (detect_line() != LINE_NOT_DETECTED) {
            avoid_line();
            break;
        }

        else if (search_ball() == BALL_FOUND) {
            // Proceed to [Collect Ball]
            return BALL_FOUND;
        }

        else if (detect_car == OBS_DETECTED) {
            avoid_obs();
            break;
        }

        else {
            move('f', 1, 1);
        }
    }
    // Back to [Spin Search]
    return BALL_NOT_FOUND;
}

int collect_ball() {
    int ball_collected = 0;
    motor[ball_roller] = 127;
    move('f', 1, 1);
    while(!ball_collected) {
        if (SensorValue[ball_limit]!=LIMIT_NOT_PRESSED) {   // limit switch on platform triggered
            motor[ball_servo] = -60;
            ball_collected = 1;
            move('f', 0, 0);
            motor[ball_roller] = 0;
        }
    }
    return 0;
}

task main() {
    start();
    while (true) {  // TODO: Termination Condition: Ball collected
        state = spin_search();
        if (state == BALL_NOT_FOUND) {
            state = moving();
        }
        else {
            state = collect_ball();
        }
    }
}