// https://randomnerdtutorials.com/esp-now-one-to-many-esp32-esp8266/

#include <WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop()
{
}