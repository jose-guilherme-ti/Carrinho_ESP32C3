#include "ESCController.h"
#include "Config.h"

void ESCController::begin() {
    // O ESC recebe PWM semelhante ao de um servo RC: 50 Hz.
    ledcSetup(ESC_CHANNEL, ESC_FREQ, ESC_RESOLUTION);
    ledcAttachPin(ESC_PIN, ESC_CHANNEL);

    velocidadeAtual = 0.0f;
    velocidadeAlvo = 0.0f;

    // Garante sinal mínimo/parado durante a inicialização.
    update();

    Serial.println("ESC configurado em sinal minimo");
}

uint32_t ESCController::microsecondsToDuty(int microseconds) {
    const uint32_t periodoUs = 20000;
    const uint32_t maxDuty = (1UL << ESC_RESOLUTION) - 1;

    return ((uint64_t)microseconds * maxDuty) / periodoUs;
}

// Define a velocidade desejada em porcentagem.
void ESCController::setTarget(float velocidade) {
    velocidadeAlvo = constrain(velocidade, 0.0f, 100.0f);
}

// Solicita parada. A desaceleração real ocorre em update().
void ESCController::stop() {
    velocidadeAlvo = 0.0f;
}

void ESCController::update() {
    // Rampas independentes tornam a resposta mais suave.
    const float subida = 1.5f;
    const float descida = 4.0f;

    if (velocidadeAtual < velocidadeAlvo) {
        velocidadeAtual = min(velocidadeAtual + subida, velocidadeAlvo);
    } else if (velocidadeAtual > velocidadeAlvo) {
        velocidadeAtual = max(velocidadeAtual - descida, velocidadeAlvo);
    }

    int pulseUs = map(
        (int)velocidadeAtual,
        0, 100,
        ESC_MIN_US,
        ESC_MAX_US
    );

    ledcWrite(ESC_PIN, microsecondsToDuty(pulseUs));
}

float ESCController::getSpeed() {
    return velocidadeAtual;
}
