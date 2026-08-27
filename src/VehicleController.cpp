#include "VehicleController.h"
#include "Config.h"

void VehicleController::begin()
{
// Inicializa os módulos. Os atuadores são
// inicializados primeiro para manter o veículo
// em condição segura durante a conexão Wi-Fi.


servo.begin();
esc.begin();
mpu.begin();
network.begin();

Serial.println("Sistema pronto");


}

void VehicleController::update()
{
// ==============================================
// ATUALIZA REDE E SENSOR
// ==============================================


network.update();
mpu.update();

// ==============================================
// FAILSAFE
// ==============================================

if (!network.isConnected())
{
    // Sem comunicação recente:
    // - ESC vai para neutro;
    // - direção retorna ao centro.

    esc.stop();
    servo.setTarget(90.0f);
}
else
{
    // ==========================================
    // DIREÇÃO
    // ==========================================

    float direcao =
        network.getDirecao();

    // Aplica correção do MPU, caso esteja ativo.
    direcao -=
        mpu.getSteeringCorrection();

    servo.setTarget(direcao);

    // ==========================================
    // COMANDO DO ESC
    // ==========================================

    int acelerador =
        network.getAcelerador();

    int freio =
        network.getFreio();

    bool modoRe =
        network.getRe();

    // ------------------------------------------
    // FREIO
    // ------------------------------------------
    //
    // Por segurança, se o freio estiver
    // pressionado, o comando de movimento é
    // cancelado.
    //
    // ------------------------------------------

    if (freio > 10)
    {
        esc.stop();
    }

    // ------------------------------------------
    // MODO RÉ
    // ------------------------------------------
    //
    // R2 pressionado:
    //
    // acelerador 0%   -> 0%
    // acelerador 50%  -> -50%
    // acelerador 100% -> -100%
    //
    // O valor negativo informa ao ESCController
    // que a velocidade desejada é para trás.
    //
    // ------------------------------------------

    else if (modoRe)
    {
        esc.setTarget(
            -acelerador
        );
    }

    // ------------------------------------------
    // MODO FRENTE
    // ------------------------------------------

    else
    {
        esc.setTarget(
            acelerador
        );
    }
}

// ==============================================
// ATUALIZA OS ATUADORES
// ==============================================

servo.update();
esc.update();


}

void VehicleController::debug()
{
static unsigned long ultimoDebug = 0;


if (
    millis() - ultimoDebug <
    INTERVALO_DEBUG
) {
    return;
}

ultimoDebug = millis();

Serial.print("Direcao: ");
Serial.print(
    servo.getAngle(),
    1
);

Serial.print(" | ESC: ");
Serial.print(
    esc.getSpeed(),
    1
);

Serial.print("% | Re: ");
Serial.print(
    network.getRe() ? "SIM" : "NAO"
);

Serial.print(" | Rede: ");
Serial.println(
    network.isConnected()
        ? "OK"
        : "FAILSAFE"
);


}
