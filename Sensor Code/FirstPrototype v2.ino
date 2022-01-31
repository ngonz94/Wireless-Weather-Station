//libraries for sensors
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"

//DHT Sensor
#define DHTPIN 11       //TODO: Change to pin d2 for second prototype
#define DHTTYPE DHT22
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
const int wv3 = 5;
const int wv4 = 10;
const int wv5 = 9;
const int wv6 = 8;
const int wv7 = 7;
const int wv8 = 6;

//Assigns weather vane's pins to low
int b1 = HIGH;
int b2 = HIGH;
int b3 = HIGH;
int b4 = HIGH;
int b5 = HIGH;
int b6 = HIGH;
int b7 = HIGH;
int b8 = HIGH;

//sets default direction of weathervane to North
int dir = 0;

//Anemometer
const byte anemPin = 3;
const int ANEM_DELAY = 1000; //amount of loops before anem count is reset
int spinCount = 0;
//maybe change to uint
//maybe change to int
int loopCount = 0;
int anemReading = 0;

unsigned long interrupt_time = millis();


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
  attachInterrupt(digitalPinToInterrupt(anemPin), wind, FALLING); //goes to wind function everytime anemPin goes Low -> High

  //SGP30
  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!

  dht.begin();

  Serial.begin(9600);

  if (! sgp.begin()) {
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

}


void loop() { //runs repeatedly

  if (loopCount++ == ANEM_DELAY ) { //change ANEM_DELAY based on time frame readings are taken. Make bigger if readings are 0. Numbers are too big, make smaller.
    loopCount = 0;
    anemReading = spinCount;
    spinCount = 0;

    Serial.print("uhhh");
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
  //  b4 = 1;
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
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  //tx weather vane reading
  //  const char msg;
  Serial.println("WV: ");
  Serial.println(dir);
  Serial.println(" ");


  //transmit anemReading
  Serial.println("Anemometer:");
  Serial.print(spinCount);
  Serial.println(" ");


  // tvocs & eco2
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.println("TVOC "); Serial.println(sgp.TVOC); Serial.println(" ");
  Serial.println("eCO2 "); Serial.println(sgp.eCO2); Serial.println(" ");

  // h2 & ethanol
  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }
  Serial.println("Raw H2 "); Serial.println(sgp.rawH2); Serial.println(" ");
  Serial.println("Raw Ethanol "); Serial.println(sgp.rawEthanol); Serial.println("");


  //tx temp celsius
  //TODO test this with tx and rx
  //TODO tx heat index if we want it
  Serial.println("Celsius: ");
  Serial.println(t);
  Serial.println(" ");

  //tx humidity
  Serial.println("Humidity: ");
  Serial.println(h);
  Serial.println(" ");

  //tx temp fahrenheit
  Serial.println("Fahrenheit: ");
  Serial.println(f);
  Serial.println(" ");


}

unsigned long last_interrupt_time = 0;


void wind() {

  spinCount++;

  //  interrupt_time = millis();
  //  //
  //    if (interrupt_time - last_interrupt_time > 100) {
  //
  //      Serial.println(last_interrupt_time);
  //      Serial.println(spinCount);
  //
  //    }
  //
  //    last_interrupt_time = interrupt_time;

}
