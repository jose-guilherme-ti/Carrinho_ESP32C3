#include "MobileController.h"
#include <WiFi.h>

// =====================================================
// PÁGINA WEB DO CONTROLE MOBILE
// =====================================================
// A página fica armazenada na memória do firmware para
// não depender de arquivos externos ou cartão SD.
// =====================================================
static const char MOBILE_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
    <title>Carrinho RC</title>
    <style>
      /* =====================================================
   INTERFACE MOBILE
   =====================================================
   Os dois joysticks ficam lado a lado e o conjunto é
   centralizado na tela. Cada controle possui uma área
   fixa (base) e um botão móvel (stick).
   ===================================================== */
      * {
        box-sizing: border-box;
        -webkit-tap-highlight-color: transparent
      }

      /* Fundo geral da página e cor padrão dos textos. */
      body {
        margin: 0;
        background: #111;
        color: #f5f5f5;
        font-family: Arial, sans-serif;
        text-align: center;
        touch-action: none;
        overscroll-behavior: none
      }

      .wrap {
        max-width: 720px;
        margin: auto;
        padding: 14px 14px 22px
      }

      /* Título principal com alto contraste. */
      h1 {
        font-size: 22px;
        margin: 6px;
        color: #ffffff
      }

      /* Status começa em uma cor neutra.
   A classe .connected será adicionada pelo JavaScript
   quando houver comunicação com o ESP32. */
      .status {
        font-size: 14px;
        font-weight: bold;
        color: #b8b8b8;
        margin: 8px
      }

      .status.connected {
        color: #42e85f
      }

      /* Área que mantém os dois joysticks centralizados. */
      .controls {
        display: flex;
        justify-content: center;
        align-items: center;
        gap: 34px;
        margin: 26px auto 20px;
        flex-wrap: wrap
      }

      .control {
        width: min(38vw, 240px);
        min-width: 145px
      }

      /* Textos dos controles com contraste maior. */
      .label {
        font-size: 14px;
        font-weight: bold;
        color: #f0f0f0;
        margin-bottom: 10px
      }

      .value {
        font-size: 18px;
        font-weight: bold;
        color: #ffffff;
        margin-top: 10px
      }

      /* Base circular do joystick. */
      .joystick {
        position: relative;
        width: 100%;
        aspect-ratio: 1;
        border-radius: 50%;
        background: radial-gradient(circle at 50% 45%, #555 0, #363636 40%, #202020 72%, #151515 100%);
        box-shadow: inset 0 0 18px #000, 0 8px 22px rgba(0, 0, 0, .35);
        touch-action: none;
        user-select: none
      }

      /* Área interna apenas para dar referência visual de centro. */
      .joystick:after {
        content: "";
        position: absolute;
        left: 50%;
        top: 50%;
        width: 52%;
        height: 52%;
        transform: translate(-50%, -50%);
        border-radius: 50%;
        border: 2px solid rgba(255, 255, 255, .10);
        pointer-events: none
      }

      /* Botão móvel que acompanha o dedo dentro da área permitida. */
      .stick {
        position: absolute;
        left: 50%;
        top: 50%;
        width: 38%;
        aspect-ratio: 1;
        transform: translate(-50%, -50%);
        border-radius: 50%;
        background: radial-gradient(circle at 38% 30%, #8d8d8d 0, #5b5b5b 28%, #2c2c2c 72%);
        box-shadow: 0 8px 16px rgba(0, 0, 0, .55), inset 0 0 12px rgba(255, 255, 255, .18);
        pointer-events: none
      }

      #steerStick {
        background: radial-gradient(circle at 38% 30%, #9b9b9b 0, #626262 28%, #303030 72%)
      }

      #steerStick.active {
        background: radial-gradient(circle at 38% 30%,
            #7dff9a 0,
            #20b84c 32%,
            #075e22 75%)
      }

      /* O acelerador começa com cor neutra. */
      #throttleStick {
        background: radial-gradient(circle at 38% 30%,
            #9b9b9b 0,
            #626262 28%,
            #303030 72%);
        transition: background .15s ease
      }

      /* Quando o acelerador estiver ativo, fica verde. */
      #throttleStick.active {
        background: radial-gradient(circle at 38% 30%,
            #7dff9a 0,
            #20b84c 32%,
            #075e22 75%)
      }

      .btn {
        border: 0;
        border-radius: 16px;
        padding: 18px 14px;
        font-size: 17px;
        font-weight: bold;
        color: white;
        background: #333;
        user-select: none;
        width: 100%;
        margin-top: 10px
      }

      #rev {
        /* Cinza neutro */
        background: #3a3a3a;
        border: 2px solid #666;
        /* Borda neutra */
        color: #ffffff;
        padding: 14px 18px;
        border-radius: 10px;
        font-size: 15px;
        font-weight: bold;
        min-width: 120px;
        transition: all .15s ease
          /* Texto branco */
      }

      #rev.active {
        background: #20b84c;
        border: 2px solid #1ea857;
        color: #ffffff;
      }

      /* Verde quando ativado */
      /* Texto branco para legibilidade */
      .stop {
        background: #8b2525
      }

      .hint {
        font-size: 12px;
        color: #aaa;
        margin-top: 12px;
        line-height: 1.45
      }

      @media(max-width:430px) {
        .controls {
          gap: 18px;
          margin-top: 18px
        }

        .control {
          width: 42vw;
          min-width: 0
        }

        .wrap {
          padding-left: 10px;
          padding-right: 10px
        }
      }
    </style>
  </head>
  <body>
    <div class="wrap">
      <h1>🚗 CARRINHO RC</h1>
      <div id="status" class="status">● CONECTADO - MODO CELULAR</div>
      <div class="controls">
        <div class="control">
          <div class="label">DIREÇÃO</div>
          <div id="steerPad" class="joystick">
            <div id="steerStick" class="stick"></div>
          </div>
          <div id="steerVal" class="value">CENTRO • 90°</div>
        </div>
        <div class="control">
          <div class="label">ACELERADOR / FREIO</div>
          <div id="throttlePad" class="joystick">
            <div id="throttleStick" class="stick"></div>
          </div>
          <div id="throttleVal" class="value">NEUTRO • 0%</div>
        </div>
      </div>
      <button id="rev" class="btn reverse">RÉ: DESLIGADA</button>
      <button id="stop" class="btn stop">🛑 PARAR</button>
      <div class="hint">Direção: mova para esquerda ou direita. Acelerador: mova para cima para acelerar e para baixo para frear. Ao soltar, os joysticks voltam automaticamente ao centro.</div>
    </div>
    <script>
      /* =====================================================
   ESTADO DOS CONTROLES
   ===================================================== */
      let s = 90,
        a = 0,
        f = 0,
        r = 0;
      const steerPad = document.querySelector('#steerPad'),
        throttlePad = document.querySelector('#throttlePad');
      const steerStick = document.querySelector('#steerStick'),
        throttleStick = document.querySelector('#throttleStick');
      const steerVal = document.querySelector('#steerVal'),
        throttleVal = document.querySelector('#throttleVal');
      const rev = document.querySelector('#rev'),
        stop = document.querySelector('#stop'),
        status = document.querySelector('#status');

      function ui() {
        /* Atualiza o texto da direção. */
        //steerVal.textContent =
        //(s === 90 ? 'CENTRO • ' : '') + s + '°';
        if (s > 90) {
          steerVal.textContent = 'DIREITA • ' + s + '°';
          steerStick.classList.add('active');
        } else if (s < 90) {
          steerVal.textContent = 'ESQUERDA • ' + s + '°';
          steerStick.classList.add('active');
        } else {
          steerVal.textContent = 'CENTRO • ' + s + '°';
          steerStick.classList.remove('active');
        }
        /* =====================================================
           ATUALIZA O TEXTO E A COR DO JOYSTICK DE ACELERAÇÃO
           =====================================================

           - Acelerando: joystick fica verde.
           - Freando: joystick continua com cor neutra.
           - Neutro: joystick fica com cor neutra.
           ===================================================== */
        if (a > 0) {
          throttleVal.textContent = 'ACELERANDO • ' + a + '%';
          // Adiciona a classe que deixa o joystick verde.
          throttleStick.classList.add('active');
        } else if (f > 0) {
          throttleVal.textContent = 'FREIO • ' + f + '%';
          // Remove o verde durante o freio.
          throttleStick.classList.remove('active');
        } else {
          throttleVal.textContent = 'NEUTRO • 0%';
          // Remove o verde quando volta ao centro.
          throttleStick.classList.remove('active');
        }
        /* Atualiza o botão de ré. */
        rev.textContent = 'RÉ:' + (r ? 'LIGADA' : 'DESLIGADA');
        rev.classList.toggle('active', r); // Adiciona/remove a classe 'active'
      }
      /* Move visualmente o botão do joystick usando porcentagem
         para manter o layout responsivo em qualquer celular. */
      function moveStick(stick, x, y) {
        stick.style.left = (50 + x) + '%';
        stick.style.top = (50 + y) + '%'
      }

      function resetSteer() {
        s = 90;
        moveStick(steerStick, 0, 0);
        ui();
        send()
      }

      function resetThrottle() {
        a = 0;
        f = 0;
        moveStick(throttleStick, 0, 0);
        ui();
        send()
      }
      /* =====================================================
         JOYSTICK DE DIREÇÃO
         =====================================================
         Usa apenas o eixo X. O botão pode se mover até 30% do
         raio visual e sempre retorna ao centro quando solto.
         ===================================================== */
      function setupSteering() {
        let active = false;

        function update(e) {
          if (!active) return;
          const rect = steerPad.getBoundingClientRect();
          let x = (e.clientX - (rect.left + rect.width / 2)) / (rect.width / 2);
          x = Math.max(-1, Math.min(1, x));
          moveStick(steerStick, x * 30, 0);
          s = Math.round(90 + x * 90);
          ui();
        }
        steerPad.addEventListener('pointerdown', e => {
          active = true;
          steerPad.setPointerCapture(e.pointerId);
          update(e)
        });
        steerPad.addEventListener('pointermove', update);
        steerPad.addEventListener('pointerup', () => {
          active = false;
          resetSteer()
        });
        steerPad.addEventListener('pointercancel', () => {
          active = false;
          resetSteer()
        });
      }
      /* =====================================================
         JOYSTICK DE ACELERAÇÃO
         =====================================================
         Usa apenas o eixo Y:
         - para cima: aumenta o acelerador;
         - para baixo: aumenta o freio.
         Isso mantém o protocolo atual do ESP32 e evita alterar
         o funcionamento já validado do ESC e do G29.
         ===================================================== */
      function setupThrottle() {
        let active = false;

        function update(e) {
          if (!active) return;
          const rect = throttlePad.getBoundingClientRect();
          let y = (e.clientY - (rect.top + rect.height / 2)) / (rect.height / 2);
          y = Math.max(-1, Math.min(1, y));
          moveStick(throttleStick, 0, y * 30);
          if (y < 0) {
            a = Math.round(-y * 100);
            f = 0
          } else {
            f = Math.round(y * 100);
            a = 0
          }
          ui();
        }
        throttlePad.addEventListener('pointerdown', e => {
          active = true;
          throttlePad.setPointerCapture(e.pointerId);
          update(e)
        });
        throttlePad.addEventListener('pointermove', update);
        throttlePad.addEventListener('pointerup', () => {
          active = false;
          resetThrottle()
        });
        throttlePad.addEventListener('pointercancel', () => {
          active = false;
          resetThrottle()
        });
      }
      /* Envia o estado atual periodicamente. A frequência mantém
         o MobileController ativo sem bloquear o loop do ESP32. */
      function send() {
        fetch(`/command?s=${s}&a=${a}&f=${f}&r=${r}`, {
          cache: 'no-store'
        }).then(() => {
          /* Comunicação funcionando:
             adiciona a classe verde. */
          status.textContent = '● CONECTADO - MODO CELULAR';
          status.classList.add('connected');
        }).catch(() => {
          /* Sem comunicação:
             remove o verde e mantém uma cor neutra. */
          status.textContent = '● SEM CONEXÃO';
          status.classList.remove('connected');
        });
      }
      setInterval(send, 80);
      rev.onclick = () => {
        r = r ? 0 : 1;
        ui();
        send()
      };
      stop.onclick = () => {
        s = 90;
        a = 0;
        f = 0;
        r = 0;
        moveStick(steerStick, 0, 0);
        moveStick(throttleStick, 0, 0);
        ui();
        fetch('/stop', {
          cache: 'no-store'
        }).catch(() => {})
      };
      window.addEventListener('pagehide', () => navigator.sendBeacon('/stop'));
      setupSteering();
      setupThrottle();
      ui();
      send();
    </script>
  </body>
