#include <SPI.h>
#include <Ethernet.h>

int tempPin = A5;
int ldrPin = A0;
int pirPin = 2;                 // PIR Out pin

int pirStatus = 0;
int ldrStatus = 0;
double tmpStatus = -1001.0;

unsigned long poslednjeMerenjeTMP = 0;
unsigned long poslednjeMerenjePIR = 0;

EthernetServer server(80);

void setup()
{
    Serial.begin(9600);
  while (!Serial) ;
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  /*uint8_t _ws_pin = 10;
  uint8_t _cs_pin = 53;
  pinMode(_cs_pin, OUTPUT);
  pinMode(_ws_pin, OUTPUT);
  digitalWrite(_ws_pin, HIGH);
  */
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
  IPAddress ip(192, 168, 1, 177);
  //byte ip[] = {192, 168, 56, 177};
  //byte gateway[] = {192, 168, 0, 1};
  //byte subnet[] = {255, 255, 255,0};
  

  
  Serial.println("Ethernet WebServer Example");
  
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());



    pinMode(ldrPin, INPUT);
  pinMode(pirPin, INPUT); 
  pinMode(tempPin, INPUT);

}

void temperatura()
{
  unsigned long sad = millis();
  
  if (poslednjeMerenjeTMP == 0 || sad - poslednjeMerenjeTMP > 60000)
  {
    poslednjeMerenjeTMP = sad;

    // Merenje trenutne temperature
    double trenutnaTemperatura = 0;
  
    for (byte i = 0; i < 30; ++i)
    {
      trenutnaTemperatura += analogRead(tempPin);
      delay(1);
    }
  
    trenutnaTemperatura /= (30.0 * 1.246);

    // Postavljanje nove temperature
    if (tmpStatus < -1000.0 || abs(trenutnaTemperatura - tmpStatus) <= 3.0)
    {
      tmpStatus = trenutnaTemperatura;
    }
  }
}

void pokret()
{
  unsigned long sad = millis();
  
  byte percentage = 0;
  for (byte i = 0; i < 100; ++i)
  {
    percentage += digitalRead(pirPin);
    delay(1);
  }

  if (!pirStatus && percentage >= 75)
  {
    pirStatus = 1;
    poslednjeMerenjePIR = sad;
  }
  else if (pirStatus && percentage <= 10 && sad - poslednjeMerenjePIR >= 1000)
  {
    pirStatus = 0;
  }
}

void izmeri()
{
  ldrStatus = analogRead(ldrPin);
  
  pokret();
  
  temperatura();
}

void loop()
{
  izmeri();
  
  if (Serial.read() == 't')
  {
   Serial.print("PIR: "); Serial.println(pirStatus);
   Serial.print("LDR: "); Serial.println(ldrStatus);
   Serial.print("TMP: "); Serial.println(tmpStatus);
  }
  
  EthernetClient client = server.available();
  if (client)
  {
    boolean currentLineIsBlank = true;
    
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        //Serial.write(c);
        
        if (c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/xml");
          client.println("Connection: close");
          client.println();
          client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
          client.println("<Senzori>");
          client.println("<Toplotni>");
          client.println(tmpStatus);
          client.println("</Toplotni>");
          client.println("<Pokreta>");
          client.println(pirStatus);
          client.println("</Pokreta>");
          client.println("<Svetlosti>");
          client.println(ldrStatus);
          client.println("</Svetlosti>");
          client.println("</Senzori>");
          break;
        }
        
        if (c == '\n') currentLineIsBlank = true;
        else if (c != '\r') currentLineIsBlank = false;
      }
    }
    
    delay(1);
    client.stop();
  }
}
