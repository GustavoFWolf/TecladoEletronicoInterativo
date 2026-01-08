#include "Config.h"
#include "Audio.h"
#include "Leds.h"
#include "WebInterface.h"
#include "Musica.h"

NoteButton botoes[NUM_BOTOES] = {
  {23, "Do", 261.63f, HIGH},
  {22, "Db", 277.18f, HIGH},
  {3,  "Re", 293.66f, HIGH},
  {21, "Rb", 311.13f, HIGH},
  {19, "Mi", 329.63f, HIGH},
  {17, "Fa", 349.23f, HIGH},
  {15, "Fb", 369.99f, HIGH},
  {0,  "Sol", 392.00f, HIGH},
  {4,  "SLb", 415.30f, HIGH},
  {16, "La", 440.00f, HIGH},
  {13, "Lb", 466.16f, HIGH},
  {33, "Si", 493.88f, HIGH}
};

NotaGravada memoria[MAX_NOTAS];
int totalNotas = 0;
bool gravando = false;
unsigned long inicioGravacao = 0;

bool reproduzindo = false;
bool visualizando = false;
bool tocandoComSom = true;
unsigned long inicioReproducao = 0;

NotaGravada* musicaAtual = musicaDefault;
int totalMusicaAtual = totalDefault;

bool ledsAtivos[NUM_BOTOES] = {false};
uint16_t leds = 0;

volatile int lastPressedIndex = -1;
volatile float currentFreq = 0;
volatile bool tocandoNota = false;

void setup() {

  Serial.begin(115200);

  for (int i = 0; i < NUM_BOTOES; i++) {
    pinMode(botoes[i].pin, INPUT_PULLUP);
  }

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  pinMode(ALAVANCA_GRAVAR, INPUT_PULLUP);
  pinMode(ALAVANCA_VISUAL, INPUT_PULLUP);
  pinMode(ALAVANCA_PLAY_SOM, INPUT_PULLUP);

  setupI2S();

  xTaskCreatePinnedToCore(
    taskSomFunc,
    "AudioTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );

  setupWebInterface();

  Serial.println("Sistema iniciado");
}

void loop() {

  for (int i = 0; i < NUM_BOTOES; i++) {

    bool pressionado = digitalRead(botoes[i].pin) == LOW;

    if (pressionado && !botoes[i].estadoAnterior) {

      lastPressedIndex = i;
      currentFreq = botoes[i].freq;
      tocandoNota = true;

      if (gravando && totalNotas < MAX_NOTAS) {
        memoria[totalNotas++] = {
          i,
          millis() - inicioGravacao,
          0
        };
      }
    }

    if (!pressionado && botoes[i].estadoAnterior) {

      tocandoNota = false;

      if (gravando && totalNotas > 0) {
        memoria[totalNotas - 1].duracao =
          millis() - inicioGravacao - memoria[totalNotas - 1].inicio;
      }
    }

    botoes[i].estadoAnterior = pressionado;
  }

  if (digitalRead(ALAVANCA_GRAVAR) == LOW && !gravando) {
    gravando = true;
    totalNotas = 0;
    inicioGravacao = millis();
    Serial.println("Gravação iniciada");
  }
  else if (digitalRead(ALAVANCA_GRAVAR) == HIGH && gravando) {
    gravando = false;
    Serial.println("Gravação finalizada");
  }

  if (!reproduzindo) {

    if (digitalRead(ALAVANCA_VISUAL) == LOW) {
      iniciarReproducao(false);
    }
    else if (digitalRead(ALAVANCA_PLAY_SOM) == LOW) {
      iniciarReproducao(true);
    }
  }

  processarMusica();

  atualizarLeds();

  loopWebInterface();
}
