#include <DHT.h>

#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>
#include "SoftwareSerial.h"
#include "ThingSpeak.h"

SoftwareSerial esp8266(2,3);

#define DHTPIN 4
#define DHTYPE DHT11
//#define TOKEN ""

#define DEBUG true

WiFiEspClient espClient;

#define WIFI_AP "Compt"
#define WIFI_PASSWORD "123456781"

unsigned long Canal = "2316146";
const char* Key = "4T4LBDNDHRD9VT9X";
String Status = "";


int status = WL_IDLE_STATUS;
unsigned long last_message;

char server [] = "192.168.137.1";

DHT dht (DHTPIN, DHTYPE);

//ubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  dht.begin();
  InitWiFi();

  ThingSpeak.begin(espClient);

  //client.setClient(espClient);
  //client.setServer("test.mosquitto.org", 1883);

  last_message = 0;

  String clientName="esp-cliente";
  Serial.print("conectando em ");
  Serial.print(server);
  Serial.print(" como ");
  Serial.println(clientName);
}

void loop() {
  // put your main code here, to run repeatedly:
  status = WiFi.status();

  if (status !=WL_CONNECTED){
    while (status != WL_CONNECTED) {
      Serial.print("TO TENTANDO CONECTAR: ");
      Serial.println(WIFI_AP);
      status =WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(50);
    }
    
  Serial.println("CONSEGUI CONECTAR AO:");
  Serial.print("ENDEREÇO DE IP: ");
  Serial.println(WiFi.localIP());
  }

  //if(!client.connected() ){
  //  reconnect();
 // }

  //client.loop();

  if (millis() - last_message > 1000) {
    getAndSendTemperatureAndHumidityData();
    last_message = millis();
  }
}

void getAndSendTemperatureAndHumidityData() {
  Serial.println("COLETANDO INFORMAÇÕES");

  float h = dht.readHumidity();

  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)){
    Serial.println("Pane no sistema, alguém me desconfigurou");
    return;
  }

  Serial.print("Umidade: ");
  Serial.print(h);
  Serial.print(" Temperatura: ");
  Serial.print(t);
  Serial.println("*C ");

  ThingSpeak.setField(1,t);
  ThingSpeak.setField(2,h);

  ThingSpeak.setStatus(Status);

  int x = ThingSpeak.writeFields(Canal,Key);
  if(x==200){
    Serial.println("Canal updatado sucessamenti");
  } else {
    Serial.println ("Problema updatando canal. HTTP erro" + String(x));
  }
  delay (15000);

  //String temperature = String(t);
  //String humidity = String(h);

  //Serial.print("Temperatura e Umidade: [");
  //Serial.print(temperature);
  //Serial.print(" , ");
  //Serial.print(humidity);
  //Serial.println("];");
  //Serial.println("------------------------------------------");

  //delay(5000);

  //String payload = "{";
  //payload += "\"temperature\":"; payload += temperature; payload;
  //payload += "\"humidity\":"; payload += humidity;
  //payload += "}";

  //send payload

  //char attributes[100];
  //payload.toCharArray(attributes, 100);
  //client.publish("teste", attributes);
  //Serial.println( attributes );
}

void InitWiFi() {
  esp8266.begin(19200);
  WiFi.init(&esp8266);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  Serial.println("Conectando ao AP");
  while (status != WL_CONNECTED) {
    Serial.print("Tentando conectar ao WPA SSID: ");
    Serial.println(WIFI_AP);
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  Serial.println("Conectado ao AP: ");
  Serial.println(WiFi.localIP());
}

//void reconnect(){
//  while(!client.connected() ){
 //   Serial.print("conectando no thingspeak nó \n");
 //   if( client.connect("Teste") ){
  //    Serial.println( "[cabosse]" );
  //    delay(2000);
  //  }
 // }
//}
