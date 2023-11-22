#include "mbed.h"


// Define L298N motor control pins
PwmOut PwmR(PTA4);//control right 
DigitalOut Rforward(PTD4);  // Adjust these pin assignments as per your connection
DigitalOut Rbackward(PTA12);

PwmOut PwmL(PTA2);//control left
DigitalOut Lforward(PTC8);
DigitalOut Lbackward(PTA5);

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



int main() {
    while(1){

        control_R(0.2,1);
        control_L(0.2,1);
        wait_us(1000000);//Start speed 0.1

        stopMotor();
        wait_us(1000000);//Wait

        control_R(1,1);
        control_L(1,1);
        wait_us(1000000);//Speed at 1 

        stopMotor();
         wait_us(1000000);//Wait

        control_R(0.2,0);
        control_L(0.2,0);
        wait_us(1000000);//Backward speed 0.1

        stopMotor();
        wait_us(1000000);//Wait

        control_R(1,0);
        control_L(1,0);
        wait_us(1000000);//Backward speed 1

        stopMotor();
        wait_us(1000000);//Wait

        control_R(0.2,0);
        control_L(1,1);
        wait_us(5000000);//Turn left

        stopMotor();
        wait_us(1000000);//Wait

        control_R(1,1);
        control_L(0.2,0);
        wait_us(5000000);//Turn right

         wait_us(1000000);//Wait

        control_R(0.5,1);
        control_L(0.5,0);
        wait_us(1000000);//Turning 

        stopMotor();
    }


}
        if (rgb_readings[3] > rgb_readings[1] && rgb_readings[3] > rgb_readings[2]) {
            printf("Blue is highest\n");
        }
        wait_us(100000);
    }
}
