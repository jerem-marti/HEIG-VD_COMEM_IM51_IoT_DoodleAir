#include "WSClient.h"

WSClient::WSClient(const char* host, uint16_t port)
    : websockets_server_host(host), websockets_server_port(port) {}

void WSClient::begin(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("🔄 Connexion WiFi...");
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\n✅ WiFi connecté !" : "\n❌ Échec WiFi !");

    Serial.println("🔗 Connexion WebSocket...");
    client.connect(websockets_server_host, websockets_server_port, "/");
}

void WSClient::checkConnection() {
    if (!client.available()) {
        Serial.println("🔄 Reconnexion WebSocket...");
        client.connect(websockets_server_host, websockets_server_port, "/");
    }
}

void WSClient::poll() {
    if (client.available()) {
        client.poll();
    }
}

void WSClient::send(String message) {
    if (message != "") {
        client.send(message);
    }
}
