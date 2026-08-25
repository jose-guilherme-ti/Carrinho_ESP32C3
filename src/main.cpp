#include <Arduino.h>
#include "VehicleController.h"
// =====================================================
// ARQUIVO PRINCIPAL
// =====================================================
// Mantido propositalmente pequeno. Toda a lógica do
// veículo está encapsulada em VehicleController.
VehicleController vehicle;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" CARRINHO G29 - ESP32-C3");
    Serial.println("================================");
    vehicle.begin();
}

void loop() {
    vehicle.update();
    vehicle.debug();

    // Pequeno intervalo para não ocupar a CPU
    // desnecessariamente.
    delay(5);
}
