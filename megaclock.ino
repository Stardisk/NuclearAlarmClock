#include <radSens1v2.h>
#include "Math.h"
#include "analogButton.h"
#include "tumbler.h"
#include "AudioFileSourceSD.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioGeneratorMP3.h"
#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include <SPI.h>
#include "ESP8266WiFi.h"
#include "LedControl.h"

static const uint8_t _D0   = 3;
static const uint8_t _D1   = 1;
static const uint8_t _D2   = 16;
static const uint8_t _D3   = 5;
static const uint8_t _D4   = 4;
static const uint8_t _D5   = 14;
static const uint8_t _D6   = 12;
static const uint8_t _D7   = 13;
static const uint8_t _D8   = 0;
static const uint8_t _D9   = 2;
static const uint8_t _D10  = 15;
static const uint8_t _D11  = 13;
static const uint8_t _D12  = 12;
static const uint8_t _D13  = 14;
static const uint8_t _D14  = 4;
static const uint8_t _D15  = 5;


byte _symbolsArr[101] = {0b00000000,
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [1-10]
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [11-20]
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [21-30]
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [31-40]
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000000, 0b00100101, 0b01111110, 0b00110000, 0b01101101, // [41-44], "-", [46], "/", "0", "1", "2"
  0b01111001, 0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01111011, 0b00000000, 0b00000000, 0b00000000, // "3", "4", "5", "6", "7", "8", "9", [58-60]
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110111, 0b00011111, 0b01001110, 0b00111101, 0b01001111, 0b01000111, // [61-64], "A", "B", "C", "D", "E", "F"
  0b01011110, 0b00010111, 0b00000110, 0b00111100, 0b01010111, 0b00001110, 0b01010101, 0b01110110, 0b01111110, 0b01100111, // "G", "H", "I", "J", "K", "L", "M", "N", "O", "P"
  0b01110011, 0b00000101, 0b01011011, 0b00001111, 0b00111110, 0b00011100, 0b00101011, 0b00110111, 0b00111011, 0b01101101, // "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
  0b00000000, 0b00000000, 0b00000000, 0b01100010, 0b00001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000  // [91-93, "^", "_", 96-100]    
};
bool dots[4];
bool rsActive = false;
String rsData, staticData;    
//LedControl LC = LedControl(_D4,_D3,_D2,1);
LedControl LC = LedControl(_D2,1);

ClimateGuard_RadSens1v2 radSens(RS_DEFAULT_I2C_ADDRESS);

//tumbler tmblrAlarmEnabled(_D9, true);

File audioDir;
AudioFileSourceSD *source = NULL;
AudioOutputI2SNoDAC *output = NULL;
AudioGeneratorMP3 *decoder = NULL;
AudioFileSourceICYStream *stream;
AudioFileSourceBuffer *buff;

byte hour, minute, second;
byte alarmHour = 7, alarmMinute = 0, alarmSound = 0;
bool alarmTriggered = false;
bool blink = false;
byte currentMode; //ID текущего режима. от него зависит показания индикатора и реакция кнопок.

bool isPlayingMusic = false; bool isOnlineMusic = false;
float volume = 1;
String radioStations[10]; 
byte currentRadioStation = 0;

// ********** special functions for indicator *************
void setDot(byte dotNumber, byte state){
  if(dots[dotNumber] != state){
    if(state < 2){ dots[dotNumber] = state;}
    else{ dots[dotNumber] = (dots[dotNumber]) ? 0:1;    }
  }      
}   
void sendDataToDisplay(String data){
    LC.setRow(0, 3, _symbolsArr[data[0]] + dots[3] * 128);
    LC.setRow(0, 2, _symbolsArr[data[1]] + dots[2] * 128);
    LC.setRow(0, 1, _symbolsArr[data[2]] + dots[1] * 128);
    LC.setRow(0, 0, _symbolsArr[data[3]] + dots[0] * 128);  
}
void runningString(String data){    
  static uint32_t rsTimer;
  static byte rsStartPos, rsDataLen;
  if(!rsActive){ 
    rsStartPos = 0;
    rsActive = true;
    rsTimer = millis();
    rsData = "   "; rsData += data.substring(0, 124);
    rsDataLen = rsData.length();    
  }      
  if(millis() - rsTimer > 750){
    sendDataToDisplay(data.substring(rsStartPos, rsStartPos+4));
    rsTimer = millis();
    rsStartPos++;        
  }
  if(rsStartPos > rsDataLen){        
    rsStartPos = 0;         
  }           
}
void sendData(String inputData){
  if(rsActive){ rsActive = false;} //отключаем предыдущую бегущую строку, если она была
  //задаем новые данные
  inputData.toUpperCase();
  if(inputData.length() <= 4){  staticData = inputData;  sendDataToDisplay(staticData); }
  else{ runningString(inputData); staticData = "    ";} 
}
//******** end of indicator functions ***************

