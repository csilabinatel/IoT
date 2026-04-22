#include <SPI.h>        // Biblioteca para comunicação SPI (usada pelo módulo LoRa)
#include <LoRa.h>       // Biblioteca para controle do módulo LoRa (SX1276)

// Definição dos pinos
#define NSS 10          // Pino Chip Select (CS) do LoRa
#define RST 9           // Pino de RESET do módulo LoRa
#define DIO0 2          // Pino de interrupção (DIO0) do LoRa (indica eventos como envio/recebimento)

#define SENSOR A5       // Pino analógico A5 onde está conectado o sensor de umidade
float umidade2 = 0;     // Variável para armazenar valor de umidade (uso auxiliar)

void setup() {
  Serial.begin(9600);  // Inicializa comunicação serial a 9600 bps
  while (!Serial);     // Aguarda a serial estar pronta (útil em algumas placas)

  // Inicializa LoRa com os pinos definidos
  LoRa.setPins(NSS, RST, DIO0);  // Informa para a biblioteca quais pinos estão conectados ao módulo

  // Frequência 915 MHz
  if (!LoRa.begin(915E6)) {      // Tenta iniciar o LoRa na frequência de 915 MHz
    Serial.println("Erro ao iniciar LoRa"); // Mostra erro se não conseguir iniciar
    while (true);                // Trava o programa caso haja falha
  }
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("LoRa transmissor iniciado"); // Confirma inicialização com sucesso
}

void loop() {
  float umidade = analogRead(SENSOR);

  // ENVIO IGUAL PARA OS DOIS CASOS
  LoRa.beginPacket();
  LoRa.print("Grupo1:");
  LoRa.println(umidade); 
  LoRa.endPacket();

  // SERIAL IGUAL PARA OS DOIS CASOS
  Serial.print("Enviado: ");
  Serial.println(umidade);

  delay(500);
}
