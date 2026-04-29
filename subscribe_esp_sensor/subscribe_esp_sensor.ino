#include <ESP8266WiFi.h>   // Biblioteca WiFi para ESP8266
#include <PubSubClient.h>  // Biblioteca MQTT

// ====== CONFIGURAÇÕES WIFI ======
const char* ssid = "";       // Nome do WiFi
const char* password = "";   // Senha do WiFi

// ====== CONFIGURAÇÕES MQTT ======
const char* mqtt_server = ""; // IP do broker (ex: 192.168.1.10)
const int mqtt_port = 1883;   // Porta MQTT

const char* mqtt_user = "";   // Usuário MQTT
const char* mqtt_pass = "";   // Senha MQTT

const char* topic = "casa/botao"; // Tópico que será assinado

// ====== PINOS ======
#define LED_AZUL D1     // LED azul (liga com 1)
#define LED_VERMELHO D2 // LED vermelho (liga com 0)

// ====== OBJETOS ======
WiFiClient espClient;           // Cliente WiFi
PubSubClient client(espClient); // Cliente MQTT

// ====== CALLBACK (QUANDO RECEBE MENSAGEM) ======
void callback(char* topico, byte* payload, unsigned int length) {

  String mensagem = "";

  // Converte o payload (byte) para String
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }

  Serial.print("TOPICO: ");
  Serial.println(topico);

  Serial.print("MENSAGEM: ");
  Serial.println(mensagem);

  // ===== CONTROLE DOS LEDs =====
  if (mensagem == "1") {
    digitalWrite(LED_AZUL, HIGH);      // Liga LED azul
    digitalWrite(LED_VERMELHO, LOW);   // Desliga vermelho
    Serial.println("LED AZUL LIGADO");
  } 
  
  else if (mensagem == "0") {
    digitalWrite(LED_AZUL, LOW);       // Desliga azul
    digitalWrite(LED_VERMELHO, HIGH);  // Liga vermelho
    Serial.println("LED VERMELHO LIGADO");
  }

  Serial.println("-------------------");
}

// ====== CONEXÃO WIFI ======
void conectaWiFi() {

  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP()); // Mostra IP
}

// ====== CONEXÃO MQTT ======
void conectaMQTT() {

  while (!client.connected()) {

    Serial.print("Conectando MQTT...");

    if (client.connect("ESP_SUB", mqtt_user, mqtt_pass)) {
      Serial.println("conectado");

      client.subscribe(topic); // Assina o tópico
      Serial.print("ASSINADO: ");
      Serial.println(topic);
    } 
    
    else {
      Serial.print("Erro rc=");
      Serial.print(client.state());
      Serial.println(" tentando em 5s");
      delay(5000);
    }
  }
}

// ====== SETUP ======
void setup() {

  Serial.begin(115200); // Inicia serial

  pinMode(LED_AZUL, OUTPUT);     // Define LED azul como saída
  pinMode(LED_VERMELHO, OUTPUT); // Define LED vermelho como saída

  digitalWrite(LED_AZUL, LOW);     // Começa desligado
  digitalWrite(LED_VERMELHO, LOW); // Começa desligado

  conectaWiFi(); // Conecta WiFi

  client.setServer(mqtt_server, mqtt_port); // Define broker
  client.setCallback(callback);             // Define função de recebimento

  conectaMQTT(); // Conecta MQTT
}

// ====== LOOP ======
void loop() {

  if (!client.connected()) {
    conectaMQTT(); // Reconecta se cair
  }

  client.loop(); // Mantém escutando mensagens
}
