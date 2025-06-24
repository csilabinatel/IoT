#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int buzzer =  D2;

const char* ssid = "WLL-Inatel";
const char* password = "inatelsemfio";  // senha da rede Wi-Fi

// Informações do broker MQTT
const char* mqttServer = "0d7c95f4e39541858c28b9278c812618.s1.eu.hivemq.cloud";   
const char* mqttUser = "INATEL";           
const char* mqttPassword = "Telecom2025";  
const int mqttPort = 8883;                 
const char* mqttTopicSub = "TROCARESSENOME/#";       
const char* ID = "TROCARESSENOME";// Deve ser um nome único na rede            

WiFiClientSecure espClient;  // CLIENTE SEGURO
PubSubClient client(espClient);

// Função de callback para mensagens MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';          
  int MSG = atoi((char*)payload);  
  Serial.printf("Mensagem recebida [%s]: %d\n", topic, MSG);

  if (MSG == 1) {
    Serial.println("Ligando LED!");
    tone(buzzer, 2000);  // Toca som a 1000 Hz
    delay(1000);
    tone(buzzer, 0);  // Toca som a 1000 Hz
  } else if (MSG == 0) {
    Serial.println("Desligando LED!");
    tone(buzzer, 0);  // Toca som a 1000 Hz
  }
}

void connect() { 
  while (!client.connected()) { 
    Serial.println("Conectando ao Broker MQTT...");

    if (client.connect(ID, mqttUser, mqttPassword)) {
      Serial.println("Conectado ao MQTT!");
      client.subscribe(mqttTopicSub);
    } else {
      Serial.printf("Falha na conexão MQTT. Código de erro: %d\n", client.state());
      Serial.println("Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");

  // Configuração do cliente seguro
  espClient.setInsecure();  // REMOVE CHECAGEM DO CERTIFICADO (ou use um CA válido)
  espClient.setTimeout(30);  // Timeout de 30s para conexões lentas

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  connect();
}

void loop() {
  if (!client.connected()) {
    connect();
  }
  client.loop();
}
