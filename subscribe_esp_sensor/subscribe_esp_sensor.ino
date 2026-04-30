#include <ESP8266WiFi.h>   // Biblioteca que permite o ESP8266 conectar no Wi-Fi
#include <PubSubClient.h>  // Biblioteca que permite usar o protocolo MQTT

// ====== CONFIGURAÇÃO WIFI ======
const char* ssid = "CSI-Lab";        // Nome da rede Wi-Fi que o ESP vai tentar conectar
const char* password = "In@teLCS&I"; // Senha da rede Wi-Fi

// ====== CONFIGURAÇÃO MQTT ======
const char* server = "192.168.66.11"; // Endereço IP do broker MQTT (servidor)
const int port = 1883;                // Porta do MQTT (1883 = padrão sem segurança)
const char* user = "csilab";          // Usuário para autenticação no broker
const char* pass = "WhoAmI#2024";     // Senha para autenticação no broker
const char* topic = "casa/botao";     // Tópico que o ESP vai escutar (subscribe)

// ====== DEFINIÇÃO DOS LEDS ======
#define LED1 D5   // LED1 será ligado quando receber "0"
#define LED2 D6   // LED2 será ligado quando receber "1"
#define LED3 D7   // LED3 será ligado quando receber "1"

// ====== OBJETOS ======
WiFiClient wifiClient;                 // Cria um objeto que gerencia a conexão Wi-Fi
PubSubClient client(wifiClient);       // Cria o cliente MQTT usando a conexão Wi-Fi

// ====== FUNÇÃO QUE EXECUTA QUANDO CHEGA MENSAGEM MQTT ======
void callback(char* topico, byte* payload, unsigned int length) {
  
  Serial.println("CHEGOU MENSAGEM");   // Mostra que o ESP recebeu algo do broker

  // Aqui vamos analisar apenas o PRIMEIRO caractere recebido
  // payload[0] é o primeiro byte da mensagem

  if (payload[0] == '0') {             // Se o primeiro caractere for '0'
    digitalWrite(LED1, HIGH);          // Liga o LED1
    digitalWrite(LED2, LOW);           // Garante que LED2 está desligado
    digitalWrite(LED3, LOW);           // Garante que LED3 está desligado
    Serial.println("Recebi 0");        // Mostra na serial
  }

  if (payload[0] == '1') {             // Se o primeiro caractere for '1'
    digitalWrite(LED1, LOW);           // Desliga o LED1
    digitalWrite(LED2, HIGH);          // Liga o LED2
    digitalWrite(LED3, HIGH);          // Liga o LED3
    Serial.println("Recebi 1");        // Mostra na serial
  }
}

// ====== FUNÇÃO SETUP (RODA UMA VEZ AO LIGAR) ======
void setup() {

  Serial.begin(115200);                 // Inicia comunicação serial (para debug)
  Serial.println("INICIOU CODIGO");     // Confirma que o ESP começou a rodar

  pinMode(LED1, OUTPUT);                // Define LED1 como saída
  pinMode(LED2, OUTPUT);                // Define LED2 como saída
  pinMode(LED3, OUTPUT);                // Define LED3 como saída

  Serial.println("Iniciando WiFi...");  // Mensagem antes de tentar Wi-Fi

  WiFi.begin(ssid, password);           // Começa tentativa de conexão Wi-Fi

  int tentativas = 0;                   // Contador de tentativas

  // Loop tenta conectar no Wi-Fi no máximo 10 vezes
  while (WiFi.status() != WL_CONNECTED && tentativas < 10) {
    Serial.println("Tentando WiFi..."); // Mostra tentativa
    delay(1000);                       // Espera 1 segundo
    tentativas++;                      // Soma uma tentativa
  }

  // Verifica se conectou ou não
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi CONECTADO");  // Conectou com sucesso
  } else {
    Serial.println("FALHA WIFI");      // NÃO conectou → problema de rede
  }

  client.setServer(server, port);      // Define o endereço do broker MQTT

  client.setCallback(callback);        // Define a função que será chamada quando chegar mensagem

  Serial.println("Conectando MQTT..."); // Aviso antes de conectar no broker

  // Tenta conectar UMA VEZ (não trava o código)
  if (client.connect("ESP_SUB", user, pass)) {
    Serial.println("MQTT CONECTADO");  // Conectou com sucesso

    client.subscribe(topic);           // Se inscreve no tópico para receber mensagens
    Serial.println("INSCRITO NO TOPICO"); // Confirma inscrição
  } else {
    Serial.print("ERRO MQTT: ");       // Mostra erro
    Serial.println(client.state());    // Código do erro MQTT
  }
}

// ====== LOOP (RODA PARA SEMPRE) ======
void loop() {

  client.loop();                       // Mantém a comunicação MQTT funcionando
}
