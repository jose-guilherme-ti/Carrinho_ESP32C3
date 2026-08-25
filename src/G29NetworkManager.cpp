#include "G29NetworkManager.h"
#include "Config.h"

// Inicializa a rede Wi-Fi criada pelo ESP32 e o socket UDP.
void G29NetworkManager::begin() {
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
    delay(300);

    Serial.println("Wi-Fi iniciado");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    if (udp.begin(UDP_PORT)) {
        Serial.print("UDP iniciado na porta: ");
        Serial.println(UDP_PORT);
    } else {
        Serial.println("ERRO: nao foi possivel iniciar UDP");
    }
}

// Procura novos pacotes e atualiza os comandos recebidos.
void G29NetworkManager::update() {
    int packetSize = udp.parsePacket();

    if (!packetSize) {
        return;
    }

    int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);

    if (len <= 0) {
        return;
    }

    packetBuffer[len] = '\0';

    int s, a, f;

    int resultado = sscanf(
        packetBuffer,
        "S:%d,A:%d,F:%d",
        &s, &a, &f
    );

    // Ignora pacotes com formato diferente do protocolo.
    if (resultado != 3) {
        return;
    }

    direcao = constrain(s, 0, 180);
    acelerador = constrain(a, 0, 100);
    freio = constrain(f, 0, 100);

    // Registra o momento do último comando válido.
    ultimoPacote = millis();
}

// Verifica se a comunicação com o computador continua ativa.
bool G29NetworkManager::isConnected() {
    return (millis() - ultimoPacote) < TEMPO_FAILSAFE;
}

int G29NetworkManager::getDirecao() {
    return direcao;
}

int G29NetworkManager::getAcelerador() {
    return acelerador;
}

int G29NetworkManager::getFreio() {
    return freio;
}
