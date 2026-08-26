#include "ESCController.h"
#include "Config.h"

void ESCController::begin()
{
    Serial.println("[ESC] Inicializando...");

    bool ok = ledcAttach(
        ESC_PIN,
        ESC_FREQ,
        ESC_RESOLUTION
    );

    if (!ok) {
        Serial.println("[ESC] ERRO: ledcAttach falhou!");
        return;
    }

    velocidadeAtual = 0.0f;
    velocidadeAlvo = 0.0f;

    // ESC bidirecional:
    // 1500 us representa a posição neutra.
    writeMicroseconds(ESC_NEUTRAL_US);

    Serial.println("[ESC] Inicializado em NEUTRO");
}

uint32_t ESCController::microsecondsToDuty(
    int microseconds
)
{
    const uint32_t periodoUs =
        1000000UL / ESC_FREQ;

    const uint32_t maxDuty =
        (1UL << ESC_RESOLUTION) - 1;

    return (
        (uint64_t)microseconds * maxDuty
    ) / periodoUs;
}

void ESCController::writeMicroseconds(
    int microseconds
)
{
    microseconds = constrain(
        microseconds,
        ESC_REVERSE_US,
        ESC_FORWARD_US
    );

    uint32_t duty =
        microsecondsToDuty(microseconds);

    ledcWrite(ESC_PIN, duty);
}

void ESCController::setTarget(float velocidade)
{
    velocidadeAlvo = constrain(
        velocidade,
        0.0f,
        100.0f
    );
}

void ESCController::stop()
{
    velocidadeAlvo = 0.0f;
}

void ESCController::update()
{
    // Rampa inicial segura.
    const float subida = 0.5f;
    const float descida = 2.0f;

    if (velocidadeAtual < velocidadeAlvo) {

        velocidadeAtual = min(
            velocidadeAtual + subida,
            velocidadeAlvo
        );

    } else if (velocidadeAtual > velocidadeAlvo) {

        velocidadeAtual = max(
            velocidadeAtual - descida,
            velocidadeAlvo
        );
    }

    // 0%   = 1500 us = neutro
    // 100% = 2000 us = frente máxima
    int pulseUs = map(
        (int)velocidadeAtual,
        0,
        100,
        ESC_NEUTRAL_US,
        ESC_FORWARD_US
    );

    writeMicroseconds(pulseUs);
}

float ESCController::getSpeed()
{
    return velocidadeAtual;
}