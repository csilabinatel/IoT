#include <ESP8266WiFi.h>   // Biblioteca para conexão WiFi no ESP8266
#include <PubSubClient.h>  // Biblioteca para comunicação MQTT

// ====== CONFIGURAÇÕES WIFI ======
const char* ssid = "";       // Nome da rede WiFi
const char* password = "";   // Senha do WiFi

// ====== CONFIGURAÇÕES MQTT ======
const char* mqtt_server = ""; // IP do broker MQTT (ex: 192.168.1.10)
const int mqtt_port = 1883;   // Porta padrão MQTT

const char* mqtt_user = "";   // Usuário do broker
const char* mqtt_pass = "";   // Senha do broker

// ====== DEFINIÇÃO DE PINO ======
#define BOTAO_PIN D3   // Pino onde o botão está conectado (GPIO0)

// ====== VARIÁVEIS ======
int estadoBotao = 0;   // Variável para armazenar estado do botão

WiFiClient espClient;           // Cria cliente WiFi
PubSubClient client(espClient); // Cria cliente MQTT

// ====== FUNÇÃO WIFI ======
void conectaWiFi() {
  WiFi.begin(ssid, password); // Inicia conexão com WiFi

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) { // Espera conectar
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP()); // Mostra IP do ESP
}

// ====== FUNÇÃO MQTT ======
void conectaMQTT() {
  while (!client.connected()) { // Enquanto não conectar

    Serial.print("Conectando ao broker MQTT...");

    if (client.connect("ESP_Client", mqtt_user, mqtt_pass)) { // Tenta conectar
      Serial.println("conectado");
    } 
    
    else {
      Serial.print("Erro, rc=");
      Serial.print(client.state()); // Mostra erro
      Serial.println(" tentando novamente em 5s");
      delay(5000); // Aguarda antes de tentar novamente
    }
  }
}

// ====== SETUP ======
void setup() {

  Serial.begin(115200); // Inicia comunicação serial

  conectaWiFi(); // Conecta no WiFi

  pinMode(BOTAO_PIN, INPUT_PULLUP); // Define botão com pull-up interno

  client.setServer(mqtt_server, mqtt_port); // Define servidor MQTT

  conectaMQTT(); // Conecta ao broker
}

// ====== LOOP PRINCIPAL ======
void loop() {

  if (!client.connected()) {
    conectaMQTT(); // Reconecta se cair
  }

  client.loop(); // Mantém conexão ativa

  // ===== LEITURA DO BOTÃO =====
  int leitura = digitalRead(BOTAO_PIN); // Lê o pino

  // Com pull-up: LOW = pressionado, HIGH = solto
  estadoBotao = (leitura == LOW) ? 1 : 0;

  // ===== PUBLICAÇÃO MQTT =====
  client.publish("casa/botao", String(estadoBotao).c_str()); // Envia valor

  Serial.print("Botão: ");
  Serial.println(estadoBotao); // Mostra no serial

  delay(2000); // Aguarda 2 segundos
}
