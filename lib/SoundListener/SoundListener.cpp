#include "SoundListener.h"
#include <Arduino.h>
#include <math.h>
#include <driver/i2s.h>

void SoundListener::setup(){
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLING_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };

    const i2s_pin_config_t pins = {
        .bck_io_num = 36,
        .ws_io_num = 39,
        .data_out_num = -1,
        .data_in_num = 32
    };

    esp_err_t res = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (res != ESP_OK) {
        Serial.println("i2s_driver_install failed");
    }

    res = i2s_set_pin(I2S_NUM_0, &pins);
    if (res != ESP_OK) {
        Serial.println("i2s_set_pin failed");
    }
    // Ensure DMA buffer is cleared and give peripheral time to stabilize
    i2s_zero_dma_buffer(I2S_NUM_0);
    delay(200);
    Serial.println("I2S OK");
}

void SoundListener::update(){
    // ensure bytesRead starts at 0
    bytesRead = 0;

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

int SoundListener::getBufferSize(){
    return SOUND_BUFFER_SIZE;
}

int32_t* SoundListener::getBuffer(){
    return soundBuffer;
}

size_t SoundListener::getBytesRead(){
    return bytesRead;
}
