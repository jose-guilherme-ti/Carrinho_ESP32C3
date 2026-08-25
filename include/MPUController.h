#ifndef MPU_CONTROLLER_H
#define MPU_CONTROLLER_H

#include <Arduino.h>

// =====================================================
// MPUController
// =====================================================
// Inicializa e lê o MPU6050 apenas quando MPU_ATIVADO
// estiver definido como true em Config.h.
//
// Atualmente fornece uma correção simples baseada na
// velocidade angular do eixo Z. A classe foi isolada
// para permitir futuramente filtros e PID.
class MPUController {
private:
    bool disponivel = false;
    float gyroZOffset = 0.0f;
    float correcaoDirecao = 0.0f;

    void calibrar();

public:
    void begin();
    void update();

    bool isAvailable();
    float getSteeringCorrection();
};

#endif
