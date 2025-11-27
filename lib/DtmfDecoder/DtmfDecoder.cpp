#include "DtmfDecoder.h"
#include <math.h>

DTMFDecoder::DTMFDecoder() {
    lastDecodedKey = 0;
    memset(sequence, 0, SEQ_SIZE);
}

float DTMFDecoder::Goertzel(float freq0, int32_t* buff, int buffSize) {
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

void DTMFDecoder::addToSequence(char key) {
    if(key == sequence[SEQ_SIZE - 1]) {
        // Same as last key, ignore
        return;
    }
    for (int i = 0; i < SEQ_SIZE - 1; i++) {
        sequence[i] = sequence[i + 1];
    }
    sequence[SEQ_SIZE - 1] = key;
    Serial.printf("Current Sequence: %s\n", getSequence().c_str());
}

char DTMFDecoder::detectDTMF(int32_t* buff, int buffSize) {
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
    if (currentKey != 0 && currentKey != lastDecodedKey) {
        addToSequence(currentKey);
    }

    lastDecodedKey = currentKey;
    return currentKey;
}

String DTMFDecoder::getSequence() {
    String s = "";
    for (int i = 0; i < SEQ_SIZE; i++) {
        if (sequence[i] != 0) {
            s += sequence[i];
        }
    }
    return s;
}

void DTMFDecoder::clearSequence() {
    memset(sequence, 0, SEQ_SIZE);
    lastDecodedKey = 0;
}
