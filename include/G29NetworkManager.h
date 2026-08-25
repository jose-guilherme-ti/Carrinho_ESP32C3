#ifndef G29_NETWORK_MANAGER_H
#define G29_NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "Config.h"

// =====================================================
// G29NetworkManager
// =====================================================
//
// Conecta o ESP32-C3 ao roteador Wi-Fi e recebe pacotes
// UDP enviados pelo programa Python.
//
// Formato esperado:
// S:90,A:50,F:0
//
// S = direção (0 a 180)
// A = acelerador (0 a 100)
// F = freio (0 a 100)
class G29NetworkManager {
private:
    WiFiUDP udp;

    // Buffer utilizado para receber os pacotes UDP.
    char packetBuffer[128];

    // Últimos comandos válidos recebidos.
    int direcao = 90;
    int acelerador = 0;
    int freio = 0;

    // Momento do último pacote válido.
    unsigned long ultimoPacote = 0;

    // Controle da tentativa de reconexão Wi-Fi.
    unsigned long ultimaTentativaWiFi = 0;

    // Intervalo entre tentativas de reconexão.
    static constexpr unsigned long INTERVALO_RECONEXAO = 5000;

    // Controla se o socket UDP já foi iniciado.
    bool udpIniciado = false;

    void connectWiFi();
    void reconnectWiFi();

public:
    void begin();
    void update();

    // Retorna false se o Wi-Fi estiver desconectado ou
    // se não chegar comando dentro do tempo de failsafe.
    bool isConnected();

    int getDirecao();
    int getAcelerador();
    int getFreio();

    bool isWiFiConnected();
};

#endif