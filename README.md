# ElectricPowerRecorder

Projet basé sur une clé USB [ESP32-S3-GEEK](https://www.waveshare.com/wiki/ESP32-S3-GEEK) de la Société [Waveshare](https://github.com/waveshareteam)<br>

![ESP32-S3-GEEK](EPowerRecorder-ScreenShot-Init.png)

## 🛄 Matériels supplémentaires utilisés
1. Capteur de courant AC 20A/1V SEN0211 (marquage SCT013 de la Société YHDC)
2. Convertisseur AC/DC avec son câble d'interface de 3 broches (3V3, GND et sortie analogique connectés respectivement au 3V3, GND et GPIO16 de l'ESP32-S3-GEEK)
3. Connecteur JST-SH1.0 femelle 4 broches pour l'alimentation en 3V3 + GND du convertisseur AC/DC + entrée analogique pour les connexions à l'ESP32-S3-GEEK

![ESP32-S3-GEEK](EPower-Modules-20250722.png)

## 🛠️ Environnement de développement
Arduino IDE 2.3.2 + package pour ESP32
(https://espressif.github.io/arduino-esp32/package_esp32_index.json)
