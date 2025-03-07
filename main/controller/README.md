# Contrôleur - Maze Tilt

Ce dossier contient le code permettant de récupérer les données de l'IMU (accéléromètre + gyroscope) et de les envoyer à l'application via WebSocket.

## 📌 Déploiement

### 1️⃣ Prérequis

- **Carte ESP32 (Feather HUZZAH32 ou autre modèle compatible)**
- **Capteur IMU (MPU6050 ou ICM-20948)**
- **Plateforme de développement :** [PlatformIO](https://platformio.org/) ou Arduino IDE
- **Connexion WiFi disponible** pour transmettre les données via WebSocket

### 2️⃣ Installation des dépendances

Si vous utilisez **PlatformIO**, assurez-vous d’avoir les bibliothèques suivantes dans `platformio.ini` :

```ini
[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
lib_deps = 
	gilmaimon/ArduinoWebsockets@^0.5.4
	sparkfun/SparkFun 9DoF IMU Breakout - ICM 20948 - Arduino Library@^1.3.0
lib_ignore = WebServer
monitor_speed = 115200
```
Si vous utilisez Arduino IDE, installez ces bibliothèques via le gestionnaire de bibliothèques.

### 3️⃣ Compilation et flash du firmware
1. Connecter l’ESP32 à votre ordinateur via USB.
2. Configurer le WiFi en modifiant les variables dans `src/config.h` (voir ci-dessous).
3. Compiler et flasher le code sur l’ESP32 avec PlatformIO ou Arduino IDE.
```sh
# Avec PlatformIO
pio run --target upload
```
### 🔧 Configuration
Les variables suivantes doivent être ajustées dans `src/config.h` en fonction de votre réseau et serveur WebSocket.

| Variable         | Description |
|-----------------|-------------|
| `WIFI_SSID`     | Nom du réseau WiFi auquel l'ESP32 doit se connecter. |
| `WIFI_PASSWORD` | Mot de passe du réseau WiFi. |
| `WEBSOCKET_URL` | Adresse WebSocket où envoyer les données (ex: `ws://192.168.1.100:8080`). |
| `IMU_UPDATE_RATE` | Fréquence d’envoi des données en millisecondes. |

Exemple de `config.h` :
```cpp
#define WIFI_SSID "MonWiFi"
#define WIFI_PASSWORD "MotDePasse"
#define WEBSOCKET_URL "ws://192.168.1.100:8080"
#define IMU_UPDATE_RATE 50 // Envoie les données toutes les 50ms
```
### 🚀 Débogage
- Problème de connexion WiFi ? Vérifiez que WIFI_SSID et WIFI_PASSWORD sont corrects.
- WebSocket ne fonctionne pas ? Assurez-vous que le serveur WebSocket tourne bien sur l’ordinateur cible.
- ESP32 ne répond plus ? Effectuez un reset via le bouton EN sur la carte.
### 🛠 Améliorations possibles
- Ajouter une gestion avancée de la perte de connexion WiFi avec une reconnexion automatique plus robuste.
- Optimiser l’envoi des données pour réduire l’occupation du réseau.
- Tester d'autres capteurs IMU plus précis pour améliorer la stabilité.