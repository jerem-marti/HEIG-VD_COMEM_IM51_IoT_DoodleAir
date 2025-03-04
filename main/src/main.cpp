#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <Wire.h>
#include "ICM_20948.h"  // Bibliothèque du capteur IMU

using namespace websockets;

// 🔹 Configuration WiFi et WebSocket
const char* ssid = "J";  
const char* password = "DEUX * deux = 4"; 
const char* websockets_server_host = "192.168.15.13"; 
const uint16_t websockets_server_port = 8080; 

WebsocketsClient client;
ICM_20948_I2C myICM;  // Objet IMU

// 🔹 Pins des boutons et du potentiomètre
#define BUTTON_COLOR 26
#define BUTTON_ERASE 36   
#define POT_PIN 34   

// 🔹 État des boutons pour éviter les envois multiples
bool lastButtonColorState = HIGH;
bool lastButtonEraseState = HIGH;
unsigned long lastIMUSendTime = 0;
unsigned long lastPotSendTime = 0;
unsigned long lastButtonSendTime = 0;

// 📡 Vérifie et reconnecte le WiFi si nécessaire
void reconnectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("📶 WiFi perdu, tentative de reconnexion...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
            Serial.print(".");
            delay(1000);
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n✅ WiFi reconnecté !");
        } else {
            Serial.println("\n❌ Impossible de se reconnecter au WiFi.");
        }
    }
}

// 🔄 Vérifie la connexion WebSocket et reconnecte si nécessaire
void checkWebSocketConnection() {
    if (!client.available()) {
        Serial.println("🔄 Reconnexion WebSocket en cours...");
        bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
        if (connected) {
            Serial.println("✅ Reconnecté au WebSocket !");
            client.send("Reconnexion ESP32 !");
        } else {
            Serial.println("⚠️ Échec de la reconnexion WebSocket...");
        }
    }
}

// 📩 Callback exécuté lorsqu'un message est reçu
void onMessageCallback(WebsocketsMessage message) {
    Serial.print("📩 Message reçu : ");
    Serial.println(message.data());
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // 🔹 Configuration des boutons (PULL-UP externes donc pas besoin d'INPUT_PULLUP)
    pinMode(BUTTON_COLOR, INPUT);
    pinMode(BUTTON_ERASE, INPUT);

    // 🔹 Initialisation du IMU
    if (myICM.begin(Wire, 0x68) == false) {
        Serial.println("⚠️ Échec de la détection du IMU !");
    } else {
        Serial.println("✅ IMU détecté !");
    }

    // 🔹 Connexion WiFi
    WiFi.begin(ssid, password);
    Serial.print("🔄 Connexion WiFi...");
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n❌ Impossible de se connecter au WiFi !");
        return;
    }
    Serial.println("\n✅ WiFi connecté !");
    Serial.print("🖥️ Adresse IP ESP32: ");
    Serial.println(WiFi.localIP());

    // 🔹 Connexion WebSocket
    Serial.println("🔗 Connexion au serveur WebSocket...");
    bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
    if (connected) {
        Serial.println("✅ Connecté au WebSocket !");
        client.send("ESP32 connecté !");
    } else {
        Serial.println("❌ Impossible de se connecter au WebSocket !");
    }

    client.onMessage(onMessageCallback);
}

void loop() {
    reconnectWiFi();
    checkWebSocketConnection();

    // 🔹 Vérifie les messages WebSocket
    if (client.available()) {
        client.poll();
    }

    unsigned long now = millis();

    // 🔹 Envoie les données du IMU toutes les 500 ms
    if (now - lastIMUSendTime > 500) {
        lastIMUSendTime = now;
        if (myICM.dataReady()) {
            myICM.getAGMT();
            String imuData = "{\"accelX\":" + String(myICM.accX()) + 
                             ",\"accelY\":" + String(myICM.accY()) + 
                             ",\"accelZ\":" + String(myICM.accZ()) + 
                             ",\"gyroX\":" + String(myICM.gyrX()) + 
                             ",\"gyroY\":" + String(myICM.gyrY()) + 
                             ",\"gyroZ\":" + String(myICM.gyrZ()) + "}";
            client.send(imuData);
            Serial.println("📡 IMU envoyé : " + imuData);
        }
    }

    // 🔹 Envoie la valeur du potentiomètre toutes les 500 ms
    if (now - lastPotSendTime > 500) {
        lastPotSendTime = now;
        int potValue = analogRead(POT_PIN);
        String potData = "{\"potentiometer\":" + String(potValue) + "}";
        client.send(potData);
        Serial.println("🎛️ Potentiomètre envoyé : " + potData);
    }

    // 🔹 Vérifie l'état des boutons toutes les 100 ms
    if (now - lastButtonSendTime > 100) {
        lastButtonSendTime = now;

        bool buttonColorState = digitalRead(BUTTON_COLOR);
        bool buttonEraseState = digitalRead(BUTTON_ERASE);

        if (buttonColorState == LOW && lastButtonColorState == HIGH) {
            client.send("{\"button\": \"color\", \"state\": \"pressed\"}");
            Serial.println("🟡 Bouton COLOR pressé !");
        }
        if (buttonColorState == HIGH && lastButtonColorState == LOW) {
            client.send("{\"button\": \"color\", \"state\": \"released\"}");
            Serial.println("🟡 Bouton COLOR relâché !");
        }

        if (buttonEraseState == LOW && lastButtonEraseState == HIGH) {
            client.send("{\"button\": \"erase\", \"state\": \"pressed\"}");
            Serial.println("🔴 Bouton ERASE pressé !");
        }
        if (buttonEraseState == HIGH && lastButtonEraseState == LOW) {
            client.send("{\"button\": \"erase\", \"state\": \"released\"}");
            Serial.println("🔴 Bouton ERASE relâché !");
        }

        lastButtonColorState = buttonColorState;
        lastButtonEraseState = buttonEraseState;
    }

    delay(50);
}
