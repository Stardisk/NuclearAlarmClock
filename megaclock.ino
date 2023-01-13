#include "sevenSegment.h";

sevenSegment mainIndicator;
byte hour, minute, second;
byte currentMode; // 0 - часы, 1 - прием данных



void setup(){
  Serial.begin(9600);
  delay(100);         

  mainIndicator.setDot(0, 1);
}

void loop(){  
  byte getInput = mainIndicator.waitForInput();
  if(getInput){
    currentMode = (getInput == 1) ? 1:0;
  }
  
  clock();

  
  



  mainIndicator.dynamicIndication();
}


void clock(){
  static uint32_t clockTimer;
  int difference = millis() - clockTimer;
  static int collectedDifference;

  if(difference > 999){    
/*    Serial.println(millis());
    Serial.println(difference);
    Serial.println("--------");*/
    
    clockTimer = millis();
    second++;    

    collectedDifference += (difference - 1000);
    if(collectedDifference > 1249){
      second++;
      collectedDifference = 0;
    }

    if(second > 59){ minute++; second = 0;}
    if(minute > 59){ hour++; minute = 0;}
    if(hour > 23){ hour = 0;}   
    String formattedTime; formattedTime.reserve(4);
    formattedTime  += addTimeZero(minute); formattedTime += addTimeZero(second);    
    if(currentMode == 0){
      mainIndicator.sendData(formattedTime);      
    }
  }
}
String addTimeZero(byte number){
  String convNumber; convNumber.reserve(2);
  convNumber = String(number);
  if(number < 10){ convNumber = 0 + convNumber;}
  return convNumber;
};


