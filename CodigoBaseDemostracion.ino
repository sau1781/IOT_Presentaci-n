#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ArduinoJson.h>

// ==========================================
// 1. CONFIGURACIÓN DE RED Y THINGSBOARD
// ==========================================
const char* ssid = "TU_SSID_WIFI";             // <-- Pon el nombre de tu red WiFi (2.4 GHz)
const char* password = "TU_PASSWORD_WIFI";     // <-- Pon la contraseña de tu WiFi
const char* token = "TU_ACCESS_TOKEN";         // <-- Pon el Access Token de ThingsBoard

const char* mqtt_server = "thingsboard.cloud";
const int mqtt_port = 1883;

// ==========================================
// 2. DEFINICIÓN DE PINES Y VARIABLES
// ==========================================
const int DHT_PIN = 15; // Sensor DHT22 en GPIO 15
const int LED_PIN = 13; // LED en GPIO 13

DHTesp dhtSensor;
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
bool ledState = false;
bool manualOverride = false; // Evita que la regla automática sobrescriba tu comando RPC

// ==========================================
// 3. FUNCIÓN PARA ENVIAR TELEMETRÍA
// ==========================================
void sendTelemetry(float temp, float hum) {
  StaticJsonDocument<256> doc;
  doc["uptime_ms"] = millis();
  doc["free_heap_bytes"] = ESP.getFreeHeap();
  doc["temperature"] = serialized(String(temp, 1));
  doc["humidity"] = serialized(String(hum, 1));
  doc["led_status"] = ledState ? 1 : 0;

  char buffer[256];
  serializeJson(doc, buffer);

  client.publish("v1/devices/me/telemetry", buffer);
  Serial.print("Telemetría MQTT enviada: ");
  Serial.println(buffer);
}

// ==========================================
// 4. CALLBACK RPC (RECIBE COMANDOS DE THINGSBOARD)
// ==========================================
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("Error al leer JSON RPC: ");
    Serial.println(error.c_str());
    return;
  }

  String method = doc["method"].as<String>();

  if (method == "set_led") {
    manualOverride = true; // Da prioridad al interruptor del Dashboard

    // Soporta tanto booleanos (true/false) como enteros (1/0)
    if (doc["params"].is<bool>()) {
      ledState = doc["params"].as<bool>();
    } else if (doc["params"].is<int>()) {
      ledState = (doc["params"].as<int>() == 1);
    }

    digitalWrite(LED_PIN, ledState ? HIGH : LOW);

    if (ledState) {
      Serial.println("-> RPC: LED ENCENDIDO");
    } else {
      Serial.println("-> RPC: LED APAGADO");
    }

    // 1. Responder al request RPC para evitar error de Timeout en el Switch
    String topicStr = String(topic);
    String requestId = topicStr.substring(topicStr.lastIndexOf('/') + 1);
    String responseTopic = "v1/devices/me/rpc/response/" + requestId;
    
    String responsePayload = ledState ? "true" : "false";
    client.publish(responseTopic.c_str(), responsePayload.c_str());

    // 2. Actualizar inmediatamente led_status en la telemetría
    StaticJsonDocument<64> telDoc;
    telDoc["led_status"] = ledState ? 1 : 0;
    char telBuffer[64];
    serializeJson(telDoc, telBuffer);
    client.publish("v1/devices/me/telemetry", telBuffer);
  }
}

// ==========================================
// 5. CONEXIÓN WIFI Y RECONEXIÓN MQTT
// ==========================================
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡WiFi conectado exitosamente!");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a ThingsBoard MQTT...");
    String clientId = "ESP32_Node_" + String(random(0xffff), HEX);

    // En ThingsBoard el Access Token va como usuario MQTT (sin contraseña)
    if (client.connect(clientId.c_str