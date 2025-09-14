#include <AccelStepper.h>
#include <Servo.h>
#define wristpitch 5
#define wristroll 7
#define griBBer 6
//--------------------------------------------------------
// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:

#define basedir 52            //------base motor direction pin
#define basestep 50           //------base motor step pin

#define shoulderdir 51        //------shoulder motor direction pin
#define shoulderstep 49       //------shoulder motor direction pin

#define elbowdir 41              //------arm motor direction pin
#define elbowstep 43             //------arm motor step pin

#define motorInterfaceType 1

//--------------------------------------------------------
/////limit switch variables and pin alias
#define baseLimit_1 23//23
#define baseLimit_2 25//25
#define baseLimit_3 31
#define baseLimit_4 33

bool limit_1_state;
bool limit_2_state;
bool limit_3_state;
bool limit_4_state;
//--------------------------------------------------------
Servo wrist_pitch;
Servo wrist_roll;
Servo gribber;


//--------------------------------------------------------

int th1;
int th2;
int th3;
int th4;
int th5;
int gri;
int a[7];

int steps1 = (166 * 6400.0 / 360.0);
int steps2 = (116 * 6400.0 / 360.0) * 4.5;
int steps3 = (15 * 6400.0 / 360.0) * 4.5;

int zero_th1 = 166;//167
int zero_th2 = 116;//116
int zero_th3 = 15;

int zsteps1 = (zero_th1 * 6400.0 / 360.0);
int zsteps2 = (zero_th2 * 6400.0 / 360.0) * 4.5;
int zsteps3 = (zero_th3 * 6400.0 / 360.0) * 4.5;
bool zeroo;


bool bend = 0;
bool shend = 0;
bool elend = 0;


bool end1;
bool end2;

bool zerodone = false;

bool done = false;
int x;

//--------------------------------------------------------

// Create a new instance of the AccelStepper class:
AccelStepper base = AccelStepper(motorInterfaceType, basestep, basedir);
AccelStepper shoulder = AccelStepper(motorInterfaceType, shoulderstep, shoulderdir);
AccelStepper elbow = AccelStepper(motorInterfaceType, elbowstep, elbowdir);

void messageMTLB() {
  while (!zerodone) {
    
    limit_1_state = digitalRead(baseLimit_1);
    limit_2_state = digitalRead(baseLimit_2);
    limit_3_state = digitalRead(baseLimit_3);
    limit_4_state = digitalRead(baseLimit_4);

    zero();

  }

  if (zerodone) {
    while(Serial.available() > 0){
    if(Serial.read() != 'w')
    {
      Serial.flush();
      break;
    }
    
    for (int i = 0; i<7 ; i++){
      a[i] = Serial.parseInt();
    }
    
    th1 = a[0];
    th2 = a[1];
    th3 = a[2];
    th4 = a[3];
    th5 = a[4];
    gri = a[5];
    zeroo = a[6];
    
    steps1 =  (th1 * 6400.0 / 360.0);
    steps2 =  (th2 * 6400.0 / 360.0) * 4.5;
    steps3 =  (th3 * 6400.0 / 360.0) * 4.5;

    wrist_pitch.write(th4);
    wrist_roll.write(th5);
    gribber.write(gri);
    //Serial.println(limit_3_state);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(baseLimit_1, INPUT_PULLUP);
  pinMode(baseLimit_2, INPUT_PULLUP);
  pinMode(baseLimit_3, INPUT_PULLUP);
  pinMode(baseLimit_4, INPUT_PULLUP);

  wrist_pitch.attach(wristpitch);
  wrist_roll.attach(wristroll);
  gribber.attach(griBBer);


  // Set the maximum speed and acceleration:
  base.setMaxSpeed(1000);
  base.setAcceleration(500);

  shoulder.setMaxSpeed(1000);
  shoulder.setAcceleration(500);

  elbow.setMaxSpeed(1000);
  elbow.setAcceleration(500);

  wrist_pitch.write(110);
  wrist_roll.write(0);
  gribber.write(0);
}


void loop() {

  messageMTLB();

  if (zerodone) {

  
    wrist_pitch.write(th4);
    wrist_roll.write(th5);
    gribber.write(gri);
    
    base.moveTo(steps1 +100);
    base.run();

    shoulder.moveTo(steps2 );
    shoulder.run();

    elbow.moveTo(steps3 );
    elbow.run();

    /*if (!(base.isRunning())){
      target();
    }*/
    
  }
}


void zero()
{
  shend = 0;
  elend = 0;
  bend = 0;
  if (limit_3_state == 0) {

    elbow.setMaxSpeed(2000);
    elbow.setSpeed(1000);
    shoulder.setMaxSpeed(2000);
    shoulder.setSpeed(1000);

    shoulder.runSpeed();
    elbow.runSpeed();
  }
  if (limit_3_state == 1) {
    shoulder.setCurrentPosition(zsteps2);
    shend = 1;

  }
  //----------------------------------------------
  if (limit_4_state == 1 && shend == 1) {
    elbow.setMaxSpeed(-2000);
    elbow.setSpeed(-1000);
    elbow.runSpeed();
  }
  if (limit_4_state == 0) {
    elbow.setCurrentPosition(zsteps3);
    elend = 1;
  }
  //----------------------------------------------
  if (limit_2_state == 0 && shend == 1) {
    base.setMaxSpeed(2000);
    base.setSpeed(1000);
    base.runSpeed();
  }
  if (limit_2_state == 1) {
    base.setCurrentPosition(zsteps1);
    bend = 1;
  }
  if (shend == 1 && elend == 1 && bend == 1) {
    zeroo = 0;
    zerodone = true;
    shoulder.stop();
    elbow.stop();
    base.stop();
  }
}

void target(){
  
    wrist_pitch.write(30);
    wrist_roll.write(0);
    gribber.write(90);
  
    base.moveTo(0);
    base.run();

    shoulder.moveTo(steps2 );
    shoulder.run();

    elbow.moveTo(533);
    elbow.run();

    if (!base.isRunning()){
      target1();
    }
}

void target1(){
  
    wrist_pitch.write(30);
    wrist_roll.write(0);
    gribber.write(0);

    elbow.moveTo(177);
    elbow.run();
}
//----------------------------------------
//limit_2_state == 1
//limit_3_state == 0
//limit_4_state == 0
