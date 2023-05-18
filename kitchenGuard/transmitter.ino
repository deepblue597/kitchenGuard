//#include "pitch.h"
//#include "LiquidCrystal.h"
//#include "Waveshare_LCD1602_RGB.h"
#include "DHT.h"
#include <RF22.h>
#include <RF22Router.h>
#define MY_ADDRESS 12
#define DESTINATION_ADDRESS 5
RF22Router rf22(MY_ADDRESS);
//
//// Defintions for the humidity sensor
#define DHTPIN 7
#define DHTTYPE DHT11


DHT dhtsensors(DHTPIN, DHTTYPE);


/* variables for the LCD screen */
//int Contrast=75;
//LiquidCrystal lcd(8, 9, 5, 4, 3, 7);  


//Waveshare_LCD1602_RGB lcd(16,2);  //16 characters and 2 lines of show
//int r,g,b,t=0;


float temp_value_2;




boolean successful_packet = false;
// to distinguish the notes, set a minimum time between them.







const int force_sensor = A1 ; //force sensor
const int gas_sensor = A5 ; // gas sensor
const int lcd_output = 6 ; // LCD display
const int flame_sensor = A2; // flame sensor


int force_value ; // values of force sensor
float gasValue ;  // values of gas sensor


//for flame
const int sensorMin = 0;     //  sensor minimum
const int sensorMax = 1024;  // sensor maximum


int sensor_value = 0 ; //the value of the sensor we are going to send


void setup() {
  Serial.begin(9600);
  if (!rf22.init())
    Serial.println("RF22 init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.setFrequency(436.0))
    Serial.println("setFrequency Fail");
  rf22.setTxPower(RF22_TXPOW_20DBM);
  //1,2,5,8,11,14,17,20 DBM
  //rf22.setModemConfig(RF22::OOK_Rb40Bw335  );
  rf22.setModemConfig(RF22::GFSK_Rb125Fd125);
  //modulation


  // Manually define the routes for this network
  rf22.addRouteTo(DESTINATION_ADDRESS, DESTINATION_ADDRESS);
  // analogWrite(lcd_output,Contrast);


  // begin the humidity sensor
  dhtsensors.begin();
  Serial.println("MQ2 warming up!");
  //delay(20000); // allow the MQ2 (gas sensor) to warm up
 
}


void loop() {


  /* we have 7 statuses
     0 --> Everything ok
     1 --> Water boiling with pot
     2 --> Water boiling without pot
     3 --> Gas leak
     4 --> Fire
     5 --> Water leak  
     6 --> Issue with device
     These will be used to send the correct case to our receiver */
  int status = 6 ;
  bool issue = false ;
 Serial.println("begin") ; 
 Serial.println(sensor_value) ; 
  // int temp_adc_val;
  // int temp_val;
  int  temp_value_2 = dhtsensors.readTemperature(); /* Read Temperature */
  Serial.print("Temperature: ");
  Serial.print(temp_value_2);
  Serial.print("°C ~ ");
  //  temp_val = (temp_adc_val * 4.88); /* Convert adc value to equivalent voltage */
  //  temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */
  //
  //  /* Priting the temperature values  */
  // Serial.print("Temperature = ");
  // Serial.print(temp_val);
  // Serial.print(" Degree Celsius\n");
 
  //   force sensor
  force_value = analogRead(force_sensor);  // read the input on analog pin 0
  Serial.print("Force = ");
  Serial.println(force_value); // print out the value you read


  gasValue = analogRead(gas_sensor); // read analog input pin 0


  Serial.print("Gas Value: ");
  Serial.println(gasValue);


  int flame_value  = analogRead(flame_sensor); // read the analog input for flame sensor
  int range = map(flame_value,  sensorMin, sensorMax, 0, 3);
 
  // range value:
  switch (range) {
  case 0:    // A fire closer than 1.5 feet away.
    Serial.println("** Close  Fire **");
    break;
  case 1:    // A fire between 1-3 feet away.
    Serial.println("**  Distant Fire **");
    break;
  case 2:    // No fire detected.
    Serial.println("No  Fire");
    break;
  }
//  Serial.println("flame:") ; 
//  Serial.println(flame_sensor) ; 
   // read humidity and print it
  int humi  = dhtsensors.readHumidity();
  Serial.print("Humidity: ");
  Serial.print(humi);
  Serial.print("%");
 
  //
  if( temp_value_2 > 26 ) // Water Boiling, Pot placement  (TODO with more sensors : Stove )
  {
    sensor_value = temp_value_2 ;
    //issue = true ;
    if( force_value > 600) // Pot on place
    {
      status = 1 ;


    }
    else // Pot not on place  
    {
      status = 2 ;


    }
 
  }
  else if( gasValue > 500) { // Gas leak  TODO with specific gas sensors ( e.g CO ) + temp : kitchen conditions
    //issue = true ;
    
    sensor_value = gasValue ;
    status = 3 ;


  }
  else if ( range == 0 ) {  // Fire
    //issue = true ;
    sensor_value = 000 ;
    status = 4 ;
   
  }


  else if( humi > 60 ){


    sensor_value = humi ;
    status = 5  ;
  }
  else {
      status = 0 ;
      sensor_value = 0 ;


  }

  Serial.println("status is") ; 
  Serial.println(status) ; 
  Serial.println("sensors value is") ; 
  Serial.println(sensor_value) ; 
  // send the values to receiver
  char status_read[RF22_ROUTER_MAX_MESSAGE_LEN];
  uint8_t status_send[RF22_ROUTER_MAX_MESSAGE_LEN];
  memset(status_read, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  memset(status_send, '\0', RF22_ROUTER_MAX_MESSAGE_LEN);
  if (sensor_value>100)
  {
  sprintf(status_read, "%d %d", status, sensor_value);
  }
  else
   {
  sprintf(status_read, "%d 0%d", status, sensor_value);
  }
  status_read[RF22_ROUTER_MAX_MESSAGE_LEN - 1] = '\0';
  memcpy(status_send, status_read, RF22_ROUTER_MAX_MESSAGE_LEN);
  successful_packet = false;
  Serial.println(status_read);
  while (!successful_packet)
  {


    if (rf22.sendtoWait(status_send, sizeof(status_send), DESTINATION_ADDRESS) != RF22_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
    }
    else
    {
      successful_packet = true;
     // Serial.println("sendtoWait Succesful");
    }
  }


  delay(1000);
 
}
