class analogButton{

  private:
    uint32_t timeWhenPressed;    
    int maxAnalogLevelOnPress;
    byte pin;
    int pressingTime;
    int debrisTime = 100;

  public:
    int currentButtonLevel;
    bool ignoreHolding = false;     //установить этот флаг, если не требуется обрабатывать дальнейшее удержание кнопки

    analogButton(byte pinNumber, int levelOnPress){
      pin = pinNumber;
      maxAnalogLevelOnPress = levelOnPress; //делаем допуск возможного значения
    }

    int poll(){      
      currentButtonLevel = analogRead(pin);      
      //кнопка нажата
      if(currentButtonLevel > 50){            
        //смотрим, чтоб уровень кнопки не превышал максимальный для нее, но не был меньше, чем ее максимальный минус 10
        if((currentButtonLevel <= maxAnalogLevelOnPress) and (currentButtonLevel > (maxAnalogLevelOnPress - 10))){
          //нет времени нажатия - кнопку нажали только что
          if(!timeWhenPressed){
            timeWhenPressed = millis();
            return 0;
          }
          //есть время нажатия - кнопку держат какое-то время
          else{
            if(ignoreHolding){ return 0;} // запрошено игнорирование дальнейшего удерживания
            else{ 
              pressingTime = millis() - timeWhenPressed;
              if(pressingTime > debrisTime){
                return pressingTime - debrisTime;
              }
            }
          }
        }        
        else{ return 0;}
      }
      else{
        if(timeWhenPressed > 0) {
          timeWhenPressed = 0;          
          pressingTime = 0;
          ignoreHolding = false;
        }
        return 0;
      }   
    }    
};