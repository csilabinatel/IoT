# Biblioteca para usar MQTT
import paho.mqtt.client as mqtt

# Biblioteca para conectar ao banco de dados MySQL
import mysql.connector



# CONEXÃO COM BANCO DE DADOS


# Conecta no banco SQL
db = mysql.connector.connect(
    host="localhost",      # endereço do servidor SQL
    user="root",           # usuário do banco
    password="root",      # senha do banco
    database="iot"         # nome do banco
)

# Cursor permite executar comandos SQL
cursor = db.cursor()



# CONFIGURAÇÃO MQTT

BROKER = "192.168.66.11"     # IP do broker MQTT
PORT = 1883                  # porta MQTT
USER = "csilab"              # usuário MQTT
PASSWORD = "WhoAmI#2024"     # senha MQTT
TOPIC = "sensor/teste"       # tópico que será escutado


# FUNÇÃO QUANDO CHEGAR DADO


def on_message(client, userdata, msg):

    # Converte mensagem recebida para texto
    valor = msg.payload.decode()

    print("Valor recebido do MQTT:", valor)

    # Comando SQL para inserir valor
    sql = "INSERT INTO nome_da_tabela (nome_da_coluna) VALUES (%s)"

    # Executa comando SQL
    cursor.execute(sql, (valor,))

    # Salva alteração no banco
    db.commit()

    print("Valor salvo no banco")


# CLIENTE MQTT


# Cria cliente MQTT
client = mqtt.Client()

# Define usuário e senha
client.username_pw_set(USER, PASSWORD)

# Define função que será chamada quando chegar mensagem
client.on_message = on_message

# Conecta no broker MQTT
client.connect(BROKER, PORT)

# Assina o tópico
client.subscribe(TOPIC)

print("Escutando dados do MQTT...")

# Mantém o programa rodando e escutando mensagens
client.loop_forever()