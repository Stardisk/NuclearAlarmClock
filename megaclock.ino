#include "sevenSegment.h";
#include "analogButton.h";
#include "tumbler.h";

sevenSegment mainIndicator;
tumbler tmblrAlarmEnabled(1);

byte hour, minute, second;
byte currentMode; 
// 0 - часы, 1 - показ минут и секунд,  201 - прием данных с ПК и вывод их на экран, 202 - тестер кнопок, 203 - тестер уровней кнопок

void setup(){
  Serial.begin(9600);
  delay(100);
  mainIndicator.setDot(2, 1);  
}

void waitForInput(){
  //ожидание ввода с ПК для немедленного показа этих данных на дисплее    
  if (Serial.available() > 0) {  //если есть доступные данные
    delay(50);   //подождем пока все данные дойдут
    String inputData;
    inputData.reserve(64);
    byte incomingByte = 0;
    while(Serial.available() > 0){
      incomingByte = Serial.read();
      if(incomingByte != 10) { inputData += char(incomingByte);}
    }    

    //вывод всех входящих данных на семисегментник
    if(inputData.substring(0, 5) == "/segm"){   currentMode = 201;}      

    if(inputData.substring(0, 5) == "/btna"){ currentMode = 203;} 

    if(inputData.substring(0, 4) == "/mic"){  currentMode = 204;} 

    if(inputData.substring(0, 4) == "/ref"){   
      byte rate = inputData.substring(5,7).toInt();
      Serial.print("changed refresh rate to ");
      Serial.println(rate);
      mainIndicator.setRefreshRate(rate);
    } 
    
    //управление точками семисегментника: /dot [номер точки] [0/1/2 == выкл/вкл/перекл]
    if(inputData.substring(0, 4) == "/dot"){
      byte dotNumber = inputData.substring(5,6).toInt();           
      mainIndicator.setDot(dotNumber, 2);
    }    

    //выход в режим часов
    if(inputData.substring(0, 5) == "/exit"){   currentMode = 0;} 

    if(inputData.substring(0, 5) == "/mode"){   mainIndicator.sendData(String(currentMode));}      

    if(currentMode == 201) { mainIndicator.sendData(inputData);}    
  }
}

#include "controlByButtons.h";
controlByButtons buttonControl;

void loop(){  
  waitForInput();    

  if(currentMode == 203){
    mainIndicator.sendData(String(analogRead(0)));
  }     

  if(tmblrAlarmEnabled.poll()){    
    mainIndicator.setDot(0, 1);
  }
  else{
    mainIndicator.setDot(0, 0);
  }
      
  buttonControl.pollButtons();  
  clock();
  mainIndicator.dynamicIndication();
}

void clock(){
  static uint32_t clockTimer;
  int difference = millis() - clockTimer;
  static int collectedDifference;

  if(difference > 999){    
    clockTimer = millis();    
    second++;    

    /*collectedDifference += (difference - 1000);
    if(collectedDifference > 1333){
      second++;
      collectedDifference = 0;
    }*/

    if(second > 59){ minute++; second = 0; clockTimer += 41;}
    if(minute > 59){ hour++; minute = 0;}
    if(hour > 23){ hour = 0;}   
    String formattedTime; formattedTime.reserve(4);    
    if(currentMode == 0){ showFormattedTime(false); mainIndicator.setDot(2, 2);}
    else if(currentMode == 1){  showFormattedTime(true); mainIndicator.setDot(2, 1);}    
  }
}

void showFormattedTime(bool mmss){
  String formattedTime; formattedTime.reserve(4);   
  if(!mmss){ //hhmm
    formattedTime  += addTimeZero(hour); formattedTime += addTimeZero(minute);    
    mainIndicator.sendData(formattedTime);          
  }
  else{
    formattedTime  += addTimeZero(minute); formattedTime += addTimeZero(second);    
    mainIndicator.sendData(formattedTime);          
  }
}
String addTimeZero(byte number){
  String convNumber; convNumber.reserve(2);
  convNumber = String(number);
  if(number < 10){ convNumber = 0 + convNumber;}
  return convNumber;
};


