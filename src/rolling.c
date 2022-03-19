#pragma config(Motor, port2, left_motor, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor, port3, right_motor, tmotorVex393_MC29, openLoop)

#include "move.c";

task main(){
    move_motor('f');
    wait1Msec(3000);
    wait1Msec(3000);
    move_motor('b');
    wait1Msec(3000);
    wait1Msec(3000);
    move_motor('l');
    wait1Msec(3000);
    wait1Msec(3000);
    move_motor('r');
    wait1Msec(3000);
    wait1Msec(3000);
}
