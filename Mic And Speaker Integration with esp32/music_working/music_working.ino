#include <driver/i2s.h>

#define BCLK 26
#define LRC  25
#define DIN  22

int16_t mono[256];
int16_t stereo[512];

void setup()
{
    Serial.begin(921600);

    i2s_config_t cfg = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 16,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pin = {
        .bck_io_num = BCLK,
        .ws_io_num = LRC,
        .data_out_num = DIN,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin);

    Serial.println("Ready");
}

void loop()
{
    size_t got = Serial.readBytes(
        (char*)mono,
        sizeof(mono)
    );

    if (got == sizeof(mono))
    {
        for (int i = 0; i < 256; i++)
        {
            stereo[i * 2]     = mono[i];
            stereo[i * 2 + 1] = mono[i];
        }

        size_t written;

        i2s_write(
            I2S_NUM_0,
            stereo,
            sizeof(stereo),
            &written,
            portMAX_DELAY
        );
    }
}