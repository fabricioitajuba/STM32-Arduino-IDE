/*
Webserver com o STM32 e ENC28J60
Este exemplo liga e desliga o led da placa blue pill utilizando uma página http
Autor: Eng. Fabrício de Lima Ribeiro
Data: 19/03/2020
 
ENC28J60  STM32F103
SCK        PA5
SO         PA6
SI         PA7
CS         PA4
RST        PA0
 
VCC - 5V
GND - GND
 
*/
 
#include <libmaple/iwdg.h>
#include <SPI.h>
#include <UIPEthernet.h>
 
#define NET_ENC28J60_EIR          0x1C
#define NET_ENC28J60_ESTAT        0x1D
#define NET_ENC28J60_ECON1        0x1F
#define NET_ENC28J60_EIR_RXERIF   0x01
#define NET_ENC28J60_ESTAT_BUFFER 0x40
#define NET_ENC28J60_ECON1_RXEN   0x04
#define NET_ENC28J60_CHECK_PERIOD 5000UL
 
#define iwdg_init_ms(N) iwdg_init(IWDG_PRE_256,((N)/5))

#define DHCP

#define pinLED PC13
#define ETH_RS_PIN PA0
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

#ifndef DHCP
IPAddress   ipdns(192, 168,   1, 1);
IPAddress      ip(192, 168,   1, 7);//IP fixo
IPAddress gateway(192, 168,   1, 1);
IPAddress  subnet(255, 255, 255, 0);
#endif
 
EthernetServer server = EthernetServer(80);
 
String cdata = "";
unsigned long timer;
 
void setup()
{
  disableDebugPorts();

  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED,HIGH);
  
  eth_reset();
  iwdg_init_ms(4000);
  timer = millis();
}
 
void eth_reset()
{ 
  pinMode(ETH_RS_PIN, OUTPUT);
  digitalWrite(ETH_RS_PIN, LOW);
  delay(100);
  digitalWrite(ETH_RS_PIN, HIGH);
  pinMode(ETH_RS_PIN, INPUT);

#ifdef  DHCP
  Ethernet.begin(mac);
#else
  Ethernet.begin(mac, ip, ipdns, gateway, subnet);
#endif

  server.begin();
}
 
void loop()
{
  iwdg_feed();
  Ethernet.maintain();
   
  if ((millis() - timer) > NET_ENC28J60_CHECK_PERIOD)
  {    
       uint8_t stateEconRxen = Enc28J60.readReg((uint8_t) NET_ENC28J60_ECON1) & NET_ENC28J60_ECON1_RXEN;
       uint8_t stateEstatBuffer = Enc28J60.readReg((uint8_t) NET_ENC28J60_ESTAT) & NET_ENC28J60_ESTAT_BUFFER;
       uint8_t stateEirRxerif = Enc28J60.readReg((uint8_t) NET_ENC28J60_EIR) & NET_ENC28J60_EIR_RXERIF;

       if (!stateEconRxen || (stateEstatBuffer && stateEirRxerif))
       { 
          //Enc28J60.init(netConfig->macAddress);/////////////////////////////////////
         eth_reset();
       }
    timer = millis();
  }
  
  EthernetClient client = server.available();
  
  if(client)
  {    
    cdata = "";

    while(client.connected())
    {
      if(client.available())
      {
        char c = client.read();
             
        cdata.concat(c);
        
        if(cdata.indexOf("\r\n\r\n") > 0)
        {
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close")); 

          client.println();
          client.println(F("<!DOCTYPE HTML>"));
          client.println(F("<html>"));
          client.println(F("<body>"));
          client.println(F("<H2>STM32F103C8T6 WEB Server</H2>"));

          client.println(F("<br> <a href='/led_on'><button>Ligar led</button></a>"));
          client.println(F("<br> <a href='/led_off'><button>Desligar led</button></a>"));

          if(cdata.indexOf("/led_on") > 0)
          {
            digitalWrite(pinLED,LOW);
            client.println(F("<br><br>Led Ligado!"));
            client.println(F("</body>"));
            client.println(F("</html>"));
            break;
          }

          if (cdata.indexOf("/led_off") > 0)
          {
            digitalWrite(pinLED,HIGH);
            client.println(F("<br><br>Led Desligado!"));
            client.println(F("</body>"));
            client.println(F("</html>"));
            break;
          }

          if(digitalRead(pinLED))
          {
            client.println(F("<br><br>Led Desligado!"));
          }
          else
          {
            client.println(F("<br><br>Led Ligado!"));
          }
                              
          client.println(F("</body>"));
          client.println(F("</html>"));
          break;
        }
      }
    }
    client.stop();
    timer = millis();
  }  
}
