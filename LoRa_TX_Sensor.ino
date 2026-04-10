#include <SPI.h>
#include <LoRa.h>

// Definição dos pinos
#define NSS 10
#define RST 9
#define DIO0 2

#define SENSOR A5  // sensor de umidade
float umidade2 = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Inicializa LoRa com os pinos definidos
  LoRa.setPins(NSS, RST, DIO0);

  // Frequência 915 MHz
  if (!LoRa.begin(915E6)) {
    Serial.println("Erro ao iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa transmissor iniciado");
}

void loop() {
  float umidade = analogRead(SENSOR);

  if (umidade>700) {

    umidade2 = umidade;

    LoRa.beginPacket();// envia
    LoRa.print("Grupo1:");
    LoRa.print(umidade);
    LoRa.endPacket();

    Serial.print("Enviado: ");
    Serial.println(umidade);

    delay(5000); // 5 segundos após envio
  }
  else {
    LoRa.beginPacket();// envia
    LoRa.print("Grupo1:");
    LoRa.print("Solo Umido: ");
    LoRa.print(umidade);
    LoRa.endPacket();

    Serial.println("Solo,ok");
    delay(5000);
  }
}