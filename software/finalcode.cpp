#include "mbed.h"
#include "TCS3472_I2C.h"
#include <cstring>
// Line Sensors
InterruptIn R3(PTA13);
DigitalIn R2(PTD5);
DigitalIn R1(PTD0);
DigitalIn L1(PTD2);
DigitalIn L2(PTD3);
InterruptIn L3(PTA17);

// Motors    
    // Right
PwmOut PwmR(PTA12);
DigitalOut Rforward(PTA2);
DigitalOut Rbackward(PTD4);
    // Left
PwmOut PwmL(PTA1);
DigitalOut Lforward(PTA5);
DigitalOut Lbackward(PTA4);

// Interrupt LED (For testing)
// DigitalOut LED(PTD1);

// Colour Sensors
TCS3472_I2C rgb_sensor_L(PTE0, PTE1);
TCS3472_I2C rgb_sensor_R(PTC9, PTC8);

// Electromagnet
DigitalOut Electromagnet(PTE31);

// Bluetooth
UnbufferedSerial bluetooth(PTE22,PTE23);

void controlLeft(float dutycycle) {
    if (dutycycle > 0){
        Lforward = 1;
        Lbackward =0;
    }
    else{
        Lforward = 0;
        Lbackward = 1;
        dutycycle = dutycycle*-1.0;
    }
    if (PwmL != dutycycle){
        PwmL = dutycycle;
   }
}
    
void controlRight(float dutycycle) {
    if (dutycycle > 0){
        Rforward = 1;
        Rbackward =0;
    }
    else{
        Rforward = 0;
        Rbackward = 1;
        dutycycle = dutycycle*-1.0;
        }
    if (PwmR != dutycycle){
        PwmR = dutycycle;
    }
}    

void goForward (float speed){
    controlLeft(speed);
    controlRight(speed);
    }

void turnLeft (float speed, int time){
    controlLeft(-speed);
    controlRight(speed);
    wait_us(time);
    }

void turnRight(float speed, int time){
    controlLeft(speed);
    controlRight(-speed);
    wait_us(time);
    }

void interruptLeft(){
    float speed = 0.7;
    // LED = 1;
    if (!L2 == 0){
    controlLeft(-speed);
    controlRight(-speed);
    wait_us(50000);
    controlLeft(-speed);
    controlRight(speed);
    wait_us(440000);}

    else if (!L2 == 1){
    controlLeft(-speed);
    controlRight(-speed);
    wait_us(50000);
    controlLeft(-speed);
    controlRight(speed);
    wait_us(550000);}
    }

void interruptRight(){
    float speed = 0.7;
    if (!R2 == 0){
    controlLeft(-speed);
    controlRight(-speed);
    wait_us(50000);
    controlLeft(speed);
    controlRight(-speed);
    wait_us(440000);}

    else if (!R2 == 1){
    controlLeft(-speed);
    controlRight(-speed);
    wait_us(50000);
    controlLeft(speed);
    controlRight(-speed);
    wait_us(550000);}
    }

void stop(){
    PwmR = 0;
    PwmL = 0;
    }

void line(){
    if((!R2 == false)&&(!L2 == false)){goForward(0.16);};

    if((!R2 == true)&&(!L2 == false)){turnRight(0.7,100000);} //if Right Sensor is Black and Left Sensor is White then it will call turn Right function  

    if((!R2 == false)&&(!L2 == true)){turnLeft(0.7,100000);}  //if Right Sensor is White and Left Sensor is Black then it will call turn Left function

    if((!R2 == true)&&(!L2 == true)){goForward(0.16);};   //if Right Sensor and Left Sensor are at White color then it will call forword function
}

void sensorsInitialise(int int_time) {
    rgb_sensor_R.enablePowerAndRGBC();
    rgb_sensor_L.enablePowerAndRGBC();
    rgb_sensor_R.setIntegrationTime(int_time);
    rgb_sensor_L.setIntegrationTime(int_time);
}

void SigCheck();//BT functions to callback
void BTcontrol();

// Bluetooth Functions
char c [8] ={""};// 1 byte buffer

void SigCheck(){
    if (bluetooth.readable()){
        bluetooth.read(&c,8);
            BTcontrol();
    }
}

