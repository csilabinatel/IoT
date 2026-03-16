//#include <WiFi.h>
#include <ESP8266WiFi.h> // use esta se for ESP8266
#include <PubSubClient.h>

//parametros de conexão no mqtt e no wifi
const char* ssid = "CSI-Lab";
const char* password = "In@teLCS&I";

const char* mqtt_server = "192.168.66.11";
const char* mqtt_user = "csilab";
const char* mqtt_pass = "WhoAmI#2024";

WiFiClient espClient;
PubSubClient client(espClient);
// Função que mostra sempre que uma mensagem chega em um tópico assinado
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("TOPICO: ");
  Serial.println(topic);

  Serial.print("DADO RECEBIDO: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);   // MOSTRA O CONTEÚDO DO TÓPICO
  }

  Serial.println();
  Serial.println("--------------");
}

void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi conectado");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {

    if (client.connect("ESP_SUB", mqtt_user, mqtt_pass)) {
      //conecta no mqtt e assina o topico solicitado

      Serial.println("MQTT conectado");

      client.subscribe("sensor/teste");

      Serial.println("ASSINADO: sensor/teste");
    }
  }
}

void loop() {

  if (!client.connected()) {
    client.connect("ESP_SUB", mqtt_user, mqtt_pass);//conecta o mqtt
    client.subscribe("sensor/teste");// assina o topico 
  }

  client.loop();// fica assinando e verificando se tem algo novo 
}