#include <ESP8266WiFi.h>   // Biblioteca para controlar o Wi-Fi no ESP8266
#include <PubSubClient.h>  // Biblioteca para comunicação MQTT

// ====== CONFIGURAÇÃO WIFI ======
const char* ssid = "CSI-Lab";          // Nome da rede Wi-Fi que o ESP vai conectar
const char* password = "In@teLCS&I";   // Senha da rede Wi-Fi

// ====== CONFIGURAÇÃO MQTT ======
const char* server = "192.168.66.11";  // IP do broker MQTT (servidor)
const int port = 1883;                 // Porta do MQTT (1883 = sem criptografia)
const char* user = "csilab";           // Usuário do broker MQTT
const char* pass = "WhoAmI#2024";      // Senha do broker MQTT
const char* topic = "casa/botao";      // Tópico onde vamos publicar

// ====== PINO DO BOTÃO ======
#define BOTAO D2   // Pino D2 (evite D3 pois pode impedir o boot do ESP)

// ====== OBJETOS ======
WiFiClient wifiClient;                 // Cria um cliente de rede Wi-Fi
PubSubClient client(wifiClient);       // Cria cliente MQTT usando o Wi-Fi

// ====== FUNÇÃO PARA CONECTAR NO WIFI ======
void conectaWiFi() {
  Serial.println("Iniciando WiFi...");     // Mostra mensagem na serial

  WiFi.begin(ssid, password);              // Inicia tentativa de conexão com Wi-Fi

  while (WiFi.status() != WL_CONNECTED) {  // Enquanto não conectar no Wi-Fi
    Serial.println("Tentando conectar..."); // Mostra tentativa na serial
    delay(1000);                           // Espera 1 segundo antes de tentar de novo
  }

  Serial.println("WiFi conectado!");       // Confirma conexão
  Serial.println(WiFi.localIP());          // Mostra o IP do ESP
}

// ====== FUNÇÃO PARA CONECTAR NO MQTT ======
void conectaMQTT() {
  Serial.println("Conectando MQTT...");    // Informa que vai conectar no broker

  while (!client.connected()) {            // Enquanto não estiver conectado ao broker

    // Tenta conectar usando ID, usuário e senha
    if (client.connect("ESP8266_CLIENTE", user, pass)) {
      Serial.println("MQTT conectado!");   // Conectou com sucesso
    } else {
      Serial.print("Erro MQTT: ");         // Mostra erro se falhar
      Serial.println(client.state());      // Código do erro
      delay(2000);                        // Espera 2 segundos antes de tentar de novo
    }
  }
}

// ====== SETUP (RODA UMA VEZ) ======
void setup() {

  Serial.begin(115200);                   // Inicia comunicação serial em 115200
  Serial.println("INICIOU");              // Teste para ver se o código começou

  pinMode(BOTAO, INPUT_PULLUP);           // Define botão como entrada com pull-up interno

  conectaWiFi();                          // Chama função para conectar no Wi-Fi

  client.setServer(server, port);         // Define IP e porta do broker MQTT

  conectaMQTT();                          // Chama função para conectar no MQTT
}

// ====== LOOP (RODA PARA SEMPRE) ======
void loop() {

  if (!client.connected()) {              // Se perdeu conexão com MQTT
    conectaMQTT();                        // Reconecta automaticamente
  }

  client.loop();                          // Mantém a conexão MQTT viva

  int leitura = digitalRead(BOTAO);       // Lê o estado do botão (LOW ou HIGH)

  // ===== BOTÃO PRESSIONADO =====
  if (leitura == LOW) {                   // Se botão pressionado (pull-up → LOW)
    Serial.println("1");                  // Mostra 1 na serial
    client.publish(topic, "1");           // Publica "1" no tópico MQTT
  }

  // ===== BOTÃO SOLTO =====
  if (leitura == HIGH) {                  // Se botão solto
    Serial.println("0");                  // Mostra 0 na serial
    client.publish(topic, "0");           // Publica "0" no tópico MQTT
  }

  delay(7000);                            // Espera 1 segundo antes de repetir
}
