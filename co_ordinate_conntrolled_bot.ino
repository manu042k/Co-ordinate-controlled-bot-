//  Author:Manoj M 

#include <LiquidCrystal.h>                                                  //lcd libaray 
LiquidCrystal lcd(31,6,33,35,37,39);

#define encoderA1 20                                                        //encoder pins   
#define encoderB1 21

#define encoderA2 18
#define encoderB2 19

#define ena 11                                                              //L298n pins motor1 
#define in1 12
#define in2 13

#define enb 9                                                               //L298n pins motor1 
#define in3 8
#define in4 7

#define trig_C 25                                                           //hcsr05 pins 
#define echo_C 24

int  count1 =0 ;                                                            //counts encountered
int  count2 =0;

int x=50,y=50;                                                              //co-ordinate


void setup() 
{
    
  pinMode(encoderA1,INPUT_PULLUP);                                          //defining encoder (inputs values only whn its tiggered)
  pinMode(encoderB1,INPUT_PULLUP);

  pinMode(encoderA2,INPUT_PULLUP);
  pinMode(encoderB2,INPUT_PULLUP);

  attachInterrupt( digitalPinToInterrupt(20) ,encA1 ,RISING);               //executes the function when clock pulse rises   
  attachInterrupt( digitalPinToInterrupt(21) ,encB1,RISING);
  
  attachInterrupt( digitalPinToInterrupt(19) ,encA2 ,RISING);
  attachInterrupt( digitalPinToInterrupt(18) ,encB2,RISING);
 
  pinMode(ena,OUTPUT);                                                     //L298n pins
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);

  pinMode(enb,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);

  pinMode(trig_C, OUTPUT);                                                  //hcsr05 pins
  pinMode(echo_C, INPUT);

 analogWrite(5,0);                                                          //lcd contarast
 lcd.begin(16,2);                                                           //type of lcd 
  
 Serial.begin(9600);                                                        //communition speed in hz
 
 lcd.setCursor(0,0);                                                        //set cursor to (0,0) 
 lcd.print("falcon online ");                                               //lcd display  

 lcd.setCursor(0,1);
 lcd.print("x=");
 lcd.print(x);
 lcd.print(",y=");
 lcd.print(y);
 
 exe();                                                                     //executing the function

 lcd.clear();                                                               //printing at the last
 lcd.print("obj complete ");
 
 
}
  void loop()
  {

                   //no function inside the loop as we need to execute it once    
  }
  
 void exe()
 {
    double estcountt=0;                       //estimate turning count 
    
   estcountt= estturn(x,y);
   int p= estdiagonal(x,y);                    //estimate diagonal distance

    Serial.println("estcount---->\t");
  Serial.print(estcountt);
     Serial.println("\t");

   
     brake();
     delay(4000);                             //waits for 4 sec before executing the function
  
  if(dis(trig_C,echo_C)>x)                      //checks in x co orinate for obstacles 
    { 
       lcd.clear();
       lcd.print("path 1 ");                   //if no executes path1
       Serial.println("path 1");
   
       stmove(x);                              //moves x distance   
      
       brake();                                //waits for 1 sec
     delay(1000);
    
    funturn(300/3);                            //turns 90 deg
         
     brake();                                   //waits for 1 sec
     delay(1000);
      
       stmove(y);                              //moves y distance 
       brake();
      
    }
    else 
    {   
       
      
       
        funturn(estcountt/2);                  //turns diagonal angle 
        brake();
        delay(1000);
                
        if(dis(trig_C,echo_C)>estdiagonal(x,y))  //checks for obstacle 
        {  
          lcd.clear();                                        //if no executes path2
          lcd.print("path 2 ");
          Serial.println("path 2");
         
          stmove(p);                                           //moves diagonal distance 
           brake();
        }
        else
        { 
           lcd.clear();                                      //executes path 3
           lcd.print("path 3");
           Serial.println("path 3");
   
          int ang,reqangle;
          
          ang=estangle(x,y);                                  //diaganol angle
          reqangle=1.57-ang;                                  //extra angle
            
          funturn(reqangle*260/(1.377*3));                    //turn extra angle
            
          brake();                                            
          delay(1000);                                         //waits for 1 sec
          
          stmove(y);                                           //moves y distance
          brake();
          delay(1000);
          
          funturnl(300/3);                                      //turns 90 deg right
          
          stmove(x);                                             //moves x distance
          brake();
          
          
          
        }
     }
 }

  void stmove(int distance)
 {

   int estcount;                                             

   int power1=200;                                              //initinal power 
   int power2=200;

    int diff1,diff2;                                           //difference 

    int pevcount1,pevcount2;                                //pervious counts
    int presentc1,presentc2;                                 //present counts

    //perdertermination of counts
     
  estcount=(distance*10)/1.377;

    count1=0;
    count2=0;

   pevcount1=count1;
   pevcount2=count2;

   while(count1<estcount+600&&count2<estcount+600)  //turns untill counts turn to estimated count   600 error compensating  dependps on surface
   {
     executemotion(power1,power2);
    presentc1=count1;
    presentc2=count2;

    diff1=presentc1-pevcount1;
    diff2=presentc2-pevcount2;
    
    pevcount1=presentc1;
    pevcount1=presentc1;

    
     Serial.println("count1---->\t");
    Serial.print(count1);
    Serial.println("\t");
    Serial.println("count2---->\t");
    Serial.print(count2);
     Serial.println("\t");

    
    if(diff1>diff2)
    {
      power2=power2+11;
      power1=power1-11;
      
      
    }
   if(diff1<diff2)
    {
      power1=power1+11;
       
    }
   
    delay(20);
   }
   
   
 }


