#include "G29NetworkManager.h"


// =====================================================
// Conecta o ESP32-C3 ao roteador Wi-Fi.
// =====================================================
void G29NetworkManager::connectWiFi() {
    Serial.println();
    Serial.println("================================");
    Serial.println("Conectando ao Wi-Fi...");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}


// =====================================================
// Inicializa Wi-Fi e servidor UDP.
// =====================================================
void G29NetworkManager::begin() {
    connectWiFi();

    // Aguarda inicialmente alguns segundos pela conexão.
    unsigned long inicio = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - inicio < 10000
    ) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Wi-Fi conectado!");
        Serial.print("IP do ESP32-C3: ");
        Serial.println(WiFi.localIP());

        if (udp.begin(UDP_PORT)) {
            udpIniciado = true;

            Serial.print("UDP iniciado na porta: ");
            Serial.println(UDP_PORT);
        } else {
            Serial.println("ERRO: nao foi possivel iniciar UDP");
        }

    } else {
        Serial.println("ERRO: nao foi possivel conectar ao Wi-Fi");
        Serial.println(
            "O sistema continuara tentando reconectar."
        );
    }
}


// =====================================================
// Tenta reconectar caso o ESP32 perca a conexão.
// =====================================================
void G29NetworkManager::reconnectWiFi() {

    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    if (
        millis() - ultimaTentativaWiFi <
        INTERVALO_RECONEXAO
    ) {
        return;
    }

    ultimaTentativaWiFi = millis();

    Serial.println("Wi-Fi desconectado.");
    Serial.println("Tentando reconectar...");

    // Fecha o socket UDP anterior.
    if (udpIniciado) {
        udp.stop();
        udpIniciado = false;
    }

    WiFi.disconnect();

    delay(100);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}


// =====================================================
// Processa Wi-Fi e novos pacotes UDP.
// =====================================================
void G29NetworkManager::update() {

    // Verifica continuamente a conexão Wi-Fi.
    if (WiFi.status() != WL_CONNECTED) {
        reconnectWiFi();
        return;
    }

    // Se o Wi-Fi reconectou e o UDP ainda não foi
    // iniciado, inicia novamente o socket.
    if (!udpIniciado) {

        Serial.println("Wi-Fi conectado novamente!");
        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());

        if (udp.begin(UDP_PORT)) {
            udpIniciado = true;

            Serial.print(
                "UDP reiniciado na porta: "
            );
            Serial.println(UDP_PORT);

        } else {
            Serial.println(
                "ERRO ao reiniciar UDP"
            );
            return;
        }
    }


    // =================================================
    // PROCURA NOVOS PACOTES UDP
    // =================================================

    int packetSize = udp.parsePacket();

    if (!packetSize) {
        return;
    }

    int len = udp.read(
        packetBuffer,
        sizeof(packetBuffer) - 1
    );

    if (len <= 0) {
        return;
    }

    // Finaliza a string recebida.
    packetBuffer[len] = '\0';


    // =================================================
    // DECODIFICA O PACOTE
    // =================================================

    int s;
    int a;
    int f;

    int resultado = sscanf(
        packetBuffer,
        "S:%d,A:%d,F:%d",
        &s,
        &a,
        &f
    );


    // Ignora pacotes inválidos.
    if (resultado != 3) {

        Serial.print("Pacote invalido: ");
        Serial.println(packetBuffer);

        return;
    }


    // =================================================
    // ATUALIZA OS COMANDOS
    // =================================================

    direcao = constrain(s, 0, 180);

    acelerador = constrain(
        a,
        0,
        100
    );

    freio = constrain(
        f,
        0,
        100
    );


    // Registra o momento do último pacote válido.
    ultimoPacote = millis();
}


// =====================================================
// Verifica se a comunicação com o Python está ativa.
// =====================================================
bool G29NetworkManager::isConnected() {

    // Sem Wi-Fi não existe comunicação.
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    // Sem UDP também não existe comunicação.
    if (!udpIniciado) {
        return false;
    }

    // Verifica o timeout do último comando.
    return (
        millis() - ultimoPacote
    ) < TEMPO_FAILSAFE;
}


// =====================================================
// Retorna a direção.
// =====================================================
int G29NetworkManager::getDirecao() {
    return direcao;
}


// =====================================================
// Retorna o acelerador.
// =====================================================
int G29NetworkManager::getAcelerador() {
    return acelerador;
}


// =====================================================
// Retorna o freio.
// =====================================================
int G29NetworkManager::getFreio() {
    return freio;
}


// =====================================================
// Retorna o estado da conexão Wi-Fi.
// =====================================================
bool G29NetworkManager::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}