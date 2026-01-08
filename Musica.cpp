#include "Musica.h"

NotaGravada musicaDefault[] = {
 {0, 2000, 300}, {0, 2350, 300}, {2, 2700, 1000}, {0, 3750, 300},{5, 4100, 1000}, {4, 5200, 1000},
  {0, 6250, 300}, {0, 6600, 300}, {2, 6950, 1000}, {0, 8000, 300},{7, 8350, 1000}, {5, 9400, 800},{5, 10250, 800},
  {9, 11100, 300}, {9, 11450, 300}, {11, 11800, 1000}, {9, 12850, 300},{5, 13200, 800}, {4, 14050, 800},{2, 14900, 800},
  {10, 15800, 300}, {10, 16150, 300}, {9, 16500, 800}, {5, 17350, 800},{7, 18100, 800}, {5, 18950, 600},{5, 19600, 600}
};
int totalDefault = sizeof(musicaDefault) / sizeof(NotaGravada);

NotaGravada musicaFacil[] = {
  {5, 2000, 300}, {9, 2400, 300}, {5, 2800, 300}, {4, 3200, 800}, {5, 4200, 300 }, {9, 4600, 300},
  {4, 5000, 300}, {2, 5400, 800}, {2, 6400, 300}, {5, 6800, 300},{2, 7200, 300}, {0, 7600, 800},{0, 8600, 300},
  {9, 9000, 500}, {7, 9600, 600}, {0, 10300, 300}, {9, 10700, 500},{7, 11300, 500}, {5, 11900, 500},{2, 12500, 700}
};
int totalFacil = sizeof(musicaFacil) / sizeof(NotaGravada);

NotaGravada musicaDificil[] = {
  {0, 2000, 300}, {2, 2350, 400}, {2, 2800, 400}, {2, 3250, 300},{4, 3600, 300}, {5, 3950, 400},{5, 4400, 400},
  {5, 4850, 300}, {7, 5200, 300}, {4, 5550, 400},{4, 6000, 400}, {2, 6450, 300},{0, 6800, 300},{0, 7130, 300}, {2, 7460, 500},
  {0, 8000, 300}, {2, 8350, 400}, {2, 8800, 400}, {2, 9250, 300},{4, 9600, 300}, {5, 9950, 400},{5, 10400, 400}, 
  {5, 10850, 300}, {7, 11200, 300}, {4, 11550, 400},{4, 12000, 400}, {2, 12450, 300},{0, 12800, 300}, {2, 13050, 600},
  {0, 13700, 300}, {2, 14050, 400}, {2, 14500, 400}, {2, 14950, 300}, {5, 15300, 300}, {7, 15650, 400}, {7, 16100, 400}, {7, 16550, 300},
  {9, 16800, 300}, {10, 17150, 400}, {10, 17600, 400}, {9, 18050, 300}, {7, 18400, 300}, {9, 18750, 300}, {2, 19100, 600}, {2, 19750, 300},
  {4, 20100, 300}, {5, 20450, 400}, {5, 20900, 400}, {7, 21350, 400}, {9, 21800, 300}, {2, 22150, 500}, {2, 22700, 300}, {5, 23050, 300},
  {4, 23400, 400}, {4, 23850, 400}, {5, 24300, 300}, {2, 24650, 300}, {4, 25000, 700}
};
int totalDificil = sizeof(musicaDificil) / sizeof(NotaGravada);

static int indiceNota = 0;
static int acertos = 0;

void iniciarReproducao(bool comSom) {

  reproduzindo = true;
  tocandoComSom = comSom;
  visualizando = !comSom;

  indiceNota = 0;
  acertos = 0;

  inicioReproducao = millis();

  for (int i = 0; i < NUM_BOTOES; i++) {
    ledsAtivos[i] = false;
  }

  Serial.println("Reprodução iniciada");
}

void processarMusica() {

  if (!reproduzindo || musicaAtual == nullptr) return;

  unsigned long agora = millis();
  unsigned long tempo = agora - inicioReproducao;

  if (indiceNota < totalMusicaAtual) {

    NotaGravada& nota = musicaAtual[indiceNota];

    if (tempo >= nota.inicio && tempo <= nota.inicio + nota.duracao) {
      ledsAtivos[nota.indiceBotao] = true;

      if (lastPressedIndex == nota.indiceBotao) {
        acertos++;
        indiceNota++;
        ledsAtivos[nota.indiceBotao] = false;
      }
    }
    else if (tempo > nota.inicio + nota.duracao) {
      ledsAtivos[nota.indiceBotao] = false;
      indiceNota++;
    }
  }
  else {
    finalizarReproducao();
  }
}

void finalizarReproducao() {

  reproduzindo = false;

  StaticJsonDocument<128> doc;
  doc["total"] = totalMusicaAtual;
  doc["acertos"] = acertos;

  char buffer[128];
  serializeJson(doc, buffer);
  webSocket.broadcastTXT(buffer);

  Serial.println("Reprodução finalizada");
}
