#include <SPI.h>
#include <LoRa.h>

// Pinos para Arduino Uno
#define NSS 10
#define RST 9
#define DIO0 2

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(NSS, RST, DIO0);

  // Inicia o rádio
  if (!LoRa.begin(915E6)) {
    Serial.println("Erro ao iniciar LoRa");
    while (true);
  }

  // --- OTIMIZAÇÃO DE VELOCIDADE ---
  // SF7 é o mais rápido. Se quiser MAIS rápido, use 6 (mas exige precisão de cristal)
  LoRa.setSpreadingFactor(7); 
  // Largura de banda de 125E3 é padrão. 250E3 seria mais rápido, mas 125 é mais estável.
  LoRa.setSignalBandwidth(125E3); 
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("RX veloz iniciado. Aguardando Grupo 1...");
}

void loop() {
  // Verifica se chegou pacote
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String recebido = "";

    // Lê o pacote caractere por caractere
    while (LoRa.available()) {
      recebido += (char)LoRa.read();
    }

    // Filtra apenas o Grupo 1
    if (recebido.startsWith("Grupo1:")) {
      // Isola o valor após os 7 caracteres de "Grupo1:"
      String valor = recebido.substring(7);
      
      // Limpa espaços ou quebras de linha extras que venham do TX
      valor.trim(); 

      Serial.print("Umidade G1: ");
      Serial.println(valor);
      
      // RSSI mostra a qualidade do sinal (perto de 0 é melhor, ex: -35 é ótimo)
      Serial.print("Sinal: ");
      Serial.print(LoRa.packetRssi());
      Serial.println(" dBm");
      Serial.println("---");
    }
  }
}
