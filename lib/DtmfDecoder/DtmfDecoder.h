#ifndef DTMF_DECODER_H
#define DTMF_DECODER_H

#include <Arduino.h>

#define SAMPLING_RATE 8000
#define SEQ_SIZE 8

// --- DTMF Configuration ---
static constexpr float DEFAULT_DTMF_THRESHOLD = 100000000.0f;

class DTMFDecoder {
private:
    // DTMF Frequencies
    const float ROW_FREQUENCIES[4] = {697.0, 770.0, 852.0, 941.0};
    const float COL_FREQUENCIES[4] = {1209.0, 1336.0, 1477.0, 1633.0};

    // DTMF Key Map
    const char DTMF_KEYS[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    // --- Sequence Variables ---
    char sequence[SEQ_SIZE];
    char lastDecodedKey;
    unsigned long lastCharacterTime; // Timestamp of last detected character
    const unsigned long SEQUENCE_TIMEOUT_MS = 3000; // Clear sequence after 3 seconds of inactivity

    float dtmfThreshold;

    /**
     * Calculates the Magnitude of a specific frequency using the Goertzel Algorithm.
     */
    float Goertzel(float freq0, int32_t* buff, int buffSize);

    // Helper to push new key into history (Shift Left)
    void addToSequence(char key);

    void Debug(int32_t* buff, int buffSize);

public:
    DTMFDecoder();

    /**
     * Scans the buffer for DTMF tones.
     */
    char detectDTMF(int32_t* buff, int buffSize);

    /**
     * Updates the DTMF magnitude threshold.
     */
    void setThreshold(float threshold);

    /**
     * Returns the current DTMF threshold.
     */
    float getThreshold() const;

    /**
     * Returns the latest N keys as a string (e.g., "12345")
     */
    String getSequence();

    /**
     * Clears the sequence history.
     */
    void clearSequence();
};

#endif