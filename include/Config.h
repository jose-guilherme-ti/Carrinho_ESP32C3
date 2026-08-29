#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// CONFIGURAÇÃO CENTRAL DO PROJETO
// =====================================================

// Altere aqui os pinos e parâmetros gerais.
// Os demais arquivos utilizam estas constantes.

// ========================================
// CONFIGURAÇÃO DA REDE WI-FI
// ========================================

#define WIFI_SSID     "Jose-Planet/3121433_2.4G"
#define WIFI_PASSWORD "Guilherme1659"

// ---------- IP ESTÁTICO ----------

// Ajuste estes valores conforme a sua rede.
#define WIFI_STATIC_IP   IPAddress(192, 168, 0, 108)
#define WIFI_GATEWAY     IPAddress(192, 168, 0, 1)
#define WIFI_SUBNET      IPAddress(255, 255, 255, 0)
#define WIFI_DNS1        IPAddress(192, 168, 0, 1)
#define WIFI_DNS2        IPAddress(8, 8, 8, 8)

// ---------- Wi-Fi / UDP ----------

#define UDP_PORT      4210

// ---------- Controle pelo celular ----------
// Porta do servidor web. A porta 80 permite acessar
// somente pelo IP, por exemplo: http://192.168.0.108
#define MOBILE_HTTP_PORT      80

// Tempo máximo sem comandos do navegador antes de
// devolver o controle ao G29/failsafe.
#define TEMPO_MOBILE_FAILSAFE 500

// ========================================
// PINOS
// ========================================

// ESP32-C3
#define SERVO_PIN     5
#define ESC_PIN       6

// MPU6050
#define MPU_SDA_PIN   8
#define MPU_SCL_PIN   9

// ========================================
// MPU6050
// ========================================

// true  = inicializa e usa o MPU6050
// false = ignora completamente o MPU6050

#define MPU_ATIVADO   false

// ========================================
// SERVO
// ========================================

#define SERVO_FREQ        50
#define SERVO_RESOLUTION  14

#define SERVO_MIN_US      500
#define SERVO_MAX_US      2400


// Ajuste fino do centro da direção.
// Valor positivo ou negativo em graus.

#define SERVO_CENTER_OFFSET -13

// true = inverte o sentido da direção

#define SERVO_INVERTIDO     true

// ========================================
// ESC
// ========================================

// Valores típicos para ESC RC.
//
// 1000 us = motor parado
// 2000 us = aceleração máxima
//
// Confirme sempre os valores no manual do ESC.

#define ESC_FREQ          50
#define ESC_RESOLUTION    14

// Faixa padrão de controle RC
#define ESC_REVERSE_US      1000
#define ESC_NEUTRAL_US      1500
#define ESC_FORWARD_US      2000

// Pequena zona morta ao redor do neutro
#define ESC_DEADZONE        3

// ========================================
// SEGURANÇA
// ========================================

#define TEMPO_FAILSAFE    500

// ========================================
// DEBUG
// ========================================

#define INTERVALO_DEBUG   200

#endif