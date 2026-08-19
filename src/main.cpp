#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --- CONFIGURAÇÕES DA REDE WI-FI ---
// Substitua pelas credenciais da sua rede Wi-Fi (a mesma onde o PC está conectado)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- ENDEREÇO DO NODE-RED ---
// IP Local do seu computador ajustado
//const char* serverUrl = "http://192.168.67.1: Q1W21880/presenca";
//const char* serverUrl = "http://192.168.67.1:1880/presenca";
const char* serverUrl = "http://host.wokwi.internal:1880/presenca";
//const char* serverUrl = "http://10.0.2.2:1880/presenca";

void enviarPresenca(int ra) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Monta o JSON: {"ra": 1001}
    StaticJsonDocument<128> doc;
    doc["ra"] = ra;
    String requestBody;
    serializeJson(doc, requestBody);

    Serial.print("Enviando RA ");
    Serial.print(ra);
    Serial.println(" para o Node-RED...");

    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Resposta do Servidor (HTTP ");
      Serial.print(httpResponseCode);
      Serial.println("):");
      Serial.println(response);
    } else {
      Serial.print("Esse aluno não existe: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Erro: Wi-Fi desconectado!");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWi-Fi Conectado com sucesso!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
  Serial.println("\n--- PRONTO PARA USO ---");
  Serial.println("Digite o RA no Monitor Serial e pressione ENTER para registrar:");
}

void loop() {
  // Verifica se o usuário digitou algum RA no Monitor Serial
  if (Serial.available() > 0) {
    int raDigitado = Serial.parseInt(); // Lê o número digitado
    
    if (raDigitado > 0) {
      enviarPresenca(raDigitado);
    }
  }
}