#include <AccelStepper.h>
#include <Servo.h>
#define wristpitch 5
//#define wristroll 6
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
Servo gribber;


//--------------------------------------------------------

int th1;
int th2;
int th3;
int th4;
int th5;
int gri;
int a[7];

int TH1, TH2, TH3, TH4 , TH5, GRI, STEPS1, STEPS2, STEPS3;
int steps1 = 0;
int steps2 = 0;
int steps3 = 0;

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


//--------------------------------------------------------

// Create a new instance of the AccelStepper class:
AccelStepper base = AccelStepper(motorInterfaceType, basestep, basedir);
AccelStepper shoulder = AccelStepper(motorInterfaceType, shoulderstep, shoulderdir);
AccelStepper elbow = AccelStepper(motorInterfaceType, elbowstep, elbowdir);



void setup() {

  Serial.begin(115200);

  pinMode(baseLimit_1, INPUT_PULLUP);
  pinMode(baseLimit_2, INPUT_PULLUP);
  pinMode(baseLimit_3, INPUT_PULLUP);
  pinMode(baseLimit_4, INPUT_PULLUP);

  wrist_pitch.attach(wristpitch);
  gribber.attach(griBBer);

  // Set the maximum speed and acceleration:
  base.setMaxSpeed(2000);
  //base.setSpeed(1000);
  base.setAcceleration(1000);

  shoulder.setMaxSpeed(2000);
  //shoulder.setSpeed(1000);
  shoulder.setAcceleration(1000);

  elbow.setMaxSpeed(2000);
  //elbow.setSpeed(1000);
  elbow.setAcceleration(1000);

  wrist_pitch.write(110);
  gribber.write(0);

}

int Home = 0;
void loop() {

  while (Serial.available() > 0) {
    if (Serial.read() != 'w')
    {
      Serial.flush();
      break;
    }

    for (int i = 0; i < 7 ; i++) {
      a[i] = Serial.parseInt();
    }
   
    th1 = a[0];
    th2 = a[1];
    th3 = a[2];
    TH4 = a[3];
    TH5 = a[4];
    GRI = a[5];
    zeroo = a[6];

    STEPS1 =  (th1 * 6400.0 / 360.0);
    STEPS2 =  (th2 * 6400.0 / 360.0) * 4.5;
    STEPS3 =  (th3 * 6400.0 / 360.0) * 4.5;

    Home = 1 - zeroo;
    if (Home == 1)
      standby();
    else/// zero
    {
      th4 = TH4;
      gri = GRI;
      zeroo = a[6];

      steps1 = STEPS1;
      steps2 = STEPS2;
      steps3 = STEPS3;
    }

  }     ////-------------------loop---------------------

  // Serial.print(zeroo);
  if (zeroo == 1) {
    limit_1_state = digitalRead(baseLimit_1);
    limit_2_state = digitalRead(baseLimit_2);
    limit_3_state = digitalRead(baseLimit_3);
    limit_4_state = digitalRead(baseLimit_4);

    zero();
  }

  wrist_pitch.write(th4);
  gribber.write(gri);

  base.moveTo(steps1 );//1->-50/->+340/->+180
  base.run();

  shoulder.moveTo(steps2 );//-550/->-50
  shoulder.run();

  elbow.moveTo(steps3);
  elbow.run();

  if (base.distanceToGo() == 0 && shoulder.distanceToGo() == 0 && elbow.distanceToGo() == 0)
  {
    if (Home == 1) {
      delay(100);
      targetInv();
      Home = 2;
    }
    else if (Home == 2) {
      delay(100);
      targetA();
      Home = 3;
    } else if (Home == 3) {
      delay(100);
      targetB();
      Home = 4;
    } else if (Home == 4) {
      delay(50);
      targetC();
      Home = 0;
    }
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
    shoulder.stop();
    elbow.stop();
    base.stop();
  }
}

void standby()
{
  th4 = 110;
  gri = 0;

  steps1 =  (90 * 6400.0 / 360.0);
  steps2 =  (100 * 6400.0 / 360.0) * 4.5;
  steps3 =  (-10 * 6400.0 / 360.0) * 4.5;
}
void targetInv()
{
  th4 = 110;
  gri = GRI;

  steps1 = STEPS1;
  steps2 = STEPS2-300;
  steps3 = STEPS3;
}

void targetA() {

  th4 = 110;
  gri = 125;

  steps1 = 0;
  steps2 = (45 * 6400.0 / 360.0) * 4.5;
  steps3 = (0 * 6400.0 / 360.0) * 4.5;
}

void targetB() {

  th4 = 100;
  gri = 125;

  steps3 = (-35 * 6400.0 / 360.0) * 4.5;
}

void targetC() {

  th4 = 100;
  gri = 0;

}

/*void belt() {
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  
  while(distance > 10) {
    
  if (distance > 10) {
    for (int i = 0; i < 30; i++) {
      digitalWrite(convstep, LOW);
      delayMicroseconds(100);
      digitalWrite(convstep, HIGH);
      delayMicroseconds(100);
    }
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  }

}*/
