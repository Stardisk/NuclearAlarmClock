#include "sevenSegment.h";
#include "analogButton.h";

sevenSegment mainIndicator;
analogButton btn0(0, 1023);
analogButton btn1(0, 685);

byte hour, minute, second;
byte currentMode; 
// 0 - часы, 1 - показ минут и секунд,  201 - прием данных с ПК и вывод их на экран, 202 - тестер кнопок

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
    
    if(inputData.substring(0, 4) == "/btn"){
      currentMode = 202;
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

void loop(){  
  waitForInput();  

  if(currentMode == 202){
    mainIndicator.sendData(String(btn0.currentButtonLevel));
    if(btn0.poll()){
      Serial.println("btn0");
    }

    if(btn1.poll()){
      Serial.println("btn1");
    }   
  }
      
  if(currentMode == 0){
    if(btn0.poll()){
    mainIndicator.sendData("btn0");

    if(btn1.poll()){
      mainIndicator.sendData("btn1");
    }
    /*if(currentMode == 1){ currentMode = 0;}
    else if(currentMode == 0){ currentMode = 1;}     
    btn0.ignoreHolding = true;
    */
  }

  
  } 
  
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

    collectedDifference += (difference - 1000);
    if(collectedDifference > 1333){
      second++;
      collectedDifference = 0;
    }

    if(second > 59){ minute++; second = 0;}
    if(minute > 59){ hour++; minute = 0;}
    if(hour > 23){ hour = 0;}   
    String formattedTime; formattedTime.reserve(4);    
    if(currentMode == 0){
      formattedTime  += addTimeZero(hour); formattedTime += addTimeZero(minute);    
      mainIndicator.sendData(formattedTime);      
      mainIndicator.setDot(2, 2);
    }
    else if(currentMode == 1){
      formattedTime  += "  "; formattedTime += addTimeZero(second);    
      mainIndicator.sendData(formattedTime);      
      mainIndicator.setDot(2, 1);
    }
    
  }
}
String addTimeZero(byte number){
  String convNumber; convNumber.reserve(2);
  convNumber = String(number);
  if(number < 10){ convNumber = 0 + convNumber;}
  return convNumber;
};


