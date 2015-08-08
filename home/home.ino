#include <SPI.h>
#include <Ethernet.h>
#include <dht11.h> // dht11 kütüphanesini ekliyoruz.

#define DHT11PIN 2 // DHT11PIN olarak Dijital 2'yi belirliyoruz.

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 0, 250);
EthernetServer server(80);
dht11 DHT11;

void setup() {

  Serial.begin(9600);

  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {

  EthernetClient client = server.available();
  if (client) {
    
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        if (c == '\n' && currentLineIsBlank) {

          String tempInfo = GetTemperatureInfo();
          
          Serial.println(tempInfo);
          client.print(tempInfo);
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

String GetTemperatureInfo()
{
  String info = "";
  
  float _humidity = (float)DHT11.humidity;
  float _temperature = (float)DHT11.temperature;
  float _fahrenheit = (float)DHT11.fahrenheit();
  float _kelvin = (float)DHT11.kelvin();

  info += "Nem : %" + String(_humidity);
  info += "\nSicaklik : " + String(_temperature)+" C";
  info += "\nFahrenheit : " + String(_fahrenheit)+"F";

  return info;
}


