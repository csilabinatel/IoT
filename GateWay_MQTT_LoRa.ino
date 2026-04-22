#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ================= CONFIGURACOES WIFI =================
const char* ssid     = "CSI-Lab";
const char* password = "In@teLCS&I";

// ================= CONFIGURACOES MQTT =================
const char* mqtt_server = "192.168.66.11";
const int   mqtt_port   = 1883;
const char* mqtt_user   = "csilab";
const char* mqtt_pass   = "WhoAmI#2024";

// ================= PINOS LORA (ESP32) =================
// Use estes pinos para conectar o modulo LoRa no ESP32
#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS   5
#define LORA_RST  26
#define LORA_DIO0 2
#define LORA_FREQ 915E6

WiFiClient espClient;
PubSubClient client(espClient);

// ================= FUNCOES DE CONEXAO =================

void setup_wifi() {
  delay(10);
  Serial.print("Conectando em: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  // Loop ate conectar no MQTT
  while (!client.connected()) {
    Serial.print("Tentando conexao MQTT...");
    // ID do cliente unico para este gateway
    if (client.connect("Gateway_Grupo1_ESP32", mqtt_user, mqtt_pass)) {
      Serial.println(" conectado com sucesso!");
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// ================= SETUP PRINCIPAL =================

void setup() {
  Serial.begin(115200);

  // Inicia conexao sem fio
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Inicia comunicacao SPI e o Radio LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Erro critico: Modulo LoRa nao encontrado!");
    while (1);
  }

  // Sincronizacao identica ao seu Transmissor (TX)
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("Gateway online. Escutando Grupo 1...");
}

// ================= LOOP PRINCIPAL =================

void loop() {
  // Mantem conexao MQTT viva
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  // Tenta capturar um pacote de radio
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    String recebido = "";

    // Le o pacote caractere por caractere
    while (LoRa.available()) {
      recebido += (char)LoRa.read();
    }

    // Filtra apenas o Grupo 1 conforme o seu TX envia
    if (recebido.startsWith("Grupo1:")) {
      
      // Isola apenas o numero (tira o prefixo "Grupo1:")
      String valorUmidade = recebido.substring(7);
      valorUmidade.trim(); // Remove quebras de linha (\n \r)

      // Mostra no monitor serial para conferencia
      Serial.println("--- NOVO DADO ---");
      Serial.print("Origem: Grupo 1 | Valor: ");
      Serial.println(valorUmidade);
      Serial.print("Sinal (RSSI): ");
      Serial.print(LoRa.packetRssi());
      Serial.println(" dBm");

      // Define o topico MQTT
      String topic = "sensores/grupo1/umidade";

      // Envia para o Broker MQTT
      if (client.publish(topic.c_str(), valorUmidade.c_str())) {
        Serial.println("Status: Enviado para MQTT");
      } else {
        Serial.println("Status: Erro ao enviar MQTT");
      }
      Serial.println("-----------------");
    }
  }
}
