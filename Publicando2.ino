#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define botao D8  // Definição do pino do botão

// Informações da rede Wi-Fi
const char* ssid = "WLL-Inatel";         
const char* password = "inatelsemfio";  

// Informações do broker MQTT
const char* mqttServer = "0d7c95f4e39541858c28b9278c812618.s1.eu.hivemq.cloud";   
const char* mqttUser = "INATEL";           
const char* mqttPassword = "Telecom2025";  
const int mqttPort = 8883;   
const char* ID = "aaaaa";  // Nome do dispositivo
const char* mqttTopicPub = "TROCARESSENOME";  // Tópico onde o ESP vai publicar mensagens

WiFiClientSecure espClient;  
PubSubClient client(espClient);

void connect() { 
  while (!client.connected()) { 
    Serial.println("Conectando ao Broker MQTT...");

    if (client.connect(ID, mqttUser, mqttPassword)) {
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.printf("Falha na conexão MQTT. Código de erro: %d\n", client.state());
      Serial.println("Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(botao, INPUT);
  
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");

  // Configuração do cliente seguro
  espClient.setInsecure();  // REMOVE CHECAGEM DO CERTIFICADO
  espClient.setTimeout(30);  

  client.setServer(mqttServer, mqttPort);

  connect();
}

void loop() {
  static int estadoAnterior = -1; // Armazena o estado anterior do botão

  int estadoBotao = digitalRead(botao); // Lê o estado do botão

  // Só publica se o estado do botão mudar
  if (estadoBotao != estadoAnterior) {
    estadoAnterior = estadoBotao;
    
    if (estadoBotao == HIGH) {
      Serial.println("Botão pressionado! Enviando 1...");
      client.publish(mqttTopicPub, "1");
    } else {
      Serial.println("Botão solto! Enviando 0...");
      client.publish(mqttTopicPub, "0");
    }

    delay(100); // Pequeno atraso para evitar rebotes
  }

  if (!client.connected()) {
    connect();
  }
  client.loop();
}
