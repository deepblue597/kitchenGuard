#include <RF22.h>
#include <RF22Router.h>
#include "pitch.h"
#include "Waveshare_LCD1602_RGB.h"


#define MY_ADDRESS 5
RF22Router rf22(MY_ADDRESS);
const int buzzer = 4 ;
char strNumber[2];
char sensor_value[4] ;
//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
int noteDuration = 1000 / 4 ;
String second_line ; 

// to distinguish the notes, set a minimum time between them.


// the note's duration + 30% seems to work well:
int pauseBetweenNotes = noteDuration * 1.30;


Waveshare_LCD1602_RGB lcd(16,2);  //16 characters and 2 lines of show
int r,g,b,t=0;



void setup() {
  Serial.begin(9600);


   // initialize LCD
  lcd.init();
  lcd.setCursor(0,0);
  lcd.send_string("KitchenGuard");
  lcd.setCursor(0,1);
  lcd.send_string("Activation");


  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(436.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  //modulation

  delay(2000);
}


void loop() {
  uint8_t buf[RF22_ROUTER_MAX_MESSAGE_LEN];
  char incoming[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(buf, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(incoming, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  uint8_t len = sizeof(buf);
  uint8_t from;
  int received_value = 0;
 

 
  if (rf22.recvfromAck(buf, &len, &from))
  {
    buf[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
    memcpy(incoming, buf, RF22_ROUTER_MAX_MESSAGE_LEN);
    //Serial.println(incoming);
    //Serial.print("got request from : ");
    //Serial.println(from, DEC);
    received_value = atoi((char*)incoming);
    strNumber[0] = incoming[0];  // Assign the character to the first element of the array
    strNumber[1] = '\0';      // Null-terminate the array
      
    

    int status = atoi(strNumber) ;
    //Serial.println(status);
    sensor_value[0] = incoming[2];
    sensor_value[1] = incoming[3];
    sensor_value[2] = incoming[4]; 
    sensor_value[3] = '\0' ; 
    second_line = "Value:" + String(sensor_value) + "pos:" + from; 
    Serial.println("2 is") ; 
    Serial.println(incoming[2]) ;
    Serial.println("3 is")  ;
    Serial.println(second_line) ;    
    switch(status){
      case 0:
        lcd.clear() ;
        lcd.setCursor(0, 0);
        //lcd.print("All good :)");
        lcd.send_string("All good :)");
        break ; 

      case 1:
        lcd.setCursor(0, 0);
        lcd.send_string("Water Boiling");
        //lcd.print("Water Boiling");
   
        lcd.setCursor(0, 1);
        //lcd.print("Pot on place");
        lcd.send_string("Pot on place");
        break ; 


      case 2:  
        lcd.setCursor(0, 0);
        //lcd.print("Water Boiling");
        lcd.send_string("Water Boiling");
     
        lcd.setCursor(0, 1);
        //lcd.print("Pot missing");
        lcd.send_string(second_line.c_str());
        tone(buzzer,NOTE_B7, noteDuration); // play a tone for prefined time


        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(buzzer);

        break ; 




      case 3:
        lcd.setCursor(0, 0);
        //lcd.print("Gas leak!!!");
        lcd.send_string("Gas leak!!!");
       
        lcd.setCursor(0, 1);
        //lcd.print("Check kitchen!");
        lcd.send_string(second_line.c_str());
        tone(buzzer,NOTE_DS8, noteDuration);
        delay(pauseBetweenNotes);  
        noTone(buzzer);
        break ; 


      


      case 4:
        lcd.setCursor(0, 0);
        //lcd.print("Fire!!!");
        lcd.send_string("Fire!!!");
       
        lcd.setCursor(0, 1);
        //lcd.print("Check kitchen!");
        lcd.send_string("Check kitchen!");
        tone(buzzer,NOTE_DS6, noteDuration);
        delay(pauseBetweenNotes);
        noTone(buzzer);        
        break ; 
     
      case 5:
        lcd.setCursor(0, 0);
        //lcd.print("Water leak!");
        lcd.send_string("Water leak!");
       
        lcd.setCursor(0, 1);
        //lcd.print("Check kitchen!");
        lcd.send_string(second_line.c_str());
        tone(buzzer,NOTE_DS6, noteDuration);


        delay(pauseBetweenNotes);


        noTone(buzzer);    
        break ; 


      case 6:
        lcd.setCursor(0, 0);
        //lcd.print("Water leak!");
        lcd.send_string("Device problem");
       
        lcd.setCursor(0, 1);
        //lcd.print("Check kitchen!");
        lcd.send_string("Contact Us!");  
        break ; 




    }
   
    delay(1000);
  
  }

  lcd.clear() ; 


}

