#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>

// =====================================================
// ServoController
// =====================================================
// Controla o servo de direção usando o periférico LEDC
// do ESP32, sem depender da biblioteca ESP32Servo.
class ServoController {
private:
    float anguloAtual = 90.0f;
    float anguloAlvo = 90.0f;

    // Converte largura de pulso em microssegundos para
    // duty cycle do PWM configurado em 50 Hz.
    uint32_t microsecondsToDuty(int microseconds);

public:
    void begin();
    void setTarget(float angulo);
    void update();
    float getAngle();
};

#endif
