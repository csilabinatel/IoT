#include <SPI.h> // Biblioteca para comunicação SPI (usada pelo LoRa)
#include <LoRa.h>  // Biblioteca para controle do módulo LoRa

// Pinos
#define NSS 10 // Pino Chip Select (SS)
#define RST 9  // Pino de reset do módulo LoRa
#define DIO0 2 // Pino de interrupção (indica pacote recebido)

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial  115200
  while (!Serial); // Aguarda a serial iniciar (útil em algumas placas)


  LoRa.setPins(NSS, RST, DIO0); // Define os pinos do LoRa

  // Inicializa o LoRa na frequência de 915 MHz
  if (!LoRa.begin(915E6)) {
    Serial.println("Erro LoRa");// Mensagem de erro se não inicializar
    while (true);// Trava o programa se der erro
  }

  
  LoRa.setSpreadingFactor(7); // Define o Spreading Factor (velocidade vs alcance)
  LoRa.setSignalBandwidth(125E3); // Define largura de banda (125 kHz)
  LoRa.setCodingRate4(5);// Define taxa de codificação (correção de erro)
  LoRa.enableCrc(); // Ativa verificação de erro (CRC)

  Serial.println("RX OK");// Indica que o receptor está pronto
}

void loop() {

  int packetSize = LoRa.parsePacket();// Verifica se chegou algum pacote

  if (packetSize) {// Se um pacote foi recebido

    String msg = ""; // String para armazenar a mensagem recebida

    while (LoRa.available()) { // Enquanto houver dados disponíveis
      char c = (char)LoRa.read();// Lê um caractere do buffer

      //filtro
      if (c >= 32 && c <= 126) {
        msg += c;
      }
    }

    if (msg.length() < 5) return;
    if (!msg.startsWith("Grupo1:")) return;// Ignora mensagens que não começam com "Grupo1:"

    String dado = msg.substring(7);

    if (dado.startsWith("Solo Umido")) { // Verifica se o solo está úmido

      Serial.print("UMIDO -> ");
      Serial.println(dado);

    } else {

      int valor = dado.toInt(); // Converte o dado para número (caso seja valor de sensor)

      Serial.print("SECO -> "); // Indica solo seco
      Serial.println(valor); // Mostra o valor recebido
    }
  }
}
