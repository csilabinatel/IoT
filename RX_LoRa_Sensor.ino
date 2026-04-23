#include <SPI.h>
#include <LoRa.h>

// Definição dos pinos
#define NSS 10          // Pino Chip Select (CS) do LoRa //pino que ouve o Ligar do LoRa"conversa"
#define RST 9           // Pino de RESET do módulo LoRa
#define DIO0 2          // Pino de interrupção (DIO0,com a biblioteca) do LoRa (indica eventos como envio/recebimento) sem precisar dos calculos do tempo 


void setup() {
  Serial.begin(115200);//Inicialização do LoRa                                                   
  while (!Serial);

  LoRa.setPins(NSS, RST, DIO0);

  // Inicia o rádio
  if (!LoRa.begin(915E6)) {
    Serial.println("Erro ao iniciar LoRa");
    while (true);
  }

  // --- OTIMIZAÇÃO DE VELOCIDADE  IGUAL AO TX ---
  LoRa.setSpreadingFactor(7);//(7 Velocidade alta com alcance menor)Quantos bits são usados para representar cada símbolo,afeta diretamente alcance e velocidade = 2^7=128;
  // Largura de banda de 125E3 é padrão.
  LoRa.setSignalBandwidth(125E3); //Tamanho da faixa de Frêquencia
  LoRa.setCodingRate4(5);//Envia Bits extras para uma correção
  LoRa.enableCrc();//Verificação de erro(Verifica o que foi transmitido com o que está nele)

  Serial.println("RX veloz iniciado. Aguardando Grupo 1...");
}

void loop() {
  // Verifica se chegou pacote
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String recebido = "";

    // Lê o pacote caractere por caractere
    while (LoRa.available()) {
      recebido += (char)LoRa.read();//LEITURA DO PACOTE BIT POR BIT 
    }

    // Filtra apenas o Grupo 1
    if (recebido.startsWith("Grupo1:"))//Ele verifica se o começo do pacote começa com o nosso grupo{
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
