#ifndef EQ_METER_H
#define EQ_METER_H

#include <Arduino.h>
#include <Worker.h>
#include <SoundListener.h>
#include <Adafruit_NeoPixel.h>

// Eight-band graphic equalizer worker
// Note: sampling rate in the project is 8000 Hz, so frequencies above ~4000 Hz
// cannot be resolved. The original requested bands (which included 6.5 kHz and
// 16 kHz) are not usable at 8 kHz sampling. This EQ uses bands appropriate for
// an 8 kHz sampling rate.

const uint16_t EQ_PIXELS = 8;
const uint8_t  EQ_PIN    = 13; // same pin as VUMeter by default; change if needed

class EQMeter : public Worker {
private:
    SoundListener* listener = nullptr;
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(EQ_PIXELS, EQ_PIN, NEO_GRB + NEO_KHZ800);

    // Center frequencies for the eight bands (Hz). Chosen to fit under Nyquist (4kHz).
    const float bands[EQ_PIXELS] = {63.0, 160.0, 400.0, 1000.0, 2000.0, 2500.0, 3000.0, 3800.0};

    // Peak tracker per band for adaptive normalization
    float peak[EQ_PIXELS];
    // Per-band noise floor / offset estimator (to remove DC/static bias)
    float noiseFloor[EQ_PIXELS];

    // smoothing / decay parameters
    const float peakDecay = 0.995f;
    const float peakAttack = 0.8f;
    // noise floor update factors (rise = how quickly floor follows increases,
    // decay = how quickly it follows decreases). Values close to 1.0 = slow.
    const float floorRise = 0.995f;
    const float floorDecay = 0.9995f;
    // Minimum normalized level required to light a pixel. [0..1]
    float displayThreshold = 0.7f;

    // Per-band smoothed level to avoid flicker
    float smoothedLevel[EQ_PIXELS];
    // per-band active state
    bool bandActive[EQ_PIXELS];

    // Smoothing factor for temporal smoothing (0..1). Larger = more responsive.
    float smoothingAlpha = 0.3f;
    // Hysteresis factor: when active, require level < displayThreshold * hysteresisLow
    // to turn off. hysteresisLow should be in (0..1). e.g., 0.6
    float hysteresisLow = 0.4f;

public:
    // Setter for display threshold (0..1). Values <= 0 disable thresholding.
    void setDisplayThreshold(float t) { displayThreshold = t; }
    void setSmoothingAlpha(float a) { smoothingAlpha = a; }
    void setHysteresisLow(float f) { hysteresisLow = f; }

    // FFT-based band energy helper (replaces Goertzel approach)
    // FFT size (power of two)
    static const int FFT_SIZE = 256;
    // Compute band energies using FFT and write per-band magnitudes into `out` (length EQ_PIXELS)
    void computeBandEnergies(int32_t* buff, int buffSize, float* out);

    // map normalized level [0..1] to color (green->yellow->red)
    uint32_t levelToColor(float level);

public:
    EQMeter();
    void setup() override;
    void update() override;

    void setListener(SoundListener* lst){ listener = lst; }
};

#endif
