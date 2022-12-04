#include "vex.h"

using namespace vex;

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;
vex::controller c;

vex::motor m11(vex::PORT11, true);
vex::motor m12(vex::PORT12);
vex::motor m14(vex::PORT14, true);
vex::motor m15(vex::PORT15);
vex::motor_group leftMotors(m11, m12, m14, m15);
vex::motor m17(vex::PORT17);
vex::motor m18(vex::PORT18);
vex::motor m19(vex::PORT19, true);
vex::motor m20(vex::PORT20, true);
vex::motor_group rightMotors(m17, m18, m19, m20);

vex::motor intake1(vex::PORT13, false);
vex::motor intake2(vex::PORT16, true);
vex::motor_group intake(intake1, intake2);

vex::motor f1(vex::PORT1, false);
vex::motor f2(vex::PORT2, true);
vex::motor_group fly(f1, f2);

vex::triport::port& p = Brain.ThreeWirePort.G;
digital_out indexer = digital_out(p);

// define your global instances of motors and other devices here

int speed = 8;

void inc() {
    if (speed < 12) speed++;
}

void dec() {
    if (speed > 0) speed--;
}

int indexerTimer, indexerOffTimer;
bool indexerOn = false;
void on() {
    indexer.set(false);
    indexerOn = true;
    indexerTimer = vex::timer::system();
}

void off() {
    indexer.set(true);
    indexerOn = false;
    indexerOffTimer = vex::timer::system();
}

int main() {

    c.ButtonL1.pressed(inc);
    c.ButtonL2.pressed(dec);
    c.ButtonR2.pressed(on);
    c.ButtonR2.released(off);

    bool arcade = false;

    while(1) {

        if (arcade) {
            float drive = c.Axis3.position() / 100.0;
            float turn = c.Axis1.position() / 100.0;
            float max = fmax(1.0, fmax(fabs(drive+turn), fabs(drive-turn)));
            leftMotors.spin(forward,12 * (drive + turn) / max, vex::volt);
            rightMotors.spin(forward, 12 * (drive - turn) / max, vex::volt);

        } else {
            leftMotors.spin(vex::forward, c.Axis3.position() / 100.0 * 12, vex::volt);
            rightMotors.spin(vex::forward, c.Axis2.position() / 100.0 * 12, vex::volt);
        }
        

        if (indexerOn && vex::timer::system() - 250 > indexerTimer || (!indexerOn && vex::timer::system() - 300 < indexerOffTimer)) {
            intake.spin(vex::reverse, 12, vex::volt);
        } else if (c.ButtonR1.pressing()) {
            intake.spin(vex::forward, 12, vex::volt);
        } else {
            intake.stop();
        }

        fly.spin(forward, speed, vex::volt);
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}
