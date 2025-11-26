#ifndef DTMF_DECODER_H
#define DTMF_DECODER_H

#include <Arduino.h>

#define SAMPLING_RATE 8000
#define SEQ_SIZE 5

// --- DTMF Configuration ---
#define DTMF_THRESHOLD 1000000000.0 

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

    /**
     * Calculates the Magnitude of a specific frequency using the Goertzel Algorithm.
     */
    float Goertzel(float freq0, int32_t* buff, int buffSize) {
        float omega = (2.0 * PI * freq0) / SAMPLING_RATE;
        float coeff = 2.0 * cos(omega);
        
        float Q1 = 0;
        float Q2 = 0;
        
        for (int i = 0; i < buffSize; i++) {
            float sample = (float)buff[i]; 
            float Q0 = coeff * Q1 - Q2 + sample;
            Q2 = Q1;
            Q1 = Q0;
        }
        
        float magnitudeSquared = (Q1 * Q1) + (Q2 * Q2) - (Q1 * Q2 * coeff);
        if (magnitudeSquared < 0) magnitudeSquared = 0;
        
        return sqrt(magnitudeSquared);
    }

    // Helper to push new key into history (Shift Left)
    void addToSequence(char key) {
        for (int i = 0; i < SEQ_SIZE - 1; i++) {
            sequence[i] = sequence[i + 1];
        }
        sequence[SEQ_SIZE - 1] = key;
    }

public:
    DTMFDecoder() {
        lastDecodedKey = 0;
        memset(sequence, 0, SEQ_SIZE);
    }

    /**
     * Scans the buffer for DTMF tones.
     * Automatically updates the internal sequence if a NEW key press is detected.
     */
    char detectDTMF(int32_t* buff, int buffSize) {
        int maxRowIndex = -1;
        int maxColIndex = -1;
        float maxRowMag = 0;
        float maxColMag = 0;

        // 1. Find strongest ROW
        for (int i = 0; i < 4; i++) {
            float mag = Goertzel(ROW_FREQUENCIES[i], buff, buffSize);
            if (mag > maxRowMag) {
                maxRowMag = mag;
                maxRowIndex = i;
            }
        }

        // 2. Find strongest COL
        for (int i = 0; i < 4; i++) {
            float mag = Goertzel(COL_FREQUENCIES[i], buff, buffSize);
            if (mag > maxColMag) {
                maxColMag = mag;
                maxColIndex = i;
            }
        }

        char currentKey = 0;

        // 3. Validation
        if (maxRowMag > DTMF_THRESHOLD && maxColMag > DTMF_THRESHOLD) {
            currentKey = DTMF_KEYS[maxRowIndex][maxColIndex];
        }

        // 4. Sequence Logic
        // We only add to sequence if the key is valid AND different from the last frame.
        // This acts as a simple debounce/edge-trigger so holding 'A' doesn't fill the buffer.
        if (currentKey != 0 && currentKey != lastDecodedKey) {
            addToSequence(currentKey);
        }

        lastDecodedKey = currentKey;
        return currentKey;
    }

    /**
     * Returns the latest N keys as a string (e.g., "12345")
     */
    String getSequence() {
        String s = "";
        for (int i = 0; i < SEQ_SIZE; i++) {
            if (sequence[i] != 0) {
                s += sequence[i];
            }
        }
        return s;
    }

    /**
     * Clears the sequence history.
     */
    void clearSequence() {
        memset(sequence, 0, SEQ_SIZE);
        lastDecodedKey = 0;
    }
};

#endif