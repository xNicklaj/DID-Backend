#include <Arduino.h>
#include <unity.h>

#include <DtmfDecoder.h>
#include <math.h>

// Helper: DTMF frequency tables (must match DTMFDecoder)
static const float ROW_FREQUENCIES[4] = {697.0, 770.0, 852.0, 941.0};
static const float COL_FREQUENCIES[4] = {1209.0, 1336.0, 1477.0, 1633.0};

// DTMF key map (must match DTMFDecoder)
static const char DTMF_KEYS[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Find the two frequencies for a given key char. Returns true if found.
bool frequencies_for_key(char key, float &f1, float &f2) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (DTMF_KEYS[r][c] == key) {
                f1 = ROW_FREQUENCIES[r];
                f2 = COL_FREQUENCIES[c];
                return true;
            }
        }
    }
    return false;
}

// Generate a buffer containing the two-tone DTMF signal for `key`.
// buffSize samples, sampling rate 8000 (DTMFDecoder uses that macro).
// amplitude is peak per-sine amplitude (sum of two sines may reach 2*amplitude).
void generate_dtmf_buffer(char key, int32_t* buff, int buffSize, double amplitude) {
    float f1, f2;
    bool ok = frequencies_for_key(key, f1, f2);
    if (!ok) {
        // Fill zeros
        for (int i = 0; i < buffSize; ++i) buff[i] = 0;
        return;
    }

    const double fs = (double)SAMPLING_RATE;
    for (int n = 0; n < buffSize; ++n) {
        double t = (double)n / fs;
        double s = amplitude * sin(2.0 * M_PI * f1 * t) + amplitude * sin(2.0 * M_PI * f2 * t);
        // Clamp to int32 range
        if (s > 2147483647.0) s = 2147483647.0;
        if (s < -2147483648.0) s = -2147483648.0;
        buff[n] = (int32_t) s;
    }
}

void test_detect_single_key(void) {
    const int buffSize = 256;
    int32_t buff[buffSize];

    // amplitude chosen so Goertzel magnitude exceeds DTMF_THRESHOLD
    // Increase amplitude so A * (N/2) > 1e9 (A > 2e9/N). For N=256, A>7.8e6
    double amplitude = 10000000.0; // 1e7 -> A*(N/2)=1e7*128=1.28e9

    DTMFDecoder dec;
    generate_dtmf_buffer('1', buff, buffSize, amplitude);
    char detected = dec.detectDTMF(buff, buffSize);
    TEST_ASSERT_EQUAL_INT((int)'1', (int)detected);
}

void test_detect_various_keys(void) {
    const int buffSize = 256;
    int32_t buff[buffSize];
    // Use larger amplitude so test signal exceeds the decoder's threshold
    double amplitude = 10000000.0;

    char keys[] = {'1','5','9','0','*','#','A','D'};
    for (size_t i = 0; i < sizeof(keys); ++i) {
        DTMFDecoder dec;
        generate_dtmf_buffer(keys[i], buff, buffSize, amplitude);
        char detected = dec.detectDTMF(buff, buffSize);
        char msg[32];
        sprintf(msg, "key %c -> detected %c", keys[i], detected);
        TEST_ASSERT_MESSAGE(detected == keys[i], msg);
    }
}

void test_no_tone_returns_zero(void) {
    const int buffSize = 256;
    int32_t buff[buffSize];

    // Generate a low-amplitude tone at 1000 Hz (not DTMF)
    double amplitude = 1000.0; // too small to exceed threshold
    const double fs = (double)SAMPLING_RATE;
    for (int n = 0; n < buffSize; ++n) {
        double t = (double)n / fs;
        double s = amplitude * sin(2.0 * M_PI * 1000.0 * t);
        buff[n] = (int32_t)s;
    }

    DTMFDecoder dec;
    char detected = dec.detectDTMF(buff, buffSize);
    TEST_ASSERT_EQUAL_INT(0, (int)detected);
}

void setup() {
    delay(200);
    UNITY_BEGIN();
    RUN_TEST(test_detect_single_key);
    RUN_TEST(test_detect_various_keys);
    RUN_TEST(test_no_tone_returns_zero);
    UNITY_END();
}

void loop() {
}
