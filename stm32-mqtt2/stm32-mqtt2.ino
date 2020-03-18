/*
 * Programa para teste do protocolo mqtt utilizando o STM32 e o ENC28J60
 * Uma variável é armazenada na memória flash
 * Autor: Eng. Fabrício de Lima Ribeiro
 * Data: 18/03/2020
 * 

ENC28J60  STM32F103
SCK        PA5
SO         PA6
SI         PA7
CS         PA4
RST        R
 
VCC - 5V
GND - GND

 * 
 * Para testar:
 * ~$ mosquitto_sub -t "#" -v
 */

#include <SPI.h>1
#include <UIPEthernet.h>
#include "PubSubClient.h"
#include <EEPROM.h>

#define NET_ENC28J60_EIR          0x1C
#define NET_ENC28J60_ESTAT        0x1D
#define NET_ENC28J60_ECON1        0x1F
#define NET_ENC28J60_EIR_RXERIF   0x01
#define NET_ENC28J60_ESTAT_BUFFER 0x40
#define NET_ENC28J60_ECON1_RXEN   0x04
#define NET_ENC28J60_CHECK_PERIOD 5000UL

#define DHCP

#define CLIENT_ID       "UNO"
#define PUBLISH_DELAY   3000

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

#ifndef DHCP
IPAddress   ipdns(192, 168,   1, 1);
IPAddress      ip(192, 168,   1, 7);//IP fixo
IPAddress gateway(192, 168,   1, 1);
IPAddress  subnet(255, 255, 255, 0);
#endif

EthernetClient ethClient;
PubSubClient mqttClient;

long previousMillis;

int addr = 0;
int val;

void setup() 
{

#ifdef  DHCP
  Ethernet.begin(mac);
#else
  Ethernet.begin(mac, ip, ipdns, gateway, subnet);
#endif
  
  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer("192.168.1.100", 1883);
  previousMillis = millis();
}

void loop() 
{
  if (millis() - previousMillis > PUBLISH_DELAY) 
  {
    sendData();
    previousMillis = millis();
  }

  mqttClient.loop();
}

void sendData() 
{
  char msgBuffer[7];
  if (mqttClient.connect(CLIENT_ID)) 
  {
    val = EEPROM.read(addr);
    mqttClient.publish("/teste/", "Ok!");
    mqttClient.publish("/contagem/", deblank(dtostrf(val, 3, 0, msgBuffer)));
    val++;
    EEPROM.write(addr, val);
  }
}

char * deblank(char *str)
{
  char *out = str;
  char *put = str;

  for (; *str != '\0'; ++str) {

    if (*str != ' ') {
      *put++ = *str;
    }
  }
  *put = '\0';
  return out;
}
