#include "sevenSegment74.h";
#include "analogButton.h";
#include "tumbler.h";

sevenSegment74 mainIndicator(4,3,2);
tumbler tmblrAlarmEnabled(1);

byte hour, minute, second;
byte alarmHour = 7, alarmMinute = 0;
bool blink = false;
byte currentMode; //ID текущего режима. от него зависит показания индикатора и реакция кнопок.
// 0 - часы
// 1 - показ минут и секунд
// 2 - главное меню
//  201 - прием данных с ПК и вывод их на экран, 202 - тестер кнопок, 203 - тестер уровней кнопок

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
      if(incomingByte != 13 and incomingByte != 10) { inputData += char(incomingByte);}
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

void showFormattedTime(byte mode){
  String formattedTime; formattedTime.reserve(4);   
  switch(mode){
    //hhmm
    case 0: { formattedTime  += addTimeZero(hour, true); formattedTime += addTimeZero(minute, false); break;}
    //mmss
    case 1: { formattedTime  += addTimeZero(minute, false); formattedTime += addTimeZero(second, false); break;}
    //alarm hhmm
    case 2: { formattedTime  += addTimeZero(alarmHour, true); formattedTime += addTimeZero(alarmMinute, false); break;}    
    //alarm blink hh
    case 3: { formattedTime += "  "; formattedTime += addTimeZero(alarmMinute, false); break;}
    //alarm blink mm
    case 4: { formattedTime  += addTimeZero(alarmHour, true); formattedTime += "  ";  break;}    
  }  
  mainIndicator.sendData(formattedTime);      
}
String addTimeZero(byte number, bool isHour){
  String convNumber; convNumber.reserve(2);
  convNumber = String(number);
  if(number < 10){ 
    if(isHour){ convNumber = " " + convNumber;}
    else {convNumber = 0 + convNumber;}
  }
  return convNumber;
};

void clock(){
  static uint32_t clockTimer;
  int difference = millis() - clockTimer;  

  if(difference > 999){    
    clockTimer = millis();    
    second++;       

    if(second > 59){ minute++; second = 0; clockTimer += 41;}
    if(minute > 59){ hour++; minute = 0;}
    if(hour > 23){ hour = 0;}   
    if(hour == alarmHour and minute == alarmMinute){
      if(tmblrAlarmEnabled.poll()){ mainIndicator.setDot(3,1);}
    }
    String formattedTime; formattedTime.reserve(4);    
    if(currentMode == 0){ showFormattedTime(0); mainIndicator.setDot(2, 2);}
    else if(currentMode == 1){  showFormattedTime(1); mainIndicator.setDot(2, 1);}    
    else { indicateSomethingElse();}
  }
}

void indicateSomethingElse(){
  switch(currentMode){
    //показ будильника
    case 3:{  showFormattedTime(2); mainIndicator.setDot(2,1); break;}
    //настройка будильника: мигание ЧАСОВ
    case 4:{  
      if(!blink){ showFormattedTime(3); blink = true;}
      else{ showFormattedTime(2); blink = false;} break;
    }
    //настройка будильника: мигание МИНУТ
    case 5:{
      if(!blink){ showFormattedTime(4); blink = true;}
      else{ showFormattedTime(2); blink = false;} break;
    }    
  }
}




