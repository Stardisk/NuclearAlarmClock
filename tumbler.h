class tumbler{
  private:
    byte pin;        
    bool digital;
  
  public:
    tumbler(byte pinNumber, bool isDigital){
      pin = pinNumber;      
      digital = isDigital;
      if(digital){
        pinMode(pinNumber, INPUT_PULLUP);
      }
    }

    bool poll(){
      if(digital){ return !digitalRead(pin);}
      else{ return (analogRead(pin) > 1000); }      
    }
};