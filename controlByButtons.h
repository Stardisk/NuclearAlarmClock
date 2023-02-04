//опрос кнопок в разных режимах работы. делаю это классом, чтоб функции отсюда не болтались в глобальной области видимости
analogButton btnOK(0, 1023);
analogButton btnPLUS(0, 933);
analogButton btnMINUS(0, 856);
analogButton btnSET(0, 791);

class controlByButtons{

  public:
    void pollButtons(){      
      if(currentMode == 0){      poll_0();}  //РЕЖИМ ПО УМОЛЧАНИЮ - отображение часов      
      if(currentMode == 1){      poll_1();}  //РЕЖИМ ОТОБРАЖЕНИЯ минуты:секунды
      if(currentMode == 2){      poll_2();}  //режим отображения: главное меню
    }

  private:
    String menuList[4] = {"alarmset", "battery", "radiation", "glitches"};
    byte menuItem = 0;

    void showMenuItemName(){      
      if(menuItem == 255){ menuItem = 3;}
      else if(menuItem > 3){ menuItem = 0;}    
      mainIndicator.setDot(2,0);
      mainIndicator.sendData(menuList[menuItem]);
    }

    void alarmset(){
      mainIndicator.sendData("INSIDE");
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
        showMenuItemName();
      }
      if(btnMINUS.poll()){ 
        menuItem--;        
        btnPLUS.ignoreHolding = true;
        showMenuItemName();
      }
      //кнопка МЕНЮ возвращает обратно
      if(btnSET.poll()){
        currentMode = 0;
        btnSET.ignoreHolding = true;
      }
      //кнопка ОК закидывает внутрь пункта
      if(btnOK.poll()){
      //  menuList[menuItem]();
      }
    }
};