void setup(){
  Serial.begin(115200);
  delay(100);    

  LC.shutdown(0,false);
  LC.setIntensity(0,8);
  LC.clearDisplay(0);
  sendData("INIT");
  
  output = new AudioOutputI2SNoDAC();
  decoder = new AudioGeneratorMP3();
  source = new AudioFileSourceSD();

  while(!radSens.radSens_init()){
    sendData("rad-");
  }
  Serial.println("rad init");

  //подключение SD-карты
  while(!SD.begin(_D10)){
    sendData("card");
    Serial.println("load SD");
    delay(1000);
  }    
  Serial.println("Sd loaded");
  audioDir = SD.open("/");  
  byte currentSymbol; 
  
  Serial.setDebugOutput(true);
  
  //WiFi.disconnect(true);
  //ESP.eraseConfig();

  
  
  WiFi.persistent(false);
  WiFi.begin("Stardisk2.4", "84C9D3A1");     
  while(WiFi.status() != WL_CONNECTED){
    delay(10);
  }    

  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  //загрузка настроек будильника
  File alarmSet = SD.open("/settings/alarm.txt");
  String alarmParam; byte currentParam = 0;
  while (alarmSet.available()) {        
      currentSymbol = alarmSet.read();            
      if(currentSymbol == 124){
        switch(currentParam){
          case 0: {alarmHour = alarmParam.toInt(); break;}
          case 1: {alarmMinute = alarmParam.toInt(); break;}          
        }        
        alarmParam = "";
        currentParam++;
      }
      else{   alarmParam += char(currentSymbol);}
  }
  if(currentParam == 2) { alarmSound = alarmParam.toInt();} 

  Serial.println("alarm loaded");

  //загрузка радиостанций
  File stations = SD.open("/settings/stations.txt"); byte stationId = 0;
  while (stations.available()) {    
    currentSymbol = stations.read();      
    if(currentSymbol == 13){ stationId++;}
    else if(currentSymbol != 10){
      radioStations[stationId] += char(currentSymbol);
    }    
  }  
  Serial.println(radioStations[0]);
  Serial.println(radioStations[1]);   

  
}

#include "misc.h"
#include "controlByButtons.h"
controlByButtons buttonControl;
static uint32_t pollBtnsTimer;
void loop(){ 
  if(currentMode == 203){ sendData(String(analogRead(0)));}   
  if(currentMode == 205){ sendData(String(WiFi.status()));}       

  /*if(tmblrAlarmEnabled.poll()){    
    setDot(0, 1);
  }
  else{
    setDot(0, 0);
  }*/
  if(millis() - pollBtnsTimer > 10){
    pollBtnsTimer = millis();
    buttonControl.pollButtons();
    waitForInput();  
  }
  calculatingTime();
  if(isPlayingMusic){ playMusic();}  

  if(rsActive){ runningString(rsData);}
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
  sendData(formattedTime);      
}
String addTimeZero(byte number, bool isHour){
  String convNumber; convNumber.reserve(2);
  convNumber = String(number);
  if(number < 10){ 
    if(isHour){ convNumber = " " + convNumber;}
    else {convNumber = "0" + convNumber;}
  }
  return convNumber;
};

void calculatingTime(){
  static uint32_t clockTimer;
  int difference = millis() - clockTimer;  

  if(difference > 999){    
    clockTimer = millis();    
    second++;           

    if(second > 59){ 
      minute++; second = 0; clockTimer += 41; 
      if(alarmTriggered) {alarmTriggered = false;} //чтоб будильник мог потом сработать снова
    }
    if(minute > 59){ hour++; minute = 0;}
    if(hour > 23){ hour = 0;}   
    if(hour == alarmHour and minute == alarmMinute and !alarmTriggered){
        //if(tmblrAlarmEnabled.poll()){  alarmTriggered = true; alarm();}
    }    
    String formattedTime; formattedTime.reserve(4);    
    if(currentMode == 0 or currentMode == 10 or currentMode == 15 or currentMode == 20){ showFormattedTime(0); setDot(2, 2);}
    else if(currentMode == 1){  showFormattedTime(1); setDot(2, 1);}    
    else { indicateSomethingElse();}
  }
}

void indicateSomethingElse(){
  switch(currentMode){
    //показ будильника
    case 3:{  showFormattedTime(2); setDot(2,1); break;}
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
    //показ уровня радиации
    case 14:{           
      int radLevel;
      radLevel = ceil(radSens.getRadIntensyDynamic());
      if(radLevel > 9999){ sendData("----");}
      else{
          if(radLevel > 1000){ setDot(2,1); setDot(3,0);}
          else{ setDot(3,1); setDot(2,0);}
          if(radLevel > 99){ sendData(String(radLevel));}
          else{ sendData(String("0" + String(radLevel)));}          
      }
      
      break;
    }
  }
}

void playMusic(){  
  if (decoder->isRunning()) {
    if (!decoder->loop()) decoder->stop();
  } else {
    if(isOnlineMusic){ return;}

    if(currentMode == 10 and !alarmSound){ 
      source->close();
      source->open("__alarm.mp3");
      decoder->begin(source, output);
    }
    else{
      File file = audioDir.openNextFile();
      if (file) {                
        String fileName = file.name();
        if (fileName.endsWith(".mp3") and fileName != "__alarm.mp3") {
          source->close();
          if (source->open(file.name())) {           
            Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
            decoder->begin(source, output);
          }
          else {
            Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
          }
        } 
      }   
      else{ audioDir.rewindDirectory(); }
    }      
  }
}

void playMusicFromInternet(){    
  //const String selectedRS = radioStations[currentRadioStation];
  //const char *url = &radioStations[currentRadioStation];
  // stream = new AudioFileSourceICYStream(radioStations[currentRadioStation]);
  const char *url = "http://orangepi.stardisk.xyz/radio/radio2.php";
  //const char *url = "http://nashe1.hostingradio.ru:8000/ultra-128.mp3";
  stream = new AudioFileSourceICYStream(url);
  stream->RegisterMetadataCB(MDCallback, (void*)"ICY");
  buff = new AudioFileSourceBuffer(stream, 2048);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");  
  decoder->begin(buff, output);    
}

void alarm(){
  currentMode = 10;
  output->SetGain(2.5);                  
  switch (alarmSound){
    case 0:{      
      source->close();      
      isPlayingMusic = true; break;      
    }
    case 1:{
      isPlayingMusic = true; break;
    }
    case 2:{
      playMusicFromInternet();
      isPlayingMusic = true; break;
    }
  }
}