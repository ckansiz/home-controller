#include <SPI.h>
#include <Ethernet.h>
#include <dht11.h>
#include <ArduinoJson.h>

#define DHT11PIN 2

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 0, 250);
EthernetServer server(80);
dht11 DHT11;

bool readRequest(EthernetClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      if (c == '\n' && currentLineIsBlank) {
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  return false;
}

JsonObject& getTempInfo(JsonBuffer& jsonBuffer)
{
  JsonObject& info = jsonBuffer.createObject();
  
  int chk = DHT11.read(DHT11PIN);
  info["humidity"] =(float)DHT11.humidity; 
  info["temperature"] = (float)DHT11.temperature;
  info["fahrenheit"] = (float)DHT11.fahrenheit();
  info["kelvin"] = (float)DHT11.kelvin();

  return info;
}

void writeResponse(EthernetClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  
  json.prettyPrintTo(client);
}

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
    bool success = readRequest(client);
    if (success) {
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = getTempInfo(jsonBuffer);
      writeResponse(client, json);
    }
    delay(1);
    client.stop();
  }
}


