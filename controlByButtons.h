//опрос кнопок в разных режимах работы. делаю это классом, чтоб функции отсюда не болтались в глобальной области видимости
analogButton btnOK(0, 1024);
analogButton btnPLUS(0, 990);
analogButton btnMINUS(0, 910);
analogButton btnSET(0, 840);

class controlByButtons{

  public:
    void pollButtons(){      
      switch(currentMode){
        case 0: poll_0(); break;  //РЕЖИМ ПО УМОЛЧАНИЮ - отображение часов      
        case 1: poll_1(); break;  //РЕЖИМ ОТОБРАЖЕНИЯ минуты:секунды
        case 2: poll_2(); break;  //РЕЖИМ ОТОБРАЖЕНИЯ: главное меню    
        case 3: poll_3(); break;  //НАСТРОЙКА БУДИЛЬНИКА: показ текущего настроенного времени
        case 4: poll_4(); break;  //НАСТРОЙКА БУДИЛЬНИКА: редактирование часов
        case 5: poll_5(); break;  //НАСТРОЙКА БУДИЛЬНИКА: редактирование минут        
        case 6: poll_6(); break;  //НАСТРОЙКА БУДИЛЬНИКА: предложение выбрать аудио
        case 7: poll_7(); break;  //НАСТРОЙКА БУДИЛЬНИКА: выбор аудио
        case 10: poll_10(); break;//БУДИЛЬНИК СРАБОТАЛ
        case 13: poll_13(); break; //ПОКАЗ РАДИАЦИИ (ТЕКУЩЕЕ ЗНАЧЕНИЕ)
        case 14: poll_14(); break; //ПОКАЗ РАДИАЦИИ (СРЕДНЕЕ ЗНАЧЕНИЕ)
        case 15: poll_15(); break;//режим проигрывания музыки с SD
        case 16: poll_16(); break;//показ уровня громкости
        case 20: poll_20(); break;//воспроизведение интернет-радио
        case 21: poll_21(); break;//переключение станций интернет-радио
      }   
    }   

  private:
    String menuList[4] = {"alarmset", "radiation", "sd audio player", "internet radio player"};
    byte menuModes[4] = {3,13,15,20};
    byte menuItem = 0;

    String alarmSoundSelect[3] = {"default", "sd card", "internet radio"};

    void showMenuItemName(){            
      setDot(2,0);
      sendData(menuList[menuItem]);
    }   

