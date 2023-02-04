class tumbler{
  private:
    byte pin;        
  
  public:
    tumbler(byte pinNumber){
      pin = pinNumber;      
    }

    bool poll(){
      return (analogRead(pin) > 1000);      
    }
};