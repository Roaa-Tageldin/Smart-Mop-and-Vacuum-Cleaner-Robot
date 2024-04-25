#include <ESP32Servo.h>
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
#define in1 26  //Right motor pole 1
#define in2 27  //Right motor pole 2
#define in3 14  //left motor pole 1
#define in4 12 //left motor pole 1
#define ena 25 //Right motor speed
#define enb 13 //Right motor speed
#define relay 16 //relay pin for the pump
#define fanin1 2 
#define fanin2 18 
#define fanin3 5 
#define fanin4 4
Servo myservo;  // create servo object to control a servo
Servo servo ;   // create servo object to control tthe arm  servo
int pos = 0;    // variable to store the servo position
const int trigPin = 32;//Trigger Pin of Ultrasonic
const int echoPin = 35;//Echo Pin of Ultrasonic
long duration;//used by ultrasonic Function
int distance,Rdistance,Ldistance;//distance=center Distance Rdistance=Right Distance Ldistance=Left Distance
char c='S'; 
char state = '0'; // start with on state
char m = 'Q'; // use it for pump don't use c or state pleaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaase 
// note : the start state for pump is Off 

int downVal = 180;
int upVal = 0;

void setup() 
{
  SerialBT.begin("ESP32-Bluetooth");
  Serial.begin(9600);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(ena,OUTPUT);
  pinMode(enb,OUTPUT);
  pinMode(fanin1,OUTPUT);
  pinMode(fanin3,OUTPUT);
  pinMode(fanin4,OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  myservo.attach(33);  // attaches the servo on pin 33 to the servo object
  myservo.write(90);  // Initial Pos of Servo at center
  servo.attach(19);// attaches the servo on pin 19 to the servo object
  servo.write (0);
}

void loop()
{    
    if(SerialBT.available()){
        char in = SerialBT.read();
        if (in=='0' || in =='9')
            state = in;
        else if (state=='9' &&(in=='R'||in=='F'||in=='L'||in=='D'||in=='S'))
            c = in;
        else if (in=='Q'||in=='O')
            m = in;
    }
    pump();
    if(state=='9'){
        manual_move();
    }
    else if(state=='0'){
        obstacle_avoid();
    }  
}
void obstacle_avoid(void){
   arm_dc_motor_off();
   VaccumCleaner_ON();
   distance=ultrasonic();//take ultrasonic distance value   VaccumCleaner (); 
  if(distance>30)//if greater than 30 Cm
  {
    forward();//go to forward
  }else 
  {
    if(distance<=10&&distance>3)reverse();// if close to object reverse to get more space to change orientation
    off();                   //stop robot
    myservo.write(0);       //Turn servo Right 
    delay(300);
    Rdistance=ultrasonic();  //get ultrasonic value at right
    myservo.write(180);      // Turn servo Left
    delay(300);
    Ldistance=ultrasonic();  //get ultrasonic value at left
    myservo.write(90);       //return to center pos
    delay(300);
    comparison(Rdistance, Ldistance); //go to Comparasion function
    off();                    // stop after execute Comparison function action
    delay(150);
  }
  // reset all variables
  Rdistance=0;
  Ldistance=0;
  distance=0;
  }
  
void manual_move(void){
  arm_dc_motor_on();
  VaccumCleaner_OFF();
  if(SerialBT.available()){
        char in = SerialBT.read();
        if (in=='0' || in =='9')
            state = in;
        else if (state=='9' &&(in=='R'||in=='F'||in=='L'||in=='D'||in=='S'))
            c = in;
        else if (in=='Q'||in=='O')
            m = in;
  }
  if (c=='L')
  {
    Tleft90();
  }
  else if (c=='R')
  {
    Tright90();
  }
  else if (c=='F')
  {
    forward();
  }
  else if(c=='D')
  {
    reverse();
  }
  else if(c=='S'){
    off();
  }
  else if(c=='0'){
    c='F';
    state ='0';
  }
  delay(150);
}
int ultrasonic(void)//get distance captured by ultrasonic sensor
{
  
   digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
}

void comparison(int r, int l)//compare values of right and left Servo Pos 
{
  if(r>30||l>30)
  {
   if(r>l||r==l)
   {
     Tright90();
     Serial.println(distance);
     Serial.print("Right");
   }else if(l>r)
   {
     Tleft90();
     Serial.println(distance);
     Serial.print("Left");
   }
  }/*else if(r<25&&l<25)
  {
    Tleft180();
    Serial.println(distance);
     Serial.print("left8o");
  }*/
}
void forward()
{
  analogWrite(ena,200);
  analogWrite(enb,180);
  digitalWrite(in1,HIGH);//right side
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);//left side
  digitalWrite(in4,LOW);
}
void off()
{
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
}
void Tleft90()
{
  analogWrite(ena,200);
  analogWrite(enb,200);
  digitalWrite(in1,HIGH);//right side of robot forward move
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);//left side reverse 
  digitalWrite(in4,HIGH);
  delay(650);
}
void Tright90()
{
  analogWrite(ena,200);
  analogWrite(enb,200);
  digitalWrite(in1,LOW);//right side reverse
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);//left side forward
  digitalWrite(in4,LOW);
  delay(750);  //to continue to rotate to 90
}

void Tleft180()
{
  analogWrite(ena,200);
  analogWrite(enb,200);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  delay(1500);//controller stop but pins remain constant ,continue to rotate for 180
}
void reverse()
{
  analogWrite(ena,200);
  analogWrite(enb,180);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  delay(450);
}

void VaccumCleaner_ON()
{ 
  //analogWrite(enb2,250);
  digitalWrite(fanin3, HIGH);
  digitalWrite(fanin4, LOW);
  Serial.println("VaccumCleaner_ON  ");
}

void VaccumCleaner_OFF()
{ 
  //analogWrite(enb2,250);
  Serial.println("VaccumCleaner_OFF  ");
  digitalWrite(fanin3, LOW);

}


void pump()
{
  if(m=='O')
  {
    Serial.println("pump_On  ");

    digitalWrite(relay,HIGH);
  }
  else if(m=='Q')
   {
        Serial.println("pump_Off  ");

      digitalWrite(relay,LOW);
   }
}




void arm_dc_motor_on()
{ 
  servo.write (downVal);
  digitalWrite(fanin1, HIGH);
  digitalWrite(fanin2, LOW); 
    Serial.println("arm_dc_motor_on  ");

}

void arm_dc_motor_off()
{
  digitalWrite(fanin1, LOW);
  digitalWrite(fanin2, LOW);
  servo.write (upVal);
      Serial.println("arm_dc_motor_off  ");

}