void  funturn(int radiancounts)                   //turning funtion left
{    
    count1=0;
    count2=0;
    Serial.println("count1---->\t");
    Serial.print(count1);
    Serial.println("\t");
    Serial.println("count2---->\t");
    Serial.print(count2);
     Serial.println("\t");
   while( count1<=radiancounts && count2<= radiancounts)
          {
    
                turn_R(); 
          }
}

void  funturnl(int radiancounts)                 //turning function right 
{    
    count1=0;
    count2=0;
    Serial.println("count1---->\t");
    Serial.print(count1);
    Serial.println("\t");
    Serial.println("count2---->\t");
    Serial.print(count2);
     Serial.println("\t");
   while( count1<=radiancounts && count2<= radiancounts)
          {
    
                turn_L(); 
          }
}       
 void executemotion(int powerA,int powerB)        //moves forward
 {
    powerA=constrain(powerA,0,255);
    powerB=constrain(powerB,0,255);
   digitalWrite(in1,HIGH);
   digitalWrite(in2,LOW);
     
   digitalWrite(in3,HIGH);
   digitalWrite(in4,LOW);

   analogWrite(ena,powerA);
   analogWrite(enb,powerB);
 }
   
 
 void encA1()                                  //encoder channel A of M1 
 {
   if(digitalRead(encoderA1)==HIGH)
     {
      if(digitalRead(encoderB1)==LOW)
          count1++;
       else 
         count1--;
     }
     else 
     {
      if(digitalRead(encoderB1)==HIGH)
          count1++;
       else 
         count1--;
     } 
 }

          void encB1()                                //encoder channel b of M1 
 {
   if(digitalRead(encoderB1)==HIGH)
     {
      if(digitalRead(encoderA1)==HIGH)
          count1++;
       else 
         count1--;
     }
     else 
     {
      if(digitalRead(encoderA1)==LOW)
          count1++;
       else 
         count1--;
     } 
 }


 void encA2()                                      //encoder channel A of M2 
 {
   if(digitalRead(encoderA2)==HIGH)
     {
      if(digitalRead(encoderB2)==LOW)
          count2++;
       else 
         count2--;
     }
     else 
     {
      if(digitalRead(encoderB2)==HIGH)
          count2++;
       else 
         count2--;
     } 
 }

 
        void encB2()                                      //encoder channel 2 of M2 
 {
   if(digitalRead(encoderB2)==HIGH)
     {
      if(digitalRead(encoderA2)==HIGH)
          count2++;
       else 
         count2--;
     }
     else 
     {
      if(digitalRead(encoderA2)==LOW)
          count2++;
       else 
         count2--;
     } 
 }

     void brake ()                                     //brake function
     {
       digitalWrite(in1,LOW);
       digitalWrite(in2,LOW);
       analogWrite(ena,0);
       digitalWrite(in3,LOW);
       digitalWrite(in4,LOW);
       analogWrite(enb,0);
     }
 
  void turn_R()                                      //turn right 
 {
        analogWrite(ena,150);
       digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
   
        analogWrite(enb,150);
       digitalWrite(in3,LOW);
          digitalWrite(in4,HIGH);
          
 }

  void turn_L()                                      //turns left
 {
        analogWrite(ena,150);
       digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
   
        analogWrite(enb,150);
       digitalWrite(in3,HIGH);
          digitalWrite(in4,LOW);
 }
 
 double estturn(int x,int y)                         //estimating turning counts 
 {
  double angle=0,arcln=0,estcount=0;
  angle= atan(y/x);
  arcln=angle*260;
  estcount=arcln/1.377;
  return  estcount;
 } 

double estangle(int x,int y)                          //estimating turning angle in radians
{
   double angle=0,arcln=0;
  angle= atan(y/x);
  return angle;
}


 int estdiagonal(int x,int y)                         //estimating diagonal distance   
 {
  int dia=0;
  dia=sqrt(x*x+y*y);
  
  return dia;
 }


 
 long dis(int trig,int echo)                          //hcsr05 obstacle dertimation
{
   
  long distance=0,duration=0;
   int trigPin,echoPin;
   trigPin=trig;
   echoPin=echo;
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);

  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*.034/2;
  Serial.print("distance:  ");
  Serial.println(distance);
  return distance;
}
