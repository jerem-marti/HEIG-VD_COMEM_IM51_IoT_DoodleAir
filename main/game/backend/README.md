# Lancement du backend WebSocket
Le jeu nécessite un serveur WebSocket pour recevoir les données de l’ESP32. Si vous ne l’avez pas encore démarré, vous pouvez le faire avec :

```sh
cd backend
npm install  # Si ce n'est pas encore fait
npm run server
```
ou directement :

```sh
node server.js
```
Le serveur écoute par défaut sur le port `8080`, vous pouvez modifier ce paramètre dans `config.js`.