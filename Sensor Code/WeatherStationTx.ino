//Tx
#include <RH_ASK.h> //include radiohead amplitude shift keying library
#include <SPI.h> //include dependant spi library
RH_ASK rf_driver; //create amplitude shift keying object

//libraries for sensors
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"

//DHT Sensor
//TODO change pin accordingly
//TODO check wiring instructions on dht test
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); //starts it

//SGP30
Adafruit_SGP30 sgp; //start it
/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [°C]
  @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

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

//Assigns weather vane's pins to low
int b1 = LOW;
int b2 = LOW;
int b3 = LOW;
int b4 = LOW;
int b5 = LOW;
int b6 = LOW;
int b7 = LOW;
int b8 = LOW;

//sets default direction of weathervane to North
int dir = 0;

//Anemometer
const byte anemPin = 3;
const int ANEM_DELAY = 60000; //amount of loops before anem count is reset
volatile int spinCount = 0;
//maybe change to uint
//maybe change to int
volatile int loopCount = 0;
volatile int anemReading = 0;

//DHT11



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
  pinMode(anemPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(anemPin), wind, RISING); //goes to wind function everytime anemPin goes Low -> High

  pinMode(LED_BUILTIN, OUTPUT); //initialize digital pin LED_BUILTIN as an output.

  //Initialize ASK Object
  rf_driver.init();

  //SGP30
  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!

}


void loop() { //runs repeatedly

  if (loopCount++ == ANEM_DELAY ) { //change ANEM_DELAY based on time frame readings are taken. Make bigger if readings are 0. Numbers are too big, make smaller.
    loopCount = 0;
    anemReading = spinCount;
    spinCount = 0;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }


  //read WV pins
  //may need to make them opposite with the ! sign
  b1 = digitalRead(wv1);
  b2 = digitalRead(wv2);
  b3 = digitalRead(wv3);
  b4 = digitalRead(wv4);
  b5 = digitalRead(wv5);
  b6 = digitalRead(wv6);
  b7 = digitalRead(wv7);
  b8 = digitalRead(wv8);

  //you can assign b1 - b8 a desired value for an output.
  //  b1 = 1;
  //  b2 = 0;
  //  b3 = 1;
  //  b4 = 0;
  //  b5 = 0;
  //  b6 = 0;
  //  b7 = 0;
  //  b8 = 0;

  uint8_t dir = b1 * 1 + b2 * 2 + b3 * 4 + b4 * 8 + b5 * 16 + b6 * 32 + b7 * 64 + b8 * 128; //converts values on bits into int with binary conversion
  //  uint8_t msg[1] = { dir } ; //msg is value of dir
  

  //dht readings
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  //TODO: Do we need heat index?
  //  // Compute heat index in Fahrenheit (the default)
  //  float hif = dht.computeHeatIndex(f, h);
  //  // Compute heat index in Celsius (isFahreheit = false)
  //  float hic = dht.computeHeatIndex(t, h, false);
  

  //tx weather vane reading
  const char msg;
  itoa(dir, msg, 10);
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent();                  //wait until rx receives
  delay(1000);                                 //wait 1 sec


  //transmit anemReading
  itoa(anemReading, msg, 10);
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec


  //tx tvocs
  //TODO test this with tx and rx
  itoa(sgp.TVOC, msg, 10);      //may need to set sgp.TVOC to int
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  //tx eco2
  itoa(sgp.eCO2, msg, 10);    //may need to set sgp.eCO2 to int
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  //tx rawH2
  itoa(sgp.rawH2, msg, 10);    //may need to set sgp.rawh2 to int
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  //tx rawEthanol
  itoa(sgp.rawEthanol, msg, 10);    //may need to set sgp.rawethanol to int
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec


  //tx temp celsius
  //TODO test this with tx and rx
  //TODO tx heat index if we want it
  itoa(t, msg, 10);        
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  //tx humidity
  itoa(h, msg, 10);      
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  //tx temp fahrenheit
  itoa(f, msg, 10);      
  rf_driver.send((uint8_t *)msg, strlen(msg)); //send msg as a string
  rf_driver.waitPacketSent(); //wait until rx receives
  delay(1000); //wait 1 sec

  loopCount++;

}

void wind() {

  spinCount++;

}
