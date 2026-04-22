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

  Serial.println("LoRa transmissor iniciado"); // Confirma inicialização com sucesso
}

void loop() {
  float umidade = analogRead(SENSOR); // Lê o valor analógico do sensor (0 a 1023)

  if (umidade>700) {  // Verifica se o valor indica solo seco (limiar definido como 700)

    umidade2 = umidade; // Armazena o valor lido em outra variável

    LoRa.beginPacket(); // Inicia o pacote de dados para envio
    LoRa.print("Grupo1:"); // Envia identificador do grupo
    LoRa.print(umidade);   // Envia o valor da umidade
    LoRa.endPacket();      // Finaliza e transmite o pacote

    Serial.print("Enviado: "); // Mensagem no monitor serial
    Serial.println(umidade);   // Mostra o valor enviado

    delay(5000); // Aguarda 5 segundos antes do próximo envio
  }
  else { // Caso o valor seja menor ou igual a 700 (solo úmido)

    LoRa.beginPacket(); // Inicia o pacote de dados
    LoRa.print("Grupo1:"); // Envia identificador
    LoRa.print("Solo Umido: "); // Indica que o solo está úmido
    LoRa.print(umidade); // Envia valor da leitura
    LoRa.endPacket(); // Finaliza e envia o pacote

    Serial.println("Solo,ok"); // Mensagem indicando estado normal
    delay(5000); // Aguarda 5 segundos
  }
}
