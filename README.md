# Robot Éviteur d'Obstacle avec Communication MQTT

Ce projet implémente un robot éviteur d'obstacle avec une communication MQTT en utilisant un ESP32.

---

## Prérequis

### Matériel
- **ESP32** : Microcontrôleur pour la communication MQTT.
- **Capteurs à ultrason** : Pour détecter les obstacles.
- **Moteurs et contrôleur de moteurs** : Pour le mouvement du robot.
- **Batterie** : Source d'alimentation pour le robot.

### Logiciel
- **Mosquitto** : Broker MQTT pour la gestion des messages.
- **Node-RED** : Pour la visualisation et le contrôle des données.

---

## Commandes pour Lancer le Projet

### 1. Installer Mosquitto

- Assurez-vous que Mosquitto est installé sur votre machine.
- Configurez le fichier `mosquitto.conf` pour permettre les connexions distantes si nécessaire.
- Lancer le broker avec :
  ```bash
  mosquitto -c mosquitto.conf
  ```

### 2. Lancer Node-RED

- Assurez-vous que Node-RED est installé.
- Démarrez Node-RED avec :
  ```bash
  node-red
  ```
- Accédez à l'interface sur : [http://localhost:1880](http://localhost:1880)

### 3. Programmer l'ESP32

- Chargez le code ESP32 via l'IDE Arduino ou un autre IDE compatible.
- Configurez les crédentials Wi-Fi et le broker MQTT dans le code.
- Déployez le firmware sur l'ESP32.

---

## Fonctionnalités

1. **Évitement d'obstacle** :
   - Le robot détecte les obstacles grâce aux capteurs à ultrason.
   - Les moteurs sont contrôlés pour adapter la trajectoire.

2. **Communication MQTT** :
   - Les données des capteurs sont publiées sur un topic MQTT.
   - Les commandes peuvent être reçues pour contrôler le robot à distance.

3. **Visualisation avec Node-RED** :
   - Les messages publiés par l'ESP32 sont affichés dans des graphiques.
   - Les commandes peuvent être envoyées directement depuis l'interface.

---

## Structure du Projet

- **ESP32** :
  - Code source pour la gestion des capteurs et la communication MQTT.
- **Mosquitto** :
  - Configuration du broker MQTT.
- **Node-RED** :
  - Flux Node-RED pour l'affichage et le contrôle des données.
- **Documentation** :
  - Instructions pour le déploiement et l'utilisation du projet.

---

## Schéma du Projet

1. **Capteurs à ultrason** : Connectés à l'ESP32 pour mesurer les distances.
2. **ESP32** : Publie les distances et reçoit les commandes via MQTT.
3. **Mosquitto** : Broker pour la communication entre l'ESP32 et Node-RED.
4. **Node-RED** : Visualise les données et permet le contrôle à distance.

---

## Exemple de Flux Node-RED

1. **Détection des obstacles** :
   - Les distances sont affichées dans un graphique en temps réel.
2. **Contrôle du robot** :
   - Des boutons permettent d'envoyer des commandes comme avancer, reculer ou tourner.

---

## Installation Supplémentaire

- Installer des bibliothèques Arduino pour MQTT et les capteurs :
  - `PubSubClient` pour MQTT.
  - `NewPing` ou autre pour les capteurs ultrason.

---

## Auteur

- **Nom** : Meddeb
- **Prénom** : Amine  

---

## Licence

Ce projet est sous licence MIT. Consultez le fichier `LICENSE` pour plus d'informations.

