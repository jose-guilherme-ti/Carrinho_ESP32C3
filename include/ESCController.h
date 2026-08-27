#ifndef ESC_CONTROLLER_H
#define ESC_CONTROLLER_H

#include <Arduino.h>

class ESCController
{
public:


void begin();

// -100 = ré máxima
//    0 = neutro
// +100 = frente máxima
void setTarget(float velocidade);

void stop();

void update();

float getSpeed();


private:


enum EstadoESC
{
    NEUTRO,
    FRENTE,
    FRENANDO,
    ARMANDO_RE,
    RE
};

EstadoESC estado = NEUTRO;

float velocidadeAtual = 0.0f;
float velocidadeAlvo = 0.0f;

unsigned long tempoEstado = 0;

uint32_t microsecondsToDuty(
    int microseconds
);

void writeMicroseconds(
    int microseconds
);

void mudarEstado(
    EstadoESC novoEstado
);


};

#endif
