#include "ServoController.h"
#include "Config.h"

void ServoController::begin() {
    // Associa o pino a um PWM de 50 Hz com resolução de 16 bits.
    ledcAttach(
        SERVO_PIN,
        SERVO_FREQ,
        SERVO_RESOLUTION
    );

    anguloAtual = 90.0f;
    anguloAlvo = 90.0f;

    update();
}

// Em 50 Hz, um período completo possui 20.000 us.
uint32_t ServoController::microsecondsToDuty(int microseconds) {
    const uint32_t periodoUs = 20000;
    const uint32_t maxDuty = (1UL << SERVO_RESOLUTION) - 1;

    return ((uint64_t)microseconds * maxDuty) / periodoUs;
}

// Define o novo ângulo desejado. O movimento físico é
// realizado gradualmente dentro de update().
void ServoController::setTarget(float angulo) {
    anguloAlvo = constrain(angulo, 0.0f, 180.0f);
}

void ServoController::update() {
    // Suavização da direção para evitar movimentos bruscos.
    const float velocidade = 2.5f;

    if (anguloAtual < anguloAlvo) {
        anguloAtual = min(anguloAtual + velocidade, anguloAlvo);
    } else if (anguloAtual > anguloAlvo) {
        anguloAtual = max(anguloAtual - velocidade, anguloAlvo);
    }

    int pulseUs = map(
        (int)anguloAtual,
        0, 180,
        SERVO_MIN_US,
        SERVO_MAX_US
    );

    ledcWrite(SERVO_PIN, microsecondsToDuty(pulseUs));
}

float ServoController::getAngle() {
    return anguloAtual;
}
