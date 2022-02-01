#include <RH_ASK.h> //include radiohead amplitude shift keying library
#include <SPI.h> //include dependant spi library

//libraries for bme
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

//create amplitude shift keying object
RH_ASK rf_driver;

//Weather Vane
//8 pins
//Convert input from binary to output
const int wv1 = 13;
const int wv2 = 12;
const int wv3 = 11;
const int wv4 = 10;
const int wv5 = 9;
const int wv6 = 8;
const int wv7 = 7;
const int wv8 = 6;

//Anemometer
//1 pin
//Reads counter for triggered pin for 1 minute

//Converts into mph
const int anem = 5;

//Transmitter
//1 pin. Data
const int Tx = 4;

//BME280 Sensor
//Connect SCK to I2C clock pin. Input to chip. Connect to micro clock line
//SDO Serial Data Out. Sensor to micro
#define BME_SCK 1
#define BME_MISO 3
#define SEALEVELPRESSURE_HPA (1013.25)

//SGP30 Sensor
//Connect SCL to BME SCK. Clock pin
//SDA Serial Data Out. Sensor to micro
const int sgpData = 2;


void setup() { //runs once upon start

  //weathervane pins all input
  pinMode(wv1, INPUT);
  pinMode(wv2, INPUT);
  pinMode(wv3, INPUT);
  pinMode(wv4, INPUT);
  pinMode(wv5, INPUT);
  pinMode(wv6, INPUT);
  pinMode(wv7, INPUT);
  pinMode(wv8, INPUT);

  //anemometer pin as input
  pinMode(anem, INPUT);

  //transmitter as output
  pinMode(Tx, OUTPUT);

  pinMode(sgpData, INPUT); //sgp as input

  pinMode(LED_BUILTIN, OUTPUT); //initialize digital pin LED_BUILTIN as an output.

  //Initialize ASK Object
  rf_driver.init();
  
}


void loop() { //runs repeatedly

  const char *msg = "STEAM Weather Station";
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
  delay(1000);
  

  if(!bme.begin()) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);                       // wait for a second
      while (1);
  }



}
