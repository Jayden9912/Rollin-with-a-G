/*SHARP GP2Y0A21YK0F IR distance sensor with Arduino and SharpIR library example code. More info: https://www.makerguides.com */

// Include the library:
#include "SharpIR.h"
// Define model and input pin:
#define IRPin A0

// Create variable to store the distance:
int distance_cm;

/* Model:
GP2Y0A41SK0F; 
GP2Y0A21YK0F;
*/

// Create a new instance of the SharpIR class to the respective sensor type
SharpIR mySensor = SharpIR(SharpIR::GP2Y0A41SK0F, IRPin); 


void setup() {
  // Begin serial communication at a baudrate of 9600:
  Serial.begin(9600);
}

void loop() {
  // Get a distance measurement and store it as distance_cm:
  distance_cm = mySensor.getDistance();
  
  // Print the measured distance to the serial monitor:
  Serial.print("Mean distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  delay(1000);
}
