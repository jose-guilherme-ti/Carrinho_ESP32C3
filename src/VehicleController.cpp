#include "VehicleController.h"
#include "Config.h"

void VehicleController::begin() {
    // Inicializa os módulos. A rede é iniciada depois dos
    // atuadores para que o veículo já esteja em estado seguro.
    servo.begin();
    esc.begin();
    mpu.begin();
    network.begin();

    Serial.println("Sistema pronto");
}

void VehicleController::update() {
    // Sempre processa novos pacotes e atualiza o sensor.
    network.update();
    mpu.update();

    if (!network.isConnected()) {
        // Sem comunicação recente: condição segura.
        esc.stop();
        servo.setTarget(90.0f);
    } else {
        // Direção recebida do G29 mais a correção opcional.
        float direcao = network.getDirecao();
        direcao -= mpu.getSteeringCorrection();
        servo.setTarget(direcao);

        // Por enquanto o freio apenas corta o comando ao ESC.
        // ESCs bidirecionais podem receber uma lógica específica
        // de neutro, freio e ré futuramente.
        if (network.getFreio() > 10) {
            esc.stop();
        } else {
            esc.setTarget(network.getAcelerador());
        }
    }

    // Atualiza os valores físicos após calcular todos os alvos.
    servo.update();
    esc.update();
}

void VehicleController::debug() {
    static unsigned long ultimoDebug = 0;

    if (millis() - ultimoDebug < INTERVALO_DEBUG) {
        return;
    }

    ultimoDebug = millis();

    Serial.print("Direcao: ");
    Serial.print(servo.getAngle(), 1);

    Serial.print(" | ESC: ");
    Serial.print(esc.getSpeed(), 1);

    Serial.print("% | Rede: ");
    Serial.println(network.isConnected() ? "OK" : "FAILSAFE");
}
