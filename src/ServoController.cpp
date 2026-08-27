#include "ServoController.h"
#include "Config.h"

void ServoController::begin() {

  Serial.println("[SERVO] Configurando PWM...");

  bool ok = ledcAttach(
    SERVO_PIN,
    SERVO_FREQ,
    SERVO_RESOLUTION);

  Serial.print("[SERVO] GPIO: ");
  Serial.println(SERVO_PIN);

  Serial.print("[SERVO] Frequencia: ");
  Serial.println(SERVO_FREQ);

  Serial.print("[SERVO] Resolucao: ");
  Serial.println(SERVO_RESOLUTION);

  Serial.print("[SERVO] Attach: ");
  Serial.println(ok ? "OK" : "FALHOU");

  if (!ok) {
    return;
  }

  anguloAtual = 90.0f;
  anguloAlvo = 90.0f;

  uint32_t duty = microsecondsToDuty(1500);

  Serial.print("[SERVO] Duty 1500us: ");
  Serial.println(duty);

  ledcWrite(SERVO_PIN, duty);
}
// Em 50 Hz, um período completo possui 20.000 us.
uint32_t ServoController::microsecondsToDuty(int microseconds) {
  const uint32_t periodoUs = 20000;
  const uint32_t maxDuty = (1UL << SERVO_RESOLUTION) - 1;

  return ((uint64_t)microseconds * maxDuty) / periodoUs;
}

// Define o novo ângulo desejado. O movimento físico é
// realizado gradualmente dentro de update().
void ServoController::setTarget(float angulo) {
  anguloAlvo = constrain(angulo, 0.0f, 180.0f);
}

void ServoController::update() {
    const float velocidade = 2.5f;

    if (anguloAtual < anguloAlvo) {
        anguloAtual = min(
            anguloAtual + velocidade,
            anguloAlvo
        );
    }
    else if (anguloAtual > anguloAlvo) {
        anguloAtual = max(
            anguloAtual - velocidade,
            anguloAlvo
        );
    }

    // Aplica ajuste fino para centralização mecânica.
    float anguloCorrigido =
        anguloAtual + SERVO_CENTER_OFFSET;

    // Garante que o valor continue dentro do limite.
    anguloCorrigido = constrain(
        anguloCorrigido,
        0.0f,
        180.0f
    );

    int pulseUs;

    if (SERVO_INVERTIDO) {

        pulseUs = map(
            (int)anguloCorrigido,
            0,
            180,
            SERVO_MAX_US,
            SERVO_MIN_US
        );

    } else {

        pulseUs = map(
            (int)anguloCorrigido,
            0,
            180,
            SERVO_MIN_US,
            SERVO_MAX_US
        );
    }

    ledcWrite(
        SERVO_PIN,
        microsecondsToDuty(pulseUs)
    );
}

float ServoController::getAngle() {
  return anguloAtual;
}
