#include "MPUController.h"
#include "Config.h"

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

static Adafruit_MPU6050 mpu;

void MPUController::begin() {
    disponivel = false;
    correcaoDirecao = 0.0f;

    // Não inicializa I2C nem tenta acessar o sensor quando
    // o MPU está desativado na configuração.
    if (!MPU_ATIVADO) {
        Serial.println("MPU6050 desativado");
        return;
    }

    Wire.begin(MPU_SDA_PIN, MPU_SCL_PIN);

    if (!mpu.begin()) {
        Serial.println("ERRO: MPU6050 nao encontrado");
        return;
    }

    disponivel = true;

    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    calibrar();

    Serial.println("MPU6050 pronto");
}

// Mede o pequeno erro médio do giroscópio com o carrinho parado.
void MPUController::calibrar() {
    const int amostras = 300;
    float soma = 0.0f;

    Serial.println("Calibrando MPU6050 - mantenha parado");
    delay(1000);

    for (int i = 0; i < amostras; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        soma += g.gyro.z;
        delay(3);
    }

    gyroZOffset = soma / amostras;
}

void MPUController::update() {
    if (!MPU_ATIVADO || !disponivel) {
        correcaoDirecao = 0.0f;
        return;
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float gyroZ = g.gyro.z - gyroZOffset;

    // Zona morta para reduzir ruído do sensor.
    if (fabs(gyroZ) < 0.02f) {
        gyroZ = 0.0f;
    }

    // Ganho inicial conservador. Pode ser ajustado após
    // testes reais com o carrinho.
    const float ganho = 1.5f;

    correcaoDirecao = constrain(gyroZ * ganho, -20.0f, 20.0f);
}

bool MPUController::isAvailable() {
    return disponivel;
}

float MPUController::getSteeringCorrection() {
    return correcaoDirecao;
}
