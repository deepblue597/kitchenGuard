#include "pitch.h"
#include <LiquidCrystal.h> 

/* variables for the LCD screen */
int Contrast=75;
LiquidCrystal lcd(8, 9, 5, 4, 3, 7);  


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

  analogWrite(lcd_output,Contrast);
  lcd.begin(16, 2);

  Serial.println("MQ2 warming up!");
  delay(20000); // allow the MQ2 (gas sensor) to warm up 
}

void loop() {


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
  Serial.println(force_value); // print out the value you read

  gasValue = analogRead(gas_sensor); // read analog input pin 0

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  int flame_value  = analogRead(flame_sensor); // read the analo input for flame sensor 
  int flame_bool = map(flame_value,  sensorMin, sensorMax, 0, 1); // CHECK IF THIS IS WORKING PROPERLY
  Serial.println(flame_value);



  // LCD screen output 
  lcd.setCursor(0, 0);
  lcd.print("Only Innovative");
   
  lcd.setCursor(0, 1);
  lcd.print("Subscribe");


  if( temp_val > 90 ) // Water Boiling, Pot placement  (TODO with more sensors : Stove )
  {
    if( force_value > 600) // Pot on place
    {
      lcd.setCursor(0, 0);
      lcd.print("Water Boiling");
   
      lcd.setCursor(0, 1);
      lcd.print("Pot on place");
    }
    else // Pot not on place  
    { 
      lcd.setCursor(0, 0);
      lcd.print("Water Boiling");
   
      lcd.setCursor(0, 1);
      lcd.print("Pot missing");

      tone(8,NOTE_G2, noteDuration); // play a tone for prefined time 

      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(8);
      
    }
  
  }

  if( gasValue > 700 ) { // Gas leak  TODO with specific gas sensors ( e.g CO ) + temp : kitchen conditions 
  
    lcd.setCursor(0, 0);
    lcd.print("Gas leak!!!");
   
    lcd.setCursor(0, 1);
    lcd.print("Check kitchen!");

    tone(8,NOTE_DS8, noteDuration); 

    delay(pauseBetweenNotes);
 
    noTone(8);

  }

  if ( flame_bool > 0.5 ) {  // Fire 

    lcd.setCursor(0, 0);
    lcd.print("Fire!!!");
   
    lcd.setCursor(0, 1);
    lcd.print("Check kitchen!");

    tone(8,NOTE_DS5, noteDuration); 

    delay(pauseBetweenNotes);
 
    noTone(8);


  }

  // TODO : humidity sensor for water leak and food ready 





  delay(1000);
  

}