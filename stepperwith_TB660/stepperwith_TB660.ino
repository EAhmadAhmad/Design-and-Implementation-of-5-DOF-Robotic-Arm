#define inputCLK 8
#define inputDT 7
int counter=0;
int currentStateCLK;
int previousStateCLK;

String encoder="";


void setup() {
  // put your setup code here, to run once:
pinMode(inputCLK,INPUT);
pinMode (inputDT,INPUT);
Serial.begin(9600);
pinMode(2,OUTPUT);
pinMode(4,OUTPUT);
digitalWrite(4,HIGH);
previousStateCLK=digitalRead(inputCLK);
}

void loop() {
  // put your main code here, to run repeatedly:
currentStateCLK=digitalRead(inputCLK);
digitalWrite(2,LOW);
digitalWrite(2,HIGH);
delayMicroseconds(200);
if (currentStateCLK!=previousStateCLK){
  if (digitalRead(inputDT) != currentStateCLK ){
    counter --;
    encoder="CCW";
    }
    else {
    counter ++;
    encoder="CW";
    }
    Serial.print("Direction");
    Serial.print(encoder);
    Serial.print ("-- value ::");
    Serial.println (counter);
    
  }
  previousStateCLK=currentStateCLK;
}
