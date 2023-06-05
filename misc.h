//сохранение настроек будильника
void saveAlarmSettings(){  
  SD.remove("/settings/alarm.txt");
  File alSets = SD.open("/settings/alarm.txt", FILE_WRITE);
  alSets.print(alarmHour); alSets.print("|"); alSets.print(alarmMinute); alSets.print("|"); alSets.print(alarmSound); alSets.print("|"); alSets.print(alarmEnabled);
  alSets.close();
}
//отладочная функция приема данных с последовательного порта
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

    if(inputData.substring(0, 5) == "/wifi"){   currentMode = 205;} 
 
    //управление точками семисегментника: /dot [номер точки] [0/1/2 == выкл/вкл/перекл]
    if(inputData.substring(0, 4) == "/dot"){
      byte dotNumber = inputData.substring(5,6).toInt();           
      setDot(dotNumber, 2);
    }    

    //выход в режим часов
    if(inputData.substring(0, 5) == "/exit"){   currentMode = 0;} 

    if(inputData.substring(0, 5) == "/mode"){   sendData(String(currentMode));}      

    if(currentMode == 201) { sendData(inputData);}    
  }
}

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  sendData(s2);
  Serial.flush();
}

void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}