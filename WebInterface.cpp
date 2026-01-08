#include "WebInterface.h"
#include <ArduinoJson.h>

WebServer httpServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="utf-8">
<title>Desempenho Musical</title>
<style>
  body { background:#111; color:#fff; font-family: Arial, Helvetica; text-align:center; padding:40px; }
  h1 { font-size:28px; }
  #pct { font-size:48px; margin:20px 0; }
  .bar { width:70%; height:30px; background:#333; border-radius:8px; margin: 0 auto; overflow:hidden; }
  .progress { height:100%; width:0%; background:limegreen; transition: width 0.6s; }
  #details { margin-top:8px; color:#ddd; }
  #status { font-size:13px; color:#aaa; margin-top:10px; }
  button { background:#444; color:white; border:0; padding:8px 18px; margin:6px; border-radius:8px; cursor:pointer; font-size:16px; }
  button:hover { background:#666; }
</style>
</head>
<body>
  <h1>🎵 Desempenho - Modo Visual</h1>
  <div id="pct">Aguardando reprodução...</div>
  <div class="bar"><div id="progress" class="progress"></div></div>
  <div id="details"></div>
  <div id="status">Conectando ao WebSocket...</div>

  <div style="margin-top:18px;">
    <button onclick="selecionarMusica('Fácil')"> 🟩Fácil​</button>
    <button onclick="selecionarMusica('Média')"> 🟨Média​​</button>
    <button onclick="selecionarMusica('Difícil')">​​🟥​ Difícil</button>
    <button onclick="selecionarMusica('Default')">↺ Default</button>
  </div>

<script>
  const ws = new WebSocket('ws://' + location.hostname + ':81');
  const statusEl = document.getElementById('status');
  const pctEl = document.getElementById('pct');
  const progressEl = document.getElementById('progress');
  const detailsEl = document.getElementById('details');

  ws.onopen = () => { statusEl.textContent = 'WebSocket conectado'; };
  ws.onclose = () => { statusEl.textContent = 'WebSocket desconectado'; };
  ws.onerror = (e) => { statusEl.textContent = 'Erro no WebSocket'; console.log(e); };

  ws.onmessage = (evt) => {
    try {
      const data = JSON.parse(evt.data);
      if (data.total !== undefined && data.acertos !== undefined) {
        const total = data.total || 0;
        const acertos = data.acertos || 0;
        const pct = total > 0 ? ((acertos / total) * 100) : 0;
        pctEl.textContent = pct.toFixed(1) + '% de acerto';
        progressEl.style.width = pct + '%';
        detailsEl.textContent = acertos + ' de ' + total + ' notas acertadas';
      }
      else if (data.status) {
        statusEl.textContent = 'Servidor: ' + data.status +
          (data.musica ? ' (' + data.musica + ')' : '');
      }
    } catch (err) {
      console.log('mensagem ws:', evt.data);
    }
  };

  function selecionarMusica(nome) {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(JSON.stringify({ comando: "selecionarMusica", nome: nome }));
    } else {
      alert('WebSocket não conectado ainda');
    }
  }
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  httpServer.send_P(200, "text/html", index_html);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  if (type == WStype_CONNECTED) {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.print("WS cliente conectado: ");
    Serial.println(ip.toString());
  }
  else if (type == WStype_DISCONNECTED) {
    Serial.println("WS cliente desconectado");
  }
  else if (type == WStype_TEXT) {

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
      Serial.print("WS texto não-JSON: ");
      Serial.println((const char*)payload);
      return;
    }

    const char* comando = nullptr;
    if (doc.containsKey("comando")) comando = doc["comando"];
    else if (doc.containsKey("type")) comando = doc["type"];

    if (!comando) return;

    if (strcmp(comando, "selecionarMusica") == 0 ||
        strcmp(comando, "selectSong") == 0 ||
        strcmp(comando, "playSong") == 0) {

      const char* nome = nullptr;
      if (doc.containsKey("nome")) nome = doc["nome"];
      else if (doc.containsKey("song")) nome = doc["song"];
      if (!nome) nome = "default";

      Serial.print("Selecionar música: ");
      Serial.println(nome);

      if (strcmp(nome, "Fácil") == 0) {
        extern NotaGravada musicaFacil[];
        extern int totalFacil;
        musicaAtual = musicaFacil;
        totalMusicaAtual = totalFacil;
      }
      else if (strcmp(nome, "Média") == 0) {
        extern NotaGravada musicaDefault[];
        extern int totalDefault;
        musicaAtual = musicaDefault;
        totalMusicaAtual = totalDefault;
      }
      else if (strcmp(nome, "Difícil") == 0) {
        extern NotaGravada musicaDificil[];
        extern int totalDificil;
        musicaAtual = musicaDificil;
        totalMusicaAtual = totalDificil;
      }
      else {
        extern NotaGravada musicaDefault[];
        extern int totalDefault;
        musicaAtual = musicaDefault;
        totalMusicaAtual = totalDefault;
      }

      char resposta[96];
      snprintf(resposta, sizeof(resposta),
               "{\"status\":\"ok\",\"musica\":\"%s\"}", nome);
      webSocket.sendTXT(num, resposta);
    }
  }
}

void setupWebInterface() {

  const char* ssid = "ESP32-Music";
  const char* pass = "12345678";

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  IPAddress ip = WiFi.softAPIP();
  Serial.println(ip);

  httpServer.on("/", handleRoot);
  httpServer.begin();
  Serial.println("HTTP server iniciado em /");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server iniciado na porta 81");
}

void loopWebInterface() {
  webSocket.loop();
  httpServer.handleClient();
}
