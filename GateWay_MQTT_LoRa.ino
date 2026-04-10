// ===== BIBLIOTECAS =====
#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>

// ===== PINOS LORA (ESP32) =====
#define NSS 5
#define RST 14
#define DIO0 2
#define SCK 18
#define MISO 19
#define MOSI 23

// ===== WIFI =====
const char* rede = "CSI-Lab";          // Nome da rede Wi-Fi
const char* password = "In@teLCS&I";   // Senha do Wi-Fi

WiFiClient cliente;
PubSubClient client(cliente);

// ===== MQTT (CORRIGIDO CONFORME IMAGEM) =====
const char* mqttuser = "csilab";           // Usuário MQTT
const char* mqttsenha = "WhoAmI#2024";     // Senha MQTT
const char* mqttserver = "192.168.66.11"; // IP correto do broker
const int mqttport = 1883;                // Porta MQTT

// ===== CONTROLE =====
unsigned long lasttempo = 0;  // controle de reconexão MQTT

void setup() 
{
  Serial.begin(115200);
  while(!Serial);

  // ===== SPI (OBRIGATÓRIO NO ESP32) =====
  SPI.begin(SCK, MISO, MOSI, NSS);

  // ===== LORA =====
  Serial.println("Receptor LoRa");
  LoRa.setPins(NSS, RST, DIO0);

  if(!LoRa.begin(915E6)) {
    Serial.println("Erro LoRa");
    while(1); // trava se falhar
  }

  // Parâmetros de comunicação (tem que bater com TX)
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  // ===== WIFI =====
  Serial.print("Conectando Wi-Fi: ");
  Serial.println(rede);

  WiFi.begin(rede, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // ===== MQTT =====
  client.setServer(mqttserver, mqttport); // define broker
}

void loop()
{
  // ===== GARANTE MQTT CONECTADO ANTES DE TUDO =====
  if (!client.connected()) {
    unsigned long now = millis();

    if (now - lasttempo > 5000) {
      lasttempo = now;

      Serial.print("Conectando MQTT...");

      // ID único evita conflito
      String clientId = "ESP32-";
      clientId += String(random(0xffff), HEX);

      if (client.connect(clientId.c_str(), mqttuser, mqttsenha)) {
        Serial.println("OK");
        client.publish("Status", "ESP32 conectado");
      } else {
        Serial.print("Falhou rc=");
        Serial.println(client.state()); // DEBUG REAL DO ERRO
        return; // NÃO CONTINUA SE NÃO CONECTAR
      }
    }
  } 
  else {
    client.loop(); // mantém conexão ativa
  }

  // ===== RECEBE LORA =====
  int pacote = LoRa.parsePacket();

  if(pacote) 
  {
    String receber = "";

    while (LoRa.available()) {
      char c = (char)LoRa.read();

      // aceita só caracteres válidos (remove lixo)
      if (c >= 32 && c <= 126) {
        receber += c;
      }
    }

    receber.trim();

    // ignora pacote inválido
    if (receber.length() < 5) return;
    if (!receber.startsWith("Grupo1:")) return;

    Serial.print("Pacote Recebido: ");
    Serial.println(receber);

    // ===== TRATAMENTO =====
    String dado = receber.substring(7);
    String payload;

    if (dado.startsWith("Solo Umido")) {

      Serial.println("UMIDO");

      payload = "UMIDO";

    } else {

      int valor = dado.toInt();

      Serial.print("SECO: ");
      Serial.println(valor);

      payload = "SECO:";
      payload += String(valor);
    }

    // ===== ENVIA MQTT (AGORA GARANTIDO CONECTADO) =====
    if (client.connected()) {
      client.publish("Lora/Solo", payload.c_str());
      Serial.print("Enviado MQTT: ");
      Serial.println(payload);
    } else {
      Serial.println("Erro: MQTT caiu antes de publicar");
    }
  }

  delay(200);

  // ===== RECONEXÃO WIFI =====
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconectando Wi-Fi...");
    WiFi.disconnect();
    WiFi.begin(rede, password);
    delay(3000);
  }
}