//опрос кнопок в разных режимах работы. делаю это классом, чтоб функции отсюда не болтались в глобальной области видимости
analogButton btnOK(0, 1023);
analogButton btnPLUS(0, 933);
analogButton btnMINUS(0, 856);
analogButton btnSET(0, 791);

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
      }   
    }   

  private:
    String menuList[4] = {"alarmset", "radiation", "sd audio player", "internet radio player"};
    byte menuModes[4] = {3,10,15,20};
    byte menuItem = 0;

    void showMenuItemName(){            
      mainIndicator.setDot(2,0);
      mainIndicator.sendData(menuList[menuItem]);
    }   

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

    void poll_2(){
      //кнопки ПЛЮС и МИНУС листают пункты меню
      if(btnPLUS.poll()){ 
        menuItem++;        
        btnPLUS.ignoreHolding = true;
        if(menuItem > 5){ menuItem = 0;}
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
        btnOK.ignoreHolding = true;
      }
    }

    void poll_3(){
      if(btnSET.poll()){ currentMode = 2; showMenuItemName(); btnSET.ignoreHolding = true; }
      if(btnOK.poll()){ currentMode = 4; btnOK.ignoreHolding = true;}
    }

    void poll_4(){
      if(btnPLUS.poll()){ alarmHour++; if(alarmHour > 23){ alarmHour = 0;} btnPLUS.ignoreHolding = true;}
      if(btnMINUS.poll()){ alarmHour--; if(alarmHour > 23){ alarmHour = 23;} btnMINUS.ignoreHolding = true;}
      if(btnOK.poll()){ currentMode = 5; btnOK.ignoreHolding = true;}
    }

    void poll_5(){
      if(btnPLUS.poll()){ alarmMinute++; if(alarmMinute > 59){ alarmMinute = 0;} btnPLUS.ignoreHolding = true;}
      if(btnMINUS.poll()){ alarmMinute--; if(alarmMinute > 59){ alarmMinute = 59;} btnMINUS.ignoreHolding = true;}
      if(btnOK.poll()){ currentMode = 3; btnOK.ignoreHolding = true;}
    }    
};


