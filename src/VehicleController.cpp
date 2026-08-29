#include "VehicleController.h"
#include "Config.h"

// =====================================================
// Inicialização geral do veículo.
// =====================================================
void VehicleController::begin()
{
    // Inicializa os módulos. Os atuadores são
    // inicializados primeiro para manter o veículo
    // em condição segura durante a conexão Wi-Fi.
    servo.begin();
    esc.begin();
    mpu.begin();
    network.begin();
    mobile.begin();

    Serial.println("Sistema pronto");
}

void VehicleController::update()
{
    // ==============================================
    // ATUALIZA FONTES DE COMANDO E SENSOR
    // ==============================================
    network.update();
    mobile.update();
    mpu.update();

    // O celular tem prioridade enquanto estiver enviando
    // comandos. Quando parar, o sistema volta a aceitar o
    // G29 automaticamente.
    bool usandoMobile = mobile.isActive();
    bool usandoG29 = !usandoMobile && network.isConnected();

    // ==============================================
    // FAILSAFE
    // ==============================================
    if (!usandoMobile && !usandoG29)
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
        // SELECIONA A FONTE ATIVA
        // ==========================================
        int direcao;
        int acelerador;
        int freio;
        bool modoRe;

        if (usandoMobile)
        {
            direcao = mobile.getDirecao();
            acelerador = mobile.getAcelerador();
            freio = mobile.getFreio();
            modoRe = mobile.getRe();
        }
        else
        {
            direcao = network.getDirecao();
            acelerador = network.getAcelerador();
            freio = network.getFreio();
            modoRe = network.getRe();
        }

        // ==========================================
        // DIREÇÃO
        // ==========================================
        float direcaoCorrigida = direcao;

        // Aplica correção do MPU, caso esteja ativo.
        direcaoCorrigida -= mpu.getSteeringCorrection();
        servo.setTarget(direcaoCorrigida);

        // ==========================================
        // COMANDO DO ESC
        // ==========================================
        // Por segurança, se o freio estiver pressionado,
        // o comando de movimento é cancelado.
        if (freio > 10)
        {
            esc.stop();
        }
        // RÉ: o valor negativo informa ao ESCController
        // que a velocidade desejada é para trás.
        else if (modoRe)
        {
            esc.setTarget(-acelerador);
        }
        // MODO FRENTE
        else
        {
            esc.setTarget(acelerador);
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

    if (millis() - ultimoDebug < INTERVALO_DEBUG) return;
    ultimoDebug = millis();

    Serial.print("Direcao: ");
    Serial.print(servo.getAngle(), 1);
    Serial.print(" | ESC: ");
    Serial.print(esc.getSpeed(), 1);
    Serial.print("% | Fonte: ");
    Serial.print(mobile.isActive() ? "CELULAR" :
                 (network.isConnected() ? "G29" : "FAILSAFE"));
    Serial.print(" | Re: ");
    Serial.println(mobile.isActive() ?
        (mobile.getRe() ? "SIM" : "NAO") :
        (network.getRe() ? "SIM" : "NAO"));
}
