#ifndef MOBILE_CONTROLLER_H
#define MOBILE_CONTROLLER_H

#include <Arduino.h>
#include <WebServer.h>
#include "Config.h"

// =====================================================
// MobileController
// =====================================================
// Hospeda uma interface responsiva para controle pelo
// celular. O acesso à página ativa o modo MOBILE e os
// comandos recebidos substituem temporariamente o G29.
//
// A interface envia comandos HTTP curtos e periódicos,
// mantendo a comunicação simples e compatível com o
// WebServer nativo do Arduino-ESP32.
// =====================================================
class MobileController {
private:
    WebServer server;

    int direcao = 90;
    int acelerador = 0;
    int freio = 0;
    bool re = false;
    bool paginaAberta = false;

    unsigned long ultimoComando = 0;

    void handleRoot();
    void handleCommand();
    void handleStop();
    void handleStatus();

public:
    MobileController();

    void begin();
    void update();

    // Retorna true enquanto o navegador estiver enviando
    // comandos dentro do intervalo de segurança.
    bool isActive();

    int getDirecao();
    int getAcelerador();
    int getFreio();
    bool getRe();
};

#endif