</html>
)rawliteral";

MobileController::MobileController() : server(MOBILE_HTTP_PORT) {}

void MobileController::begin()
{
    server.on("/", HTTP_GET, [this]() { handleRoot(); });
    server.on("/command", HTTP_GET, [this]() { handleCommand(); });
    server.on("/stop", HTTP_POST, [this]() { handleStop(); });
    server.on("/status", HTTP_GET, [this]() { handleStatus(); });

    server.begin();

    Serial.print("[MOBILE] Controle web iniciado na porta ");
    Serial.println(MOBILE_HTTP_PORT);
    Serial.println("[MOBILE] Abra http://192.168.0.108 no celular");
}

void MobileController::update()
{
    // processa os clientes HTTP sem bloquear o restante
    // do controle do veículo.
    server.handleClient();
}

bool MobileController::isActive()
{
    if (!paginaAberta) return false;

    return (millis() - ultimoComando) < TEMPO_MOBILE_FAILSAFE;
}

int MobileController::getDirecao() { return direcao; }
int MobileController::getAcelerador() { return acelerador; }
int MobileController::getFreio() { return freio; }
bool MobileController::getRe() { return re; }

void MobileController::handleRoot()
{
    // Abrir a página seleciona imediatamente o modo
    // mobile, mas o ESC permanece neutro até receber um
    // comando válido do navegador.
    paginaAberta = true;
    ultimoComando = millis();
    acelerador = 0;
    freio = 0;
    re = false;

    server.send_P(200, "text/html; charset=utf-8", MOBILE_PAGE);
}

void MobileController::handleCommand()
{
    if (!server.hasArg("s") || !server.hasArg("a") ||
        !server.hasArg("f") || !server.hasArg("r")) {
        server.send(400, "text/plain", "Comando invalido");
        return;
    }

    direcao = constrain(server.arg("s").toInt(), 0, 180);
    acelerador = constrain(server.arg("a").toInt(), 0, 100);
    freio = constrain(server.arg("f").toInt(), 0, 100);
    re = server.arg("r").toInt() != 0;

    // Não permite acelerar e frear simultaneamente.
    if (freio > 0) acelerador = 0;

    paginaAberta = true;
    ultimoComando = millis();

    server.send(200, "text/plain", "OK");
}

void MobileController::handleStop()
{
    direcao = 90;
    acelerador = 0;
    freio = 0;
    re = false;
    paginaAberta = false;

    server.send(200, "text/plain", "STOP");
}

void MobileController::handleStatus()
{
    String resposta = isActive() ? "MOBILE" : "INACTIVE";
    server.send(200, "text/plain", resposta);
}
