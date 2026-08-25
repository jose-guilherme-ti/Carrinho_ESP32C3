#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// CONFIGURAÇÃO CENTRAL DO PROJETO
// =====================================================
// Altere aqui os pinos e parâmetros gerais. Os demais
// arquivos utilizam estas constantes para evitar valores
// espalhados pelo código.

// ---------- Wi-Fi / UDP ----------
#define WIFI_SSID       "Carrinho_G29"
#define WIFI_PASSWORD   "12345678"
#define UDP_PORT        4210

// ---------- Pinos ----------
// Confirme a disponibilidade destes GPIOs no seu ESP32-C3.
#define SERVO_PIN       5
#define ESC_PIN         6

// Canais LEDC separados para a API clássica do Arduino-ESP32.
#define SERVO_CHANNEL   0
#define ESC_CHANNEL     1
#define MPU_SDA_PIN     8
#define MPU_SCL_PIN     9

// ---------- MPU6050 ----------
// true  = inicializa e usa o MPU6050.
// false = ignora completamente o MPU6050.
#define MPU_ATIVADO     false

// ---------- Servo ----------
#define SERVO_FREQ          50
#define SERVO_RESOLUTION    16
#define SERVO_MIN_US        500
#define SERVO_MAX_US        2400

// ---------- ESC ----------
// Valores típicos para ESC RC. Confirme o manual do ESC.
// Neste projeto, 1000 us representa motor parado e
// 2000 us representa aceleração máxima.
#define ESC_FREQ            50
#define ESC_RESOLUTION      16
#define ESC_MIN_US          1000
#define ESC_MAX_US          2000

// ---------- Segurança ----------
#define TEMPO_FAILSAFE      500

// ---------- Debug ----------
#define INTERVALO_DEBUG     200

#endif
