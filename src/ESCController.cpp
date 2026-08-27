#include "ESCController.h"
#include "Config.h"

// ==========================================
// TEMPOS PARA ARMAR A RÉ
//
// Estes valores podem precisar de ajuste
// dependendo do seu ESC.
// ==========================================

#define TEMPO_NEUTRO_MS       300
#define TEMPO_FREIO_MS        300
#define TEMPO_ARMANDO_RE_MS   300

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

estado = NEUTRO;
tempoEstado = millis();

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

void ESCController::mudarEstado(
EstadoESC novoEstado
)
{
estado = novoEstado;
tempoEstado = millis();


Serial.print("[ESC] Estado: ");

switch (estado) {

    case NEUTRO:
        Serial.println("NEUTRO");
        break;

    case FRENTE:
        Serial.println("FRENTE");
        break;

    case FRENANDO:
        Serial.println("FREIO");
        break;

    case ARMANDO_RE:
        Serial.println("ARMANDO RE");
        break;

    case RE:
        Serial.println("RE");
        break;
}


}

void ESCController::setTarget(
float velocidade
)
{
velocidadeAlvo = constrain(
velocidade,
-100.0f,
100.0f
);
}

void ESCController::stop()
{
velocidadeAlvo = 0.0f;
}

void ESCController::update()
{
unsigned long agora = millis();


// ==========================================
// MÁQUINA DE ESTADOS
// ==========================================

switch (estado) {

    // --------------------------------------
    // NEUTRO
    // --------------------------------------

    case NEUTRO:

        writeMicroseconds(
            ESC_NEUTRAL_US
        );

        // Frente solicitada
        if (velocidadeAlvo > 0) {

            velocidadeAtual = 0;

            mudarEstado(FRENTE);
        }

        // Ré solicitada
        else if (velocidadeAlvo < 0) {

            velocidadeAtual = 0;

            mudarEstado(FRENANDO);
        }

        break;


    // --------------------------------------
    // FRENTE
    // --------------------------------------

    case FRENTE:
    {
        // Solicitação de ré
        if (velocidadeAlvo < 0) {

            velocidadeAtual = 0;

            mudarEstado(FRENANDO);

            break;
        }

        // Solicitação de parada
        if (velocidadeAlvo <= 0) {

            velocidadeAtual = max(
                velocidadeAtual - 2.0f,
                0.0f
            );

            if (velocidadeAtual <= 0.0f) {

                writeMicroseconds(
                    ESC_NEUTRAL_US
                );

                mudarEstado(NEUTRO);
            }
            else {

                int pulseUs = map(
                    (int)velocidadeAtual,
                    0,
                    100,
                    ESC_NEUTRAL_US,
                    ESC_FORWARD_US
                );

                writeMicroseconds(
                    pulseUs
                );
            }

            break;
        }

        // Aceleração normal
        if (
            velocidadeAtual < velocidadeAlvo
        ) {

            velocidadeAtual = min(
                velocidadeAtual + 0.5f,
                velocidadeAlvo
            );
        }
        else if (
            velocidadeAtual > velocidadeAlvo
        ) {

            velocidadeAtual = max(
                velocidadeAtual - 2.0f,
                velocidadeAlvo
            );
        }

        int pulseUs = map(
            (int)velocidadeAtual,
            0,
            100,
            ESC_NEUTRAL_US,
            ESC_FORWARD_US
        );

        writeMicroseconds(
            pulseUs
        );

        break;
    }


    // --------------------------------------
    // FREIO
    //
    // Envia sinal mínimo para o ESC
    // reconhecer o comando de frenagem.
    // --------------------------------------

    case FRENANDO:

        writeMicroseconds(
            ESC_REVERSE_US
        );

        if (
            agora - tempoEstado >=
            TEMPO_FREIO_MS
        ) {

            mudarEstado(
                ARMANDO_RE
            );
        }

        break;


    // --------------------------------------
    // ARMANDO RÉ
    //
    // Retorna ao neutro antes de entrar
    // efetivamente em marcha ré.
    // --------------------------------------

    case ARMANDO_RE:

        writeMicroseconds(
            ESC_NEUTRAL_US
        );

        if (
            agora - tempoEstado >=
            TEMPO_ARMANDO_RE_MS
        ) {

            // Se o usuário ainda quer ré,
            // entra no modo RÉ.
            if (velocidadeAlvo < 0) {

                velocidadeAtual = 0;

                mudarEstado(RE);
            }
            else {

                mudarEstado(NEUTRO);
            }
        }

        break;


    // --------------------------------------
    // RÉ
    // --------------------------------------

    case RE:
    {
        // Se o usuário pedir frente,
        // primeiro volta para neutro.
        if (velocidadeAlvo >= 0) {

            velocidadeAtual = 0;

            writeMicroseconds(
                ESC_NEUTRAL_US
            );

            mudarEstado(NEUTRO);

            break;
        }

        float alvoRe =
            abs(velocidadeAlvo);

        // Rampa da ré
        if (
            velocidadeAtual < alvoRe
        ) {

            velocidadeAtual = min(
                velocidadeAtual + 0.5f,
                alvoRe
            );
        }
        else if (
            velocidadeAtual > alvoRe
        ) {

            velocidadeAtual = max(
                velocidadeAtual - 2.0f,
                alvoRe
            );
        }

        int pulseUs = map(
            (int)velocidadeAtual,
            0,
            100,
            ESC_NEUTRAL_US,
            ESC_REVERSE_US
        );

        writeMicroseconds(
            pulseUs
        );

        break;
    }
}


}

float ESCController::getSpeed()
{
if (estado == RE) {
return -velocidadeAtual;
}


return velocidadeAtual;


}
