from flask import Flask, jsonify
from flask_cors import CORS
import mysql.connector


# CONFIGURAÇÃO EDITÁVEL

TABELA = "nome_da_sua_tabela"  # <-- Coloque aqui o nome da tabela
COLUNAS = ["temperatura"]  # Colunas que quer retornar

DB_CONFIG = {
    "host": "SEU_HOST",       # ex: localhost
    "user": "SEU_USUARIO",    # ex: root
    "password": "SUA_SENHA",  # senha do MySQL
    "database": "SEU_BANCO"   # nome do banco
}
# -----------------------------

app = Flask(__name__)
CORS(app)

def conectar_banco():
    return mysql.connector.connect(**DB_CONFIG)

@app.route("/api/sensores", methods=["GET"])
def pegar_dados():
    try:
        conexao = conectar_banco()
        cursor = conexao.cursor(dictionary=True)

        # Monta query: pega último registro da tabela
        query = f"SELECT {', '.join(COLUNAS)} FROM {TABELA} ORDER BY id DESC LIMIT 1"
        cursor.execute(query)
        resultado = cursor.fetchone()

        if resultado:
            return jsonify(resultado)
        else:
            # Caso não tenha dados
            return jsonify({col: 0 for col in COLUNAS})

    except Exception as e:
        return jsonify({"erro": str(e)})

    finally:
        if 'cursor' in locals(): cursor.close()
        if 'conexao' in locals(): conexao.close()


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)