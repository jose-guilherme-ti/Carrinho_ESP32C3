#ifndef VEHICLE_CONTROLLER_H
#define VEHICLE_CONTROLLER_H

#include "G29NetworkManager.h"
#include "ServoController.h"
#include "ESCController.h"
#include "MPUController.h"
#include "MobileController.h"

// =====================================================
// VehicleController
// =====================================================
// Classe principal da lógica do veículo. Conecta os
// comandos recebidos da rede aos atuadores e aplica:
// - failsafe;
// - correção do MPU;
// - lógica de freio;
// - atualização do servo e ESC.
class VehicleController {
private:
    G29NetworkManager network;
    ServoController servo;
    ESCController esc;
    MPUController mpu;
    MobileController mobile;

public:
    void begin();
    void update();
    void debug();
};

#endif
