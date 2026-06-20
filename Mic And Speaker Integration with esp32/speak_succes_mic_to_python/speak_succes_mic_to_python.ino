#include <driver/i2s.h>

#define I2S_WS   25
#define I2S_SD   33
#define I2S_SCK  26

#define RATE 16000
#define BUF 256

void setup() {

  Serial.begin(921600);

  i2s_config_t cfg = {
      .mode=(i2s_mode_t)(I2S_MODE_MASTER|I2S_MODE_RX),
      .sample_rate=RATE,
      .bits_per_sample=I2S_BITS_PER_SAMPLE_32BIT,

      // GO BACK TO LEFT (your old code worked)
      .channel_format=I2S_CHANNEL_FMT_ONLY_LEFT,

      // KEEP original format
      .communication_format=I2S_COMM_FORMAT_STAND_I2S,

      .intr_alloc_flags=0,
      .dma_buf_count=8,
      .dma_buf_len=64
  };

  i2s_pin_config_t pin={
      .bck_io_num=I2S_SCK,
      .ws_io_num=I2S_WS,
      .data_out_num=-1,
      .data_in_num=I2S_SD
  };

  i2s_driver_install(I2S_NUM_0,&cfg,0,NULL);
  i2s_set_pin(I2S_NUM_0,&pin);
}

void loop() {

  int32_t sample;
  size_t bytes;

  i2s_read(
      I2S_NUM_0,
      &sample,
      4,
      &bytes,
      portMAX_DELAY
  );

  // SAME SHIFT AS YOUR WORKING TEST
  sample >>= 18;

  int16_t out = sample;

  Serial.write(
      (uint8_t*)&out,
      2
  );
}