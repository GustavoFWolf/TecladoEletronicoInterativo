#include "Leds.h"

const uint8_t LED_BITS[NUM_BOTOES] = {
  0, 1, 2, 3, 4, 5,
  6, 7, 8, 9, 10, 11
};

void enviaDados(uint16_t valor) {

  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, highByte(valor));
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, lowByte(valor));

  digitalWrite(LATCH_PIN, HIGH);
}

void atualizarLeds() {

  leds = 0;

  for (int i = 0; i < NUM_BOTOES; i++) {
    if (ledsAtivos[i]) {
      leds |= (1 << LED_BITS[i]);
    }
  }
  enviaDados(leds);
}
