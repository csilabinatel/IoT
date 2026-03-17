#include <ESP8266WiFi.h> // use esta se for ESP8266
#include <PubSubClient.h>

const char* ssid = "";//CSI-Lab - Senha
const char* password = "";//In@teLCS&I - Senha

const char* mqtt_server = "";//192.168.66.11 ip 
const int mqtt_port = 1883;

const char* mqtt_user = "";//csilab
const char* mqtt_pass = "";//WhoAmI#2024

#define TEMP_PIN A0  // Definição do pino do SENSOR


int TEMP = 0;  // Variável que armazenará o valor de temperatura do sensor (exemplo)

WiFiClient espClient; // Cria um cliente WiFi 
PubSubClient client(espClient);// Cria o cliente MQT

void conectaWiFi() {// CONEXÃO PARA O WI-FI 
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());//DEIXA CLARO QUAL O IP 
}

void conectaMQTT() {// CONEXÃO COM MQTT

  while (!client.connected()) {

    Serial.print("Conectando ao broker MQTT...");

    if (client.connect("ESP_Client", mqtt_user, mqtt_pass)) {//Tenta se conectar com a senha e user 
      Serial.println("conectado");
    } 
    
    else {
      Serial.print("Erro, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");//Se não se conectar vai novamente 
      delay(5000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  conectaWiFi();
  pinMode(TEMP_PIN, INPUT); // Define o pino 34 como entrada para leitura de sensor
  client.setServer(mqtt_server, mqtt_port);

  conectaMQTT();
}

void loop() {

  if (!client.connected()) {
    conectaMQTT();
  }

  client.loop();
  TEMP = analogRead(TEMP_PIN);// Leitura do pino para sensor analogica 
  client.publish("sensor/teste", String(TEMP).c_str());// PRECISA TRANSFORMAR EM STRING
  // SER PUBLICADO NO MQTT EM JSON

  Serial.println("Mensagem publicada");

  delay(5000);
}
