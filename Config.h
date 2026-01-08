#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <driver/i2s.h>

#define NUM_BOTOES 12
#define PI 3.14159265f
#define SAMPLE_RATE 44100
#define MASTER_AMPLITUDE 20000.0f
#define MAX_NOTAS 400


struct NoteButton {
  int pin;
  const char* nota;
  float freq;
  bool estadoAnterior;
};

struct NotaGravada {
  int indiceBotao;
  unsigned long inicio;
  unsigned long duracao;
};

extern NoteButton botoes[NUM_BOTOES];

extern const uint8_t LED_BITS[NUM_BOTOES];

#define I2S_DOUT 27
#define I2S_BCLK 26
#define I2S_LRC  25

#define DATA_PIN  14
#define CLOCK_PIN 18
#define LATCH_PIN 5

#define ALAVANCA_GRAVAR   32
#define ALAVANCA_VISUAL   35
#define ALAVANCA_PLAY_SOM 34


extern NotaGravada memoria[MAX_NOTAS];
extern int totalNotas;
extern bool gravando;
extern unsigned long inicioGravacao;

extern bool reproduzindo;
extern bool visualizando;
extern bool tocandoComSom;
extern unsigned long inicioReproducao;

extern NotaGravada* musicaAtual;
extern int totalMusicaAtual;

extern bool ledsAtivos[NUM_BOTOES];
extern uint16_t leds;

extern volatile int lastPressedIndex;
extern volatile float currentFreq;
extern volatile bool tocandoNota;

extern WebServer httpServer;
extern WebSocketsServer webSocket;

#endif