void BTcontrol(){
    bool exit = 0;
    while(exit == 0){
        if (bluetooth.readable()) {
            bluetooth.read(&c,8);  // Read serial

            if (strcmp("w",c)==0){    // Forward half speed
                controlRight(0.18);
                controlLeft(0.18);
            }
            if ( strcmp("s",c) == 0 ){   // Reverse half speed
                controlRight(-0.18);
                controlLeft(-0.18);
            }
            if (strcmp ("a",c) == 0 ){  // Start turning left
                controlRight(0.4);
                controlLeft(-0.4);
            }
            if (strcmp("d",c)==0){  // Start turning right
                controlRight(-0.4);
                controlLeft(0.4);
            }
            if (strcmp("b",c)==0){
                controlRight(0);
                controlLeft(0);
            }
            if (strcmp("p",c)==0){
                Electromagnet = 0;
            }
            if (strcmp("o",c)==0){
                Electromagnet = 1;
            }
            if (strcmp("e",c)==0){
                exit = 1;
            }
        }
    }
}

int main(){
    // Attach bluetooth interrupts
    bluetooth.attach(&SigCheck, UnbufferedSerial::RxIrq);
    // Initialises PWM period for EM
    Electromagnet = 1;
    // Attatch interrupts for outboard line sensors
    R3.fall(&interruptRight);
    L3.fall(&interruptLeft);
    // Variable initiation for colour sensors
    float thresholds_R[3] = {0.950, 0.0, 1.177};
    float thresholds_L[3] = {1.200, 0.0, 1.110};
    int rgb_readings_R[4];
    int rgb_readings_L[4];
        // Right Sensor
    float red_ratio_R;
    float green_ratio_R;
    float blue_ratio_R;
        // Left Sensor
    float red_ratio_L;
    float green_ratio_L;
    float blue_ratio_L;

    // Initialising the sensor values - sent through I2C to sensor peripheral. Integration time sent as an argument to the function
        sensorsInitialise(20);

        wait_us(1000000);
    // Disk logic
    bool disk = 0;
    char disk_colour = 'N';
    
    while(1){
        if (disk == 0){
            disk_colour = 'N';
        }
        if (disk == 0||disk_colour == 'N'){
            Electromagnet = 1;  
        }
        else if (disk == 1||disk_colour != 'N'){
            Electromagnet = 0;
        }
        // Line following
        line();
    // Collect RGB readings from sensors
        rgb_sensor_R.getAllColors(rgb_readings_R);
        float float_rgb_readings_R[4] = {static_cast<float>(rgb_readings_R[0]),static_cast<float>(rgb_readings_R[1]),static_cast<float>(rgb_readings_R[2]),static_cast<float>(rgb_readings_R[3])};
        rgb_sensor_L.getAllColors(rgb_readings_L);
        float float_rgb_readings_L[4] = {static_cast<float>(rgb_readings_L[0]),static_cast<float>(rgb_readings_L[1]),static_cast<float>(rgb_readings_L[2]),static_cast<float>(rgb_readings_L[3])};
    
    // // Ratios of colours divided by other two colour values values
    //     // Right Sensor
        red_ratio_R = 2*float_rgb_readings_R[1] / ((float_rgb_readings_R[2] + float_rgb_readings_R[3]));
        green_ratio_R = 2*float_rgb_readings_R[2] / ((float_rgb_readings_R[1] + float_rgb_readings_R[3]));
        blue_ratio_R = 2*float_rgb_readings_R[3] / ((float_rgb_readings_R[1] + float_rgb_readings_R[2]));
        // Left Sensor
        red_ratio_L = 2*float_rgb_readings_L[1] / ((float_rgb_readings_L[2] + float_rgb_readings_L[3]));
        green_ratio_L = 2*float_rgb_readings_L[2] / ((float_rgb_readings_L[1] + float_rgb_readings_L[3]));
        blue_ratio_L = 2*float_rgb_readings_L[3] / ((float_rgb_readings_L[1] + float_rgb_readings_L[2]));
    // int RedRatL = static_cast<int>(red_ratio_L*10000);
    // int RedRatR = static_cast<int>(red_ratio_L*10000);
    // printf('R= %d L= %d', RedRatR, RedRatL);

    if ((red_ratio_L < thresholds_L[0])&&(red_ratio_R >= thresholds_R[0])){
        disk = 1;
        disk_colour='R';
        Electromagnet = 0;

    }
    if((blue_ratio_L < thresholds_L[2])&&(blue_ratio_R >= thresholds_R[2])){
        disk = 1;
        disk_colour='B';
        Electromagnet = 0;

    }

    if ((disk == true)&&(disk_colour=='R')&&(red_ratio_L >= thresholds_L[0])&&(red_ratio_R >= thresholds_R[0])){
        disk = 0;
        disk_colour='N';
        Electromagnet = 1;

    }
    else if((disk == true)&&(disk_colour=='B')&&(blue_ratio_L >= thresholds_L[2])&&(blue_ratio_R >= thresholds_R[2])){
        disk = 0;
        disk_colour='N';
        Electromagnet = 1;
    }
        wait_us(1000);
    }
    
}
