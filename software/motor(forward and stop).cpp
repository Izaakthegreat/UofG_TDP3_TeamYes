#include "mbed.h"
#include "TCS3472_I2C.h"

// https://os.mbed.com/users/Allar/code/TCS3472_I2C/
// ensure library has char cast to char fixed

// Pin initialisation
TCS3472_I2C rgb_sensor_1(PTE0, PTE1);

TCS3472_I2C rgb_sensor_0(PTC9, PTC8);

// Define magnet control
DigitalOut magnet(PTE31);

// Define L298N motor control pins
PwmOut PwmR(PTA12);//control right 
DigitalOut Rforward(PTA2);  // Adjust these pin assignments as per your connection
DigitalOut Rbackward(PTD4);

PwmOut PwmL(PTA1);//control left
DigitalOut Lforward(PTA5);
DigitalOut Lbackward(PTA4);

// Define motor control 
void control_R(float dutycycle, bool direction) {
    PwmR = dutycycle;
    if (direction==1){
    Rforward = 1;
    Rbackward = 0;
    }
    else{
    Rforward = 0;
    Rbackward = 1;
    }
}
void stopMotor() {
    Rforward = 0;
    Rbackward = 0;
    Lforward = 0;
    Lbackward = 0;

}
void control_L(float dutycycle, bool direction) {
    PwmL = dutycycle;
    if(direction==1){
    Lforward = 1;
    Lbackward = 0;
    }
    else{
    Lforward = 0;
    Lbackward = 1;
    }
}

void magnetcontrol(){
    magnet=1;
}

int main() {
    magnetcontrol();
  // Initialising the sensor values - sent through I2C to sensor peripheral
  rgb_sensor_0.enablePowerAndRGBC();
  rgb_sensor_0.setIntegrationTime(10);
  rgb_sensor_1.enablePowerAndRGBC();
  rgb_sensor_1.setIntegrationTime(10);
  // Variable initiation
  float threshold[4] = {1250.0, 3.0, 3.0, 2.5};//1250.0, 3.0, 3.0, 2.5
  int rgb_readings_0[4];
  int rgb_readings_1[4];
  float line_constant;
  // Right Sensor

  // Main Loop
  while (1){ 
    rgb_sensor_0.getAllColors(rgb_readings_0);
    printf("clear: %d, red: %d, green: %d, blue: %d\n", rgb_readings_0[0],
           rgb_readings_0[1], rgb_readings_0[2], rgb_readings_0[3]);
           rgb_sensor_0.getAllColors(rgb_readings_0);
    rgb_sensor_0.getAllColors(rgb_readings_1);
    printf("clear: %d, red: %d, green: %d, blue: %d\n", rgb_readings_0[0],
           rgb_readings_1[1], rgb_readings_1[2], rgb_readings_1[3]);



//Clear values Left-Right comparison
    //line_constant will be a value between 0.0-1.0, where [0.0 -> Hard LEFT] & [1.0 -> Hard RIGHT]
    //real values for this will vary between about 0.25-0.75 from testing
   // on_line_R = (rgb_readings_0[0]/(rgb_readings_0[0]+rgb_readings_1[0]));
    //on_line_L = (rgb_readings_1[0]/(rgb_readings_0[0]+rgb_readings_1[0]));
    if (rgb_readings_0[0] > 1300 && rgb_readings_1[0] < 1500 ) {
        control_R(0.1,0);
        control_L(0.2,1);
        wait_us(1000);
    }
    if (rgb_readings_0[0] < 1300 && rgb_readings_1[0] > 1500 ) {
      control_R(0.2,1);
      control_L(0.1,0);
      wait_us(1000);
    }
    if (rgb_readings_0[0] > 1300 && rgb_readings_1[0] > 1500 ) {
      control_R(0.1,1);
      control_L(0.1,1);
      wait_us(1000);
    }
 }
}
