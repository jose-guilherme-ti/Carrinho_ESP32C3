#include "G29NetworkManager.h"

// =====================================================
// Conecta o ESP32-C3 ao roteador Wi-Fi.
// =====================================================
// =====================================================
// Conecta o ESP32-C3 ao roteador Wi-Fi.
// Configura um IP estático antes da conexão.
// =====================================================
void G29NetworkManager::connectWiFi()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("Configurando rede...");

    WiFi.mode(WIFI_STA);

    WiFi.disconnect();
    delay(100);

    if (!configureStaticIP())
    {
        Serial.println(
            "ERRO: Falha ao configurar IP estatico!");
    }

    Serial.println("Conectando ao Wi-Fi...");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);

    Serial.print("IP fixo configurado: ");
    Serial.println(WIFI_STATIC_IP);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD);
}

// =====================================================
// Inicializa Wi-Fi e servidor UDP.
// =====================================================
void G29NetworkManager::begin()
{
    connectWiFi();

    // Aguarda inicialmente alguns segundos pela conexão.
    unsigned long inicio = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - inicio < 10000)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Wi-Fi conectado!");
        Serial.print("IP do ESP32-C3: ");
        Serial.println(WiFi.localIP());

        if (udp.begin(UDP_PORT))
        {
            udpIniciado = true;

            Serial.print("UDP iniciado na porta: ");
            Serial.println(UDP_PORT);
        }
        else
        {
            Serial.println("ERRO: nao foi possivel iniciar UDP");
        }
    }
    else
    {
        Serial.println("ERRO: nao foi possivel conectar ao Wi-Fi");
        Serial.println(
            "O sistema continuara tentando reconectar.");
    }
}

// =====================================================
// Tenta reconectar caso o ESP32 perca a conexão.
// =====================================================
void G29NetworkManager::reconnectWiFi()
{

    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    if (
        millis() - ultimaTentativaWiFi <
        INTERVALO_RECONEXAO)
    {
        return;
    }

    ultimaTentativaWiFi = millis();

    Serial.println("Wi-Fi desconectado.");
    Serial.println("Tentando reconectar...");

    if (udpIniciado)
    {
        udp.stop();
        udpIniciado = false;
    }

    WiFi.disconnect();
    delay(100);

    if (!configureStaticIP())
    {
        Serial.println(
            "ERRO: Falha ao configurar IP estatico!");
    }

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD);
}

// =====================================================
// Configura os parâmetros de IP estático.
// =====================================================
bool G29NetworkManager::configureStaticIP()
{

    return WiFi.config(
        WIFI_STATIC_IP,
        WIFI_GATEWAY,
        WIFI_SUBNET,
        WIFI_DNS1,
        WIFI_DNS2);
}

// =====================================================
// Processa Wi-Fi e novos pacotes UDP.
// =====================================================
void G29NetworkManager::update()
{

    // Verifica continuamente a conexão Wi-Fi.
    if (WiFi.status() != WL_CONNECTED)
    {
        reconnectWiFi();
        return;
    }

    // Se o Wi-Fi reconectou e o UDP ainda não foi
    // iniciado, inicia novamente o socket.
    if (!udpIniciado)
    {

        Serial.println("Wi-Fi conectado novamente!");
        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());

        if (udp.begin(UDP_PORT))
        {
            udpIniciado = true;

            Serial.print(
                "UDP reiniciado na porta: ");
            Serial.println(UDP_PORT);
        }
        else
        {
            Serial.println(
                "ERRO ao reiniciar UDP");
            return;
        }
    }

    // =================================================
    // PROCURA NOVOS PACOTES UDP
    // =================================================

    int packetSize = udp.parsePacket();

    if (packetSize > 0)
    {
        // Limita o tamanho para sobrar espaço para '\0'.
        int len = udp.read(
            packetBuffer,
            sizeof(packetBuffer) - 1);

         if (len <= 0)
        {
            return;
        }

        // MUITO IMPORTANTE:
        // transforma o pacote UDP em uma string C válida.
        packetBuffer[len] = '\0';

        int s = 90;
        int a = 0;
        int f = 0;
        int r = 0;

        int resultado = sscanf(
            packetBuffer,
            "S:%d,A:%d,F:%d,R:%d",
            &s,
            &a,
            &f,
            &r);

        // DEBUG TEMPORÁRIO
        Serial.print("[UDP] Recebido: ");
        Serial.print(packetBuffer);

        Serial.print(" | Campos: ");
        Serial.println(resultado);

        // O pacote precisa conter os 4 campos.
        if (resultado != 4)
        {
            Serial.print("Pacote invalido: ");
            Serial.println(packetBuffer);

            return;
        }

        // ==========================================
        // PACOTE VÁLIDO
        // ==========================================

        direcao = constrain(
            s,
            0,
            180);

        acelerador = constrain(
            a,
            0,
            100);

        freio = constrain(
            f,
            0,
            100);

        re = constrain(
            r,
            0,
            1);

        // Atualiza o failsafe SOMENTE com pacote válido.
        ultimoPacote = millis();
        
    }

    /* // Finaliza a string recebida.
    packetBuffer[len] = '\0'; */

    // =================================================
    // DECODIFICA O PACOTE
    // =================================================

    int s;
    int a;
    int f;
    int r;

    int resultado = sscanf(
        packetBuffer,
        "S:%d,A:%d,F:%d,R:%d",
        &s,
        &a,
        &f,
        &r);

    // Ignora pacotes inválidos.
    if (resultado != 3)
    {

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
        100);

    freio = constrain(
        f,
        0,
        100);

    re = constrain(r, 0, 1);

    // Registra o momento do último pacote válido.
    ultimoPacote = millis();
}

// =====================================================
// Verifica se a comunicação com o Python está ativa.
// =====================================================
bool G29NetworkManager::isConnected()
{

    // Sem Wi-Fi não existe comunicação.
    if (WiFi.status() != WL_CONNECTED)
    {
        return false;
    }

    // Sem UDP também não existe comunicação.
    if (!udpIniciado)
    {
        return false;
    }

    // Verifica o timeout do último comando.
    return (
               millis() - ultimoPacote) < TEMPO_FAILSAFE;
}

// =====================================================
// Retorna a direção.
// =====================================================
int G29NetworkManager::getDirecao()
{
    return direcao;
}

// =====================================================
// Retorna o acelerador.
// =====================================================
int G29NetworkManager::getAcelerador()
{
    return acelerador;
}

// =====================================================
// Retorna o freio.
// =====================================================
int G29NetworkManager::getFreio()
{
    return freio;
}

// =====================================================
// Retorna o estado da conexão Wi-Fi.
// =====================================================
bool G29NetworkManager::isWiFiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

// =====================================================
// Retorna o ré.
// =====================================================
bool G29NetworkManager::getRe()
{
    return re == 1;
}