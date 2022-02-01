#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver;

void setup() {
  Serial.begin(9600);
  if (!driver.init())
      Serial.println("init failed");

}

void loop() {
  uint8_t buf[21];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) //Non-blocking
  {
    int i; //msg with a good checksum receiver, dump it.
    Serial.print("Message: ");
    Serial.println((char*)buf);

    
  }

}
