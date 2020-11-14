// https://randomnerdtutorials.com/esp-now-one-to-many-esp32-esp8266/

// #include <WiFi.h>

// 1 = AC:67:B2:2C:C6:60
// 2 = AC:67:B2:2C:B7:3C
// 3 = AC:67:B2:2C:80:FC
// 4 = AC:67:B2:2C:76:E0

// void setup()
// {
//   Serial.begin(9600);
//   while (Serial.available())
//   {
//   }

//   Serial.println();
//   Serial.print("ESP Board MAC Address:  ");
//   Serial.println(WiFi.macAddress());
// }

// void loop()
// {
//   Serial.println(WiFi.macAddress());
// }

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

typedef struct data_struct
{
  float acc = 0;
  bool reset = false;
  float speed = 1;
} data_struct;
data_struct dataPayload;

#define IS_SENDER // COMMEN SET AS RECEIVER

#ifdef IS_SENDER

#include <esp_now.h>
#include <WiFi.h>
#include "Encoder.h"

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0xAC, 0x67, 0xB2, 0x2C, 0xB7, 0x3C}; // 2
uint8_t broadcastAddress2[] = {0xAC, 0x67, 0xB2, 0x2C, 0x80, 0xFC}; // 3
// uint8_t broadcastAddress3[] = {0xAC, 0x67, 0xB2, 0x2C, 0x76, 0xE0}; // 4

// callback when data is sent
// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
// {
// char macStr[18];
// Serial.print("Packet to: ");
// Copies the sender mac address to a string
// snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
//          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
// Serial.print(macStr);
// Serial.print(" send status:\t");
// Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

void setup()
{
  Serial.begin(115200);

  encoder->begin();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // esp_now_register_send_cb(OnDataSent);

  // register peer
  esp_now_peer_info_t peerInfo;
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  // register second peer
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  // /// register third peer
  // memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  // if (esp_now_add_peer(&peerInfo) != ESP_OK)
  // {
  //   Serial.println("Failed to add peer");
  //   return;
  // }
}

void loop()
{

  if (millis() < 1000)
  {
    dataPayload.reset = true;
    esp_now_send(0, (uint8_t *)&dataPayload, sizeof(data_struct));
    return;
  }
  else
  {
    dataPayload.reset = false;
  }

  if (encoder->update())
  {
    dataPayload.acc = encoder->accFiltered;
    dataPayload.speed = encoder->accSpeed;
    esp_now_send(0, (uint8_t *)&dataPayload, sizeof(data_struct));

    // Serial.print("speed ");
    // Serial.println(dataPayload.speed);
  };

  // float t = millis() / 1000.0f;
  // dataPayload.x = sinf(t * 0.1f) * 2000000000;
  // esp_now_send(0, (uint8_t *)&dataPayload, sizeof(data_struct));

  // Serial.print("data :: ");
  // Serial.println(dataPayload.acc);
  // Serial.println(dataPayload.reset);
  // Serial.println(dataPayload.speed);

  delay(16);
}

#else

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

#include <PacketSerial.h>
#define PACKET_SIZE 16
PacketSerial_<COBS, 0, PACKET_SIZE> packetSerial;

//Create a struct_message called myData
data_struct myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.acc);
  Serial.println(myData.speed);
  Serial.println(myData.reset);
  Serial.println("-------------");

  packetSerial.send((uint8_t *)&myData, sizeof(data_struct));
}

void setup()
{
  //Initialize Serial Monitor
  Serial.begin(115200);
  Serial2.begin(115200);
  packetSerial.begin(115200);
  packetSerial.setStream(&Serial2);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  // if (millis() > 4000)
  // {
  //   packetSerial.send((uint8_t *)&myData, sizeof(data_struct));
  //   delay(16);
  // }
}

#endif