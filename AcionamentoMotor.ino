/*****
Desenvolvido por Gabriel Tavares
Universidade Federal de Minas Gerais
Nov/2019
*****/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Control";
const char* password = "lg95-net";

const char* mqtt_server = "192.168.1.2";

WiFiClient espClient;
PubSubClient client(espClient);

// Saídas
const int IN2 = 5;
const int IN1 = 4;
const int ENA = 0;


String Direcao = "0";
String Ligado = "0";
int VelocidadeAtual = 0;
int VeloficdadeRef = 0;
int AlteraVelocidadeEsquerda = 0;
int AlteraVelocidadeDireita = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  //Serial.print("Message arrived on topic: ");
  //Serial.print(topic);
  //Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (topic == "Liga/Desliga") {
    if (messageTemp == "1" && Direcao == "1") {
      Ligado = "1";
      AlteraVelocidadeEsquerda = 1;
      AlteraVelocidadeDireita = 0;
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      Serial.print("Ligado Esquerda: ");
      Serial.print(Ligado);
    }
    else if (messageTemp == "1" && Direcao == "0") {
      Ligado = "1";
      AlteraVelocidadeEsquerda = 0;
      AlteraVelocidadeDireita = 1;
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      Serial.print("Ligado Direita: ");
      Serial.print(Ligado);
    }
    else if (messageTemp == "0") {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      Ligado = "0";
      AlteraVelocidadeEsquerda = 0;
      AlteraVelocidadeDireita = 0;
      VelocidadeAtual = 0;
      Serial.print("Desligado ");
      Serial.print(Ligado);
    }
  }
  if (topic == "Direcao") {
    if (messageTemp == "1" && Ligado == "0") {
      Direcao = "1";
      Serial.print("Nova Direção: Esquerda: ");
      Serial.print(Direcao);

    }
    else if (messageTemp == "0" && Ligado == "0") {
      Direcao = "0";
      Serial.print("Nova Direção: Direita: ");
      Serial.print(Direcao);
    }
  }
  if (topic == "RefVelocidade") {
    VeloficdadeRef = messageTemp.toInt();
    if (AlteraVelocidadeEsquerda == 1) {
      if (VeloficdadeRef >= VelocidadeAtual) {
        for (int i = VelocidadeAtual; i <= VeloficdadeRef; i++) {
          analogWrite(ENA, i);
          delay(20);
        }

        VelocidadeAtual = VeloficdadeRef;
        Serial.print("Velocidade: ");
        Serial.print(VelocidadeAtual);
        Serial.println();
      }
      else {
        for (int i = VelocidadeAtual; i >= VeloficdadeRef; i--) {
          analogWrite(ENA, i);
          delay(20);
        }

        VelocidadeAtual = VeloficdadeRef;
        Serial.print("Velocidade: ");
        Serial.print(VelocidadeAtual);
        Serial.println();
      }

    }
    else if (AlteraVelocidadeDireita == 1) {
      if (VeloficdadeRef >= VelocidadeAtual) {
        for (int i = VelocidadeAtual; i <= VeloficdadeRef; i++) {
          analogWrite(ENA, i);
          delay(20);
        }

        VelocidadeAtual = VeloficdadeRef;
        Serial.print("Velocidade: ");
        Serial.print(VelocidadeAtual);
        Serial.println();
      }
      else {
        for (int i = VelocidadeAtual; i >= VeloficdadeRef; i--) {
          analogWrite(ENA, i);
          delay(20);
        }

        VelocidadeAtual = VeloficdadeRef;
        Serial.print("Velocidade: ");
        Serial.print(VelocidadeAtual);
        Serial.println();        
      }
    }
  }
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("Liga/Desliga");
      client.subscribe("Direcao");
      client.subscribe("RefVelocidade");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())

    client.connect("ESP8266Client");
}
