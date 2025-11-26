#ifndef SOUND_LISTENER_H
#define SOUND_LISTENER_H

#include <Arduino.h>
#include "driver/i2s.h"

#define SOUND_BUFFER_SIZE 256

class SoundListener {
private:
    int32_t soundBuffer[SOUND_BUFFER_SIZE];

public:
    void setup(){
        const i2s_config_t i2s_config = {
            .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = SAMPLING_RATE,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = 0,
            .dma_buf_count = 8,
            .dma_buf_len = 256,
            .use_apll = false,
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
        };

        const i2s_pin_config_t pins = {
            .bck_io_num = 33,
            .ws_io_num = 25,
            .data_out_num = -1,
            .data_in_num = 32
        };

        i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
        i2s_set_pin(I2S_NUM_0, &pins);
    }

    void execute(){
        size_t bytesRead = 0;

        esp_err_t err = i2s_read(
            I2S_NUM_0,
            soundBuffer,
            sizeof(soundBuffer),
            &bytesRead,
            portMAX_DELAY
        );

        if (err != ESP_OK) {
            Serial.println("I2S read error!");
            return;
        }
    }

    int getBufferSize(){
        return SOUND_BUFFER_SIZE;
    }

    int32_t* getBuffer(){
        return soundBuffer;
    }
};

#endif