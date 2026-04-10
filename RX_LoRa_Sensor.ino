#include <SPI.h>
#include <LoRa.h>

// Pinos
#define NSS 10
#define RST 9
#define DIO0 2

void setup() {
  Serial.begin(115200); 
  while (!Serial);

  LoRa.setPins(NSS, RST, DIO0);

  if (!LoRa.begin(915E6)) {
    Serial.println("Erro LoRa");
    while (true);
  }

  
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();

  Serial.println("RX OK");
}

void loop() {

  int packetSize = LoRa.parsePacket();

  if (packetSize) {

    String msg = "";

    while (LoRa.available()) {
      char c = (char)LoRa.read();

      // 🔹 filtro
      if (c >= 32 && c <= 126) {
        msg += c;
      }
    }

    if (msg.length() < 5) return;
    if (!msg.startsWith("Grupo1:")) return;

    String dado = msg.substring(7);

    if (dado.startsWith("Solo Umido")) {

      Serial.print("UMIDO -> ");
      Serial.println(dado);

    } else {

      int valor = dado.toInt();

      Serial.print("SECO -> ");
      Serial.println(valor);
    }
  }
}