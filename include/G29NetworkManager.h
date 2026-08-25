#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// =====================================================
// G29NetworkManager
// =====================================================
// Cria o Access Point do ESP32 e recebe pacotes UDP
// enviados pelo programa Python.
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
    char packetBuffer[128];

    int direcao = 90;
    int acelerador = 0;
    int freio = 0;

    unsigned long ultimoPacote = 0;

public:
    void begin();
    void update();

    // Retorna false quando não chegam pacotes dentro
    // do tempo definido para o failsafe.
    bool isConnected();

    int getDirecao();
    int getAcelerador();
    int getFreio();
};

#endif