    //РЕЖИМ ПО УМОЛЧАНИю
    void poll_0(){
      //кнопка SET вызывает меню
      if(btnSET.poll()){ 
        currentMode = 2; 
        showMenuItemName();
        btnSET.ignoreHolding = true;
      }
      //кнопка ОК переключает режим с обычного на минуты:секунды
      if(btnOK.poll()){ currentMode = 1; btnOK.ignoreHolding = true;}
      //кнопка ПЛЮС увеличивает минуты
      if(btnPLUS.poll()){   
        int pressingTime = btnPLUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          minute++;
          if(minute > 59){ minute = 0; hour++;}
          if(hour > 23){ hour = 0;}
          showFormattedTime(false);
        }      
        else{ 
          if(pressingTime < 50) {prevPressingTime = 0;}
        }
      }
      //кнопка МИНУС уменьшает минуты
      if(btnMINUS.poll()){   
        int pressingTime = btnMINUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          minute--; //если меньше нуля, то там 255 появляется
          if(minute > 59){ minute = 59; hour--;}
          if(hour > 23){ hour = 23;}
          showFormattedTime(false);
        }      
        else{ 
          if(pressingTime < 50) {prevPressingTime = 0;}
        }
      }
    }
    //РЕЖИМ ОТОБРАЖЕНИЯ МИНУТЫ:СЕКУНДЫ
    void poll_1(){
      //кнопка ОК возвращает обратно в обычный режим
        if(btnOK.poll()){ currentMode = 0; btnOK.ignoreHolding = true;}     
        //кнопка SET обнуляет секунды для увеличения точности хода
        if(btnSET.poll()){ 
          second = 0;
          showFormattedTime(true);
          btnSET.ignoreHolding = true;
        }
    }
    //РЕЖИМ ОТОБРАЖЕНИЯ: ГЛАВНОЕ МЕНЮ
    void poll_2(){
      //кнопки ПЛЮС и МИНУС листают пункты меню
      if(btnPLUS.poll()){ 
        menuItem++;        
        btnPLUS.ignoreHolding = true;
        if(menuItem > 3){ menuItem = 0;}
        showMenuItemName();
      }
      if(btnMINUS.poll()){ 
        menuItem--;        
        btnMINUS.ignoreHolding = true;
        if(menuItem > 99){ menuItem = 3;}
        showMenuItemName();
      }
      //кнопка МЕНЮ возвращает обратно
      if(btnSET.poll()){        
        currentMode = 0;
        btnSET.ignoreHolding = true;
      }
      //кнопка ОК закидывает внутрь пункта
      if(btnOK.poll()){        
        currentMode = menuModes[menuItem];
        if(currentMode == 15){ isPlayingMusic = true;}
        if(currentMode == 20){ playMusicFromInternet(); isPlayingMusic = true; isOnlineMusic = true;}
        btnOK.ignoreHolding = true;
      }
    }
    //НАСТРОЙКА БУДИЛЬНИКА: показ текущего настроенного времени
    void poll_3(){
      //кнопка меню возвращает в главное меню
      if(btnSET.poll()){ currentMode = 2; showMenuItemName(); btnSET.ignoreHolding = true; }
      //кнопка ОК входит в режим редактирования времени будильника: часы
      if(btnOK.poll()){ currentMode = 4; btnOK.ignoreHolding = true;}
      //кнопка ПЛЮС переводит в режим выбора звука для будильника
      if(btnPLUS.poll()) {currentMode = 6; sendData("select sound"); btnPLUS.ignoreHolding = true;}
      if(btnMINUS.poll()){ 
        if(alarmEnabled){ alarmEnabled = 0;}
        else {alarmEnabled = 1;}
        saveAlarmSettings();
        btnMINUS.ignoreHolding = true;
      }
    }
    //НАСТРОЙКА БУДИЛЬНИКА: редактирование часов
    void poll_4(){
      //Кнопка SET выходит из редактирования без сохранения
      if(btnSET.poll()){ currentMode = 3; btnSET.ignoreHolding = true;}
      //кнопки ПЛЮС и МИНУС меняют значение часов      
      if(btnPLUS.poll()){ 
        int pressingTime = btnPLUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          alarmHour++;
          if(alarmHour > 23){ alarmHour = 0;}          
          showFormattedTime(2);
        }      
        else{ if(pressingTime < 50) {prevPressingTime = 0;}}
      }
      if(btnMINUS.poll()){ 
        int pressingTime = btnMINUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          alarmHour--;
          if(alarmHour > 23){ alarmHour = 0;}          
          showFormattedTime(2);
        }      
        else{ if(pressingTime < 50) {prevPressingTime = 0;}}
      }
      //кнопка ОК сохраняет значение и переводит на редактирование минут
      if(btnOK.poll()){ currentMode = 5; btnOK.ignoreHolding = true;}
    }
    //НАСТРОЙКА БУДИЛЬНИКА: редактирование минут
    void poll_5(){
      //Кнопка SET выходит из редактирования без сохранения
      if(btnSET.poll()){ currentMode = 3; btnSET.ignoreHolding = true;}
      //кнопки ПЛЮС и МИНУС меняют значение минут
      if(btnPLUS.poll()){
        int pressingTime = btnPLUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          alarmMinute++;
          if(alarmMinute > 59){ alarmMinute = 0;}
          showFormattedTime(2);
        }      
        else{ if(pressingTime < 50) {prevPressingTime = 0;}}
      }
      if(btnMINUS.poll()){ 
        int pressingTime = btnMINUS.poll();      
        static int prevPressingTime;
        if(pressingTime > prevPressingTime+50){
          prevPressingTime = pressingTime;
          alarmMinute--;
          if(alarmMinute > 59){ alarmMinute = 59;}
          showFormattedTime(2);
        }      
        else{ if(pressingTime < 50) {prevPressingTime = 0;}}
      }
      //кнопка ОК сохраняет значение и переводит на режим отображения времени будильника
      if(btnOK.poll()){ currentMode = 3; btnOK.ignoreHolding = true; saveAlarmSettings();}
    }    
    //НАСТРОЙКА БУДИЛЬНИКА: предложение выбрать аудио
    void poll_6(){
      //Кнопка SET выходит из редактирования без сохранения
      if(btnSET.poll()){ currentMode = 2; btnSET.ignoreHolding = true;}
      //кнопки ПЛЮС и МИНУС переключают выбор аудио
      if(btnPLUS.poll()){ currentMode = 3; btnPLUS.ignoreHolding = true;}
      if(btnMINUS.poll()){ currentMode = 3; btnMINUS.ignoreHolding = true;}
      //кнопка ОК сохраняет значение и выходит в главное меню
      if(btnOK.poll()){ currentMode = 7; sendData(alarmSoundSelect[alarmSound]); btnOK.ignoreHolding = true;}
    }
    //НАСТРОЙКА БУДИЛЬНИКА: выбор аудио
    void poll_7(){
      //Кнопка SET выходит из редактирования без сохранения
      if(btnSET.poll()){ currentMode = 3; btnSET.ignoreHolding = true;}
      //кнопки ПЛЮС и МИНУС переключают выбор аудио
      if(btnPLUS.poll()){
        alarmSound++; if(alarmSound > 2){ alarmSound = 0;}
        sendData(alarmSoundSelect[alarmSound]);
        btnPLUS.ignoreHolding = true;
      }
      if(btnMINUS.poll()){
        alarmSound--; if(alarmSound > 99){ alarmSound = 2;}
        sendData(alarmSoundSelect[alarmSound]);
        btnMINUS.ignoreHolding = true;
      }
      //кнопка ОК сохраняет значение и выходит в главное меню
      if(btnOK.poll()){ currentMode = 3; btnOK.ignoreHolding = true; saveAlarmSettings();}
    }
    //БУДИЛЬНИК СРАБОТАЛ
    void poll_10(){
      if(btnSET.poll()){
        currentMode = 0;
        isPlayingMusic = false;
        decoder->stop();        
        powerHW104(false);
        btnSET.ignoreHolding = true;
      }      
    }
    //ПОКАЗ РАДИАЦИИ (ТЕКУЩЕЕ ЗНАЧЕНИЕ)
    void poll_13(){
      if(btnSET.poll()){
        currentMode = 0;
        setDot(3, 0);
        btnSET.ignoreHolding = true;
      }
      if(btnOK.poll()){
        currentMode = 14;
        btnOK.ignoreHolding = true;
      }
    }
    //ПОКАЗ РАДИАЦИИ (СРЕДНЕЕ ЗНАЧЕНИЕ)
    void poll_14(){
      if(btnSET.poll()){
        currentMode = 0;
        setDot(3, 0);
        btnSET.ignoreHolding = true;
      }
      if(btnOK.poll()){
        currentMode = 13;
        btnOK.ignoreHolding = true;
      }
    }
    //ВКЛЮЧЕНИЕ АУДИО С КАРТЫ ПАМЯТИ
    void poll_15(){
      //кнопка SET отключает воспроизведение
      if(btnSET.poll()){                
        if(isOnlineMusic){ stopPlayingMusicFromInternet();}
        else{ 
          decoder->stop(); delete decoder; decoder = NULL;
          source->close(); delete source; source = NULL;
        }
        powerHW104(false);
        isPlayingMusic = false;
        isOnlineMusic = false;
        currentMode = 0;
        btnSET.ignoreHolding = true;
      }

      if(btnPLUS.poll()){                
        if(volume < 3){ volume += 0.1;}        
        float volumeToDisp = volume * 10;
        sendData(String("VL"+addTimeZero((int)(volume * 10), true)));
        output->SetGain(float(volume));
        btnPLUS.ignoreHolding = true;
        currentMode = 16;        
      }

      if(btnMINUS.poll()){                      
        if(volume > 0){ volume -= 0.1;}        
        float volumeToDisp = volume * 10;  
        sendData(String("VL"+addTimeZero((int)(volume * 10), true)));
        output->SetGain(float(volume));
        btnMINUS.ignoreHolding = true;
        currentMode = 16;
      }

      if(btnOK.poll() and !isOnlineMusic){
        decoder->stop();
        source->close();
        btnOK.ignoreHolding = true;
      }
    }
    //ПОКАЗ ГРОМКОСТИ воспроизведения
    void poll_16(){
      static uint32_t returnTo15;
      if(!returnTo15){ returnTo15 = millis()+2000;}
      else{
        if(millis() < returnTo15){ poll_15();}
        else{
          if(isOnlineMusic){ currentMode = 20;}
          else{ currentMode = 15;}
          returnTo15 = 0;          
        }
      }
    }

    void poll_20(){
      poll_15();      

      if(btnOK.poll()){
        sendData("st "+String(currentRadioStation)); 
        currentMode = 21; 
        btnOK.ignoreHolding = true;
      }
    }

    void poll_21(){
      if(btnSET.poll()){
        btnSET.ignoreHolding = true;
        currentMode = 20;
      }
      if(btnPLUS.poll()){
        btnPLUS.ignoreHolding = true;
        currentRadioStation++;
        if(currentRadioStation > 9){ currentRadioStation = 0;}
        sendData("st "+String(currentRadioStation));                
      }
      if(btnMINUS.poll()){
        btnPLUS.ignoreHolding = true;
        currentRadioStation--;
        if(currentRadioStation > 250){ currentRadioStation = 9;}
        sendData("st "+String(currentRadioStation));        
      }
      if(btnOK.poll()){
        stopPlayingMusicFromInternet();
        playMusicFromInternet();
        currentMode = 20; btnOK.ignoreHolding = true;
      }
    }
    
};


