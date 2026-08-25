#ifndef ESC_CONTROLLER_H
#define ESC_CONTROLLER_H

#include <Arduino.h>

// =====================================================
// ESCController
// =====================================================
// Envia sinal PWM de padrão RC para um ESC de motor
// brushed. Esta versão trabalha inicialmente apenas com
// aceleração para frente, de 0 a 100%.
class ESCController {
private:
    float velocidadeAtual = 0.0f;
    float velocidadeAlvo = 0.0f;

    uint32_t microsecondsToDuty(int microseconds);

public:
    void begin();
    void setTarget(float velocidade);
    void stop();
    void update();
    float getSpeed();
};

#endif
