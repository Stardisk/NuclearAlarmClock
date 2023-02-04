void pollButtons(){
  //РЕЖИМ ПО УМОЛЧАНИЮ
  if(currentMode == 0){      
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

  //РЕЖИМ ОТОБРАЖЕНИЯ минуты:секунды
  if(currentMode == 1){
    //кнопка ОК возвращает обратно в обычный режим
    if(btnOK.poll()){ currentMode = 0; btnOK.ignoreHolding = true;}     
    //кнопка SET обнуляет секунды для увеличения точности хода
    if(btnSET.poll()){ 
      second = 0;
      showFormattedTime(true);
      btnSET.ignoreHolding = true;
    }
  }

}