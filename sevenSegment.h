class sevenSegment{

  private:
    byte digitsArr[91] = {0b00000000,
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [1-10]
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [11-20]
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [21-30]
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, // [31-40]
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b01010010, 0b00111111, 0b00000110, 0b01011011, // [41-44], "-", [46], "/", "0", "1", "2"
      0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0b00000000, 0b00000000, 0b00000000, // "3", "4", "5", "6", "7", "8", "9", [58-60]
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001, // [61-64], "A", "B", "C", "D", "E", "F"
      0b00111101, 0b01110100, 0b00010000, 0b00011110, 0b01110101, 0b00111000, 0b01010101, 0b01010100, 0b01011100, 0b01110011, // "G", "H", "I", "J", "K", "L", "M", "N", "O", "P"
      0b01100111, 0b01010000, 0b01101101, 0b01111000, 0b00011100, 0b00011100, 0b01101010, 0b01110110, 0b01101110, 0b01011011  // "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
    };
    bool dots[4];
    byte enabledDigit = 0;    
    byte refreshRate = 5;
    byte skippedCalls = 0;
    bool rsActive = false;
    String rsData, staticData;     

    //посылает на экран 4 символа, которые необходимо отрисовать
    void sendDataToDisplay(String data){      
      if(skippedCalls == refreshRate){
        //отключаем все сегменты  
        digitalWrite(2, HIGH);
        digitalWrite(3, HIGH);
        digitalWrite(4, HIGH);
        digitalWrite(5, HIGH);  
      }

      if(skippedCalls == refreshRate+1){
        data.toUpperCase();
        enabledDigit++;
        if(enabledDigit > 3){ enabledDigit = 0;}
        //включаем нужные сгементы
        for (int i=6;i<=12;i++){
          digitalWrite(i, bitRead(digitsArr[data[3-enabledDigit]], i-6));
        }
        //точка
        digitalWrite(13, dots[enabledDigit]);
        //зажигаем нужную цифру
        digitalWrite(enabledDigit+2, LOW);           
        skippedCalls = 0;
      }     

      skippedCalls++;
       
    }

    //анимированная бегущая строка
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
      sendDataToDisplay(data.substring(rsStartPos, rsStartPos+4));
      if(millis() - rsTimer > 750){
        rsTimer = millis();
        rsStartPos++;        
      }
      if(rsStartPos > rsDataLen){        
        rsStartPos = 0;         
      }           
    }    

  public:    
    sevenSegment(){
      rsData.reserve(128);
      staticData.reserve(4);     

      pinMode(6, OUTPUT);  //a
      pinMode(7, OUTPUT);  //b
      pinMode(8, OUTPUT);  //c
      pinMode(9, OUTPUT);  //d
      pinMode(10, OUTPUT); //e
      pinMode(11, OUTPUT); //f
      pinMode(12, OUTPUT); //g
      pinMode(13, OUTPUT); //dp

      pinMode(2, OUTPUT); //cathode 0
      pinMode(3, OUTPUT); //cathode 1  
      pinMode(4, OUTPUT); //cathode 2  
      pinMode(5, OUTPUT); //cathode 3  
    }    

    //вкл/выкл десятичные точки
    void setDot(byte dotNumber, byte state){
      if(dots[dotNumber] != state){
        if(state < 2){ dots[dotNumber] = state;}
        else{ dots[dotNumber] = (dots[dotNumber]) ? 0:1;    }
      }      
    }    
    
    //получение данных с микроконтроллера для их отрисовки на экранчике. Посылается единожды и будет крутиться на экранчике, пока не придут новые данные
    void sendData(String inputData){
      if(rsActive){ rsActive = false;} //отключаем предыдущую бегущую строку, если она была
      //задаем новые данные
      if(inputData.length() <= 4){  staticData = inputData;}
      else{ runningString(inputData); staticData = "    ";} 
    }

    //put this to loop()
    void dynamicIndication(){
      if(rsActive){ runningString(rsData);}
      else{ sendDataToDisplay(staticData);  }   
    }  

    void setRefreshRate(byte rate){
      refreshRate = rate;
    }
};