#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DHT.h>
#include <NewPing.h>

// === CONFIGURAÇÕES ===
#define DHTPIN D2
#define DHTTYPE DHT11
#define TRIGGER_PIN D5
#define ECHO_PIN D6
#define MAX_DISTANCE 200
#define LED_PIN D4

// === OBJETOS ===
DHT dht(DHTPIN, DHTTYPE);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
ESP8266WebServer server(80);

// === REDE Wi-Fi ===
const char* ssid = "ESP8266_Rede";       // nome da rede criada
const char* password = "12345678";       // senha da rede

void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleDados() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  unsigned int distancia = sonar.ping_cm();

  String json = "{";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1) + ",";
  json += "\"distancia\":" + String(distancia) + "}";
  
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);

  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar SPIFFS!");
    return;
  }

  WiFi.softAP(ssid, password);
  Serial.println("✅ Acesse a rede Wi-Fi:");
  Serial.println(ssid);
  Serial.println("Senha: 12345678");
  Serial.println("Abra o navegador e vá para: http://192.168.4.1");

  server.on("/", handleRoot);
  server.on("/dados", handleDados);
  server.begin();
  Serial.println("Servidor iniciado!");
}

void loop() {
  server.handleClient();

  unsigned int distancia = sonar.ping_cm();
  digitalWrite(LED_PIN, (distancia > 0 && distancia < 10) ? HIGH : LOW);
}
