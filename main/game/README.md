# Maze Tilt - Déploiement du Jeu

Ce dossier contient le code du jeu **Maze Tilt**, qui utilise **Vue.js** et **Three.js** pour la simulation du labyrinthe.

![Maze Tilt - Home](../../docs/assets/photos/accueil-game_tilt_maze.png)

## 📦 Installation et lancement

### 1️⃣ Prérequis

- [Node.js](https://nodejs.org/) installé sur votre machine
- [npm](https://www.npmjs.com/) (inclus avec Node.js)
- Un serveur WebSocket pour recevoir les données de l’ESP32

### 2️⃣ Installation des dépendances

Clonez le projet, puis exécutez :

```sh
cd game
npm install
```

### 3️⃣ Lancement en mode développement
```sh
npm run dev
```
Cela démarre un serveur local sur `http://localhost:5173/` (par défaut) où vous pouvez tester le jeu.

### 4️⃣ Lancement du backend WebSocket
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

### 5️⃣ Construction pour la production
Si vous souhaitez générer les fichiers prêts pour la mise en production :

```sh
npm run build
```
Les fichiers seront placés dans le dossier `/dist`, prêts à être déployés sur un serveur.

## ⚙️ Configuration
### 🌐 WebSocket
Par défaut, le jeu récupère les données de l’ESP32 via un serveur WebSocket.

Si besoin, modifiez l’URL du WebSocket dans `src/config.js` :

```js
export const WEBSOCKET_URL = "ws://localhost:3000";
```
Remplacez `"localhost:3000"` par l’adresse de votre serveur si nécessaire.

## 🔧 Variables importantes
| Variable       | Emplacement         | Description |
|---------------|--------------------|-------------|
| `WEBSOCKET_URL` | `src/config.js`    | Adresse du serveur WebSocket à modifier si besoin. |
| `MAZE_SIZE`    | `src/gameConfig.js` | Dimensions du labyrinthe en pixels. |
| `SCALE`        | `src/gameConfig.js` | Échelle d’affichage du labyrinthe dans Three.js. |
| `ballRadius`   | `src/gameConfig.js` | Rayon de la bille, influençant les collisions. |
| `gravity`      | `src/gameConfig.js` | Intensité de la gravité appliquée à la bille. |
| `maxSpeed`     | `src/gameConfig.js` | Vitesse maximale autorisée pour éviter une accélération incontrôlée. |

## 🛠 Déploiement sur un serveur
Si vous souhaitez héberger le jeu sur un serveur web (par exemple Vercel, Netlify, ou un serveur Apache/Nginx), vous pouvez :

1. Construire le projet :
```sh
npm run build
```
2. Déployer le contenu du dossier /dist sur votre hébergement.
Si vous utilisez Vercel :
```sh
npm install -g vercel
vercel
```
Si vous utilisez Netlify :
```sh
npm install -g netlify-cli
netlify deploy
```
## 🚀 Améliorations possibles
- Ajouter un mode plein écran pour une immersion totale.
- Intégrer un mode multijoueur avec synchronisation en temps réel.
- Optimiser le rendu pour les appareils mobiles.
```go
Ce `README.md` couvre l’installation, le lancement en mode développement, la mise en production et la configuration des variables essentielles.
```