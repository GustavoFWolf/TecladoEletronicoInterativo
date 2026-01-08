#include "Audio.h"

float volumeCompensado(float freq) {
  return MASTER_AMPLITUDE / sqrt(freq);
}

void setupI2S() {

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK,
    .ws_io_num = I2S_LRC,
    .data_out_num = I2S_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);

  Serial.println("I2S configurado");
}

void taskSomFunc(void* param) {

  const float twoPi = 2.0 * PI;
  float fase = 0.0;

  int16_t sample = 0;
  size_t bytesWritten;

  while (true) {

    if (tocandoNota && currentFreq > 0) {

      float incremento = twoPi * currentFreq / SAMPLE_RATE;
      fase += incremento;
      if (fase > twoPi) fase -= twoPi;

      float amp = volumeCompensado(currentFreq);
      sample = (int16_t)(sin(fase) * amp);

    } else {
      sample = 0;
    }

    i2s_write(I2S_NUM_0, &sample, sizeof(sample), &bytesWritten, portMAX_DELAY);
  }
}
