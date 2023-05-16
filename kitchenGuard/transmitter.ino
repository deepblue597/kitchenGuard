#include "pitch.h"
#include <LiquidCrystal.h> 
#include "Waveshare_LCD1602_RGB.h"
#include "DHT.h"

// Defintions for the humidity sensor 
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dhtsensors(DHTPIN, DHTTYPE);

/* variables for the LCD screen */
// int Contrast=75;
// LiquidCrystal lcd(8, 9, 5, 4, 3, 7);  

Waveshare_LCD1602_RGB lcd(16,2);  //16 characters and 2 lines of show
int r,g,b,t=0;


//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
int noteDuration = 1000 / 4 ; 

// to distinguish the notes, set a minimum time between them.

// the note's duration + 30% seems to work well:
int pauseBetweenNotes = noteDuration * 1.30;

const int temperature_sensor = A1;  /* LM35 O/P pin */
const int force_sensor = A0 ; //force sensor
const int gas_sensor = A5 ; // gas sensor
const int lcd_output = 6 ; // LCD display 
const int flame_sensor = A2; // flame sensor 

int force_value ; // values of force sensor
float gasValue ;  // values of gas sensor 

//for flame 
const int sensorMin = 0;     //  sensor minimum
const int sensorMax = 1024;  // sensor maximum

void setup() {
  Serial.begin(9600);

  // analogWrite(lcd_output,Contrast);
  // lcd.begin(16, 2);

      // initialize
  lcd.init();
    
  lcd.setCursor(0,0);
  lcd.send_string("good morning");
  lcd.setCursor(0,1);
  lcd.send_string("Jason");

  // begin the humidity sensor 
  dhtsensors.begin(); 

  Serial.println("MQ2 warming up!");
  delay(20000); // allow the MQ2 (gas sensor) to warm up 
  
}

void loop() {

  /* we have 6 statuses 
     0 --> Everything ok 
     1 --> Water boiling with pot 
     2 --> Water boiling without pot
     3 --> Gas leak 
     4 --> Fire 
     5 --> Water leak   
     These will be used to send the correct case to our receiver */
  int status = 0 ; 
  bool issue = false ; 
  
  int temp_adc_val;
  float temp_val;
  temp_adc_val = analogRead(temperature_sensor);  /* Read Temperature */
  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */

  /* Priting the temperature values  */
  Serial.print("Temperature = ");
  Serial.print(temp_val);
  Serial.print(" Degree Celsius\n");

  //force sensor 
  force_value = analogRead(force_sensor);  // read the input on analog pin 0
  Serial.print("Force = ");
  Serial.println(force_value); // print out the value you read

  gasValue = analogRead(gas_sensor); // read analog input pin 0

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  int flame_value  = analogRead(flame_sensor); // read the analog input for flame sensor 
  //bool flame_bool = map(flame_value,  sensorMin, sensorMax, true, false); // CHECK IF THIS IS WORKING PROPERLY
  Serial.println("fire") ; 
  Serial.println(flame_value);

  // read humidity and print it 
  float humi  = dhtsensors.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(humi);
  Serial.print("%");

  if( temp_val > 30 ) // Water Boiling, Pot placement  (TODO with more sensors : Stove )
  {
    issue = true ; 
    if( force_value > 600) // Pot on place
    {
      status = 1 ; 
      lcd.setCursor(0, 0);
      lcd.send_string("Water Boiling");
      //lcd.print("Water Boiling");
   
      lcd.setCursor(0, 1);
      //lcd.print("Pot on place");
      lcd.send_string("Pot on place");
    }
    else // Pot not on place  
    { 
      status = 2 ; 
      lcd.setCursor(0, 0);
      //lcd.print("Water Boiling");
      lcd.send_string("Water Boiling");
   
      lcd.setCursor(0, 1);
      //lcd.print("Pot missing");
      lcd.send_string("Pot missing");

      tone(8,NOTE_B7, noteDuration); // play a tone for prefined time 

      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
      
    }
  
  }


  if( gasValue > 700 && issue == false ) { // Gas leak  TODO with specific gas sensors ( e.g CO ) + temp : kitchen conditions 
    issue = true ; 
    status = 3 ; 
    lcd.setCursor(0, 0);
    //lcd.print("Gas leak!!!");
    lcd.send_string("Gas leak!!!");
   
    lcd.setCursor(0, 1);
    //lcd.print("Check kitchen!");
    lcd.send_string("Check kitchen!");

    tone(8,NOTE_DS8, noteDuration); 

    delay(pauseBetweenNotes);
 
    noTone(8);

  }

  if ( flame_value < 500 && issue == false) {  // Fire 
    issue = true ; 
    status = 4 ; 
    lcd.setCursor(0, 0);
    //lcd.print("Fire!!!");
    lcd.send_string("Fire!!!");
   
    lcd.setCursor(0, 1);
    //lcd.print("Check kitchen!");
    lcd.send_string("Check kitchen!");

    tone(8,NOTE_DS6, noteDuration); 

    delay(pauseBetweenNotes);
 
    noTone(8);


  }

  // TODO : humidity sensor for water leak and food ready 
  if( humi > 50 && issue == false ){ 
    issue = true ; 
    status = 5  ; 
    lcd.setCursor(0, 0);
    //lcd.print("Water leak!");
    lcd.send_string("Water leak!");
   
    lcd.setCursor(0, 1);
    //lcd.print("Check kitchen!");
    lcd.send_string("Check kitchen!");

    tone(8,NOTE_DS6, noteDuration); 

    delay(pauseBetweenNotes);
 
    noTone(8);
  }


  if( issue == false) { 
      lcd.clear() ; 
      lcd.setCursor(0, 0);
      //lcd.print("All good :)");
      lcd.send_string("All good :)");

  }

  delay(1000);
  

}
