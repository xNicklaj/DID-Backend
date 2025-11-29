#include "EQMeter.h"
#include <math.h>

#if __has_include(<ArduinoFFT.h>)
#include <ArduinoFFT.h>
#define EQ_USE_TEMPLATE_ARDUINOFFT 1
#elif __has_include(<arduinoFFT.h>)
#include <arduinoFFT.h>
#define EQ_USE_LEGACY_ARDUINOFFT 1
#else
#error "arduinoFFT library not found"
#endif

EQMeter::EQMeter() {
    for (int i = 0; i < EQ_PIXELS; ++i) {
        peak[i] = 1.0f; // avoid div0
        noiseFloor[i] = 0.0f;
    }
    // initialize smoothing/hysteresis state
    for (int i = 0; i < EQ_PIXELS; ++i) {
        smoothedLevel[i] = 0.0f;
        bandActive[i] = false;
    }
}

void EQMeter::setup(){
    strip.begin();
    strip.show();
}

// Compute band energies with an FFT and return per-band magnitudes in `out` (array length EQ_PIXELS)
void EQMeter::computeBandEnergies(int32_t* buff, int buffSize, float* out) {
    // Prepare FFT arrays (real, imag)
    static double vReal[FFT_SIZE];
    static double vImag[FFT_SIZE];
    // Use available arduinoFFT variant and bind it to our arrays
#if defined(EQ_USE_TEMPLATE_ARDUINOFFT)
    ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, FFT_SIZE, (double)SAMPLING_RATE);
#elif defined(EQ_USE_LEGACY_ARDUINOFFT)
    arduinoFFT FFT(vReal, vImag, FFT_SIZE, (double)SAMPLING_RATE);
#endif

    // We will use up to FFT_SIZE samples. If buffSize < FFT_SIZE, zero-pad; if > FFT_SIZE, use latest FFT_SIZE samples.
    int start = 0;
    if (buffSize >= FFT_SIZE) start = buffSize - FFT_SIZE; // take the most recent samples

    // copy samples (we'll apply FFT library windowing)
    for (int i = 0; i < FFT_SIZE; ++i) {
        double sample = 0.0;
        if (i + start < buffSize) sample = (double)buff[i + start];
        vReal[i] = sample;
        vImag[i] = 0.0;
    }
    // apply windowing and compute FFT using the available library API
#if defined(EQ_USE_TEMPLATE_ARDUINOFFT)
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();
#elif defined(EQ_USE_LEGACY_ARDUINOFFT)
    FFT.Windowing(vReal, FFT_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, FFT_SIZE);
#endif

    // vReal now holds magnitudes for bins 0..FFT_SIZE/2
    int nbins = FFT_SIZE / 2;
    // Calculate band edges as geometric midpoints between centers
    double edges[EQ_PIXELS + 1];
    for (int i = 0; i < EQ_PIXELS; ++i) {
        double c = bands[i];
        if (i == 0) {
            edges[0] = max(0.0, c / sqrt(bands[1] / bands[0]));
        }
    }
    // Simpler: compute boundaries as geometric mean between adjacent centers
    for (int i = 0; i < EQ_PIXELS - 1; ++i) {
        edges[i + 1] = sqrt((double)bands[i] * (double)bands[i + 1]);
    }
    // last upper edge: half way to Nyquist
    edges[EQ_PIXELS] = (double) (SAMPLING_RATE / 2);
    // first edge lower bound
    edges[0] = 0.0; // start from 0 Hz

    // For each band, sum magnitudes of bins whose center frequency lies between edges[b] and edges[b+1]
    double binWidth = (double)SAMPLING_RATE / (double)FFT_SIZE;
    for (int b = 0; b < EQ_PIXELS; ++b) {
        int binLo = max(1, (int)floor(edges[b] / binWidth));
        int binHi = min(nbins - 1, (int)ceil(edges[b + 1] / binWidth));
        double sum = 0.0;
        for (int k = binLo; k <= binHi; ++k) {
            sum += vReal[k];
        }
        out[b] = (float)sum;
    }
}

uint32_t EQMeter::levelToColor(float level){
    // clamp
    if (level <= 0.0f) return strip.Color(0,0,0);
    if (level > 1.0f) level = 1.0f;

    // green (0,150,0) -> yellow (150,150,0) -> red (150,0,0)
    const int G1 = 0, R1 = 0;
    const int G2 = 150, R2 = 150;
    const int G3 = 0, R3 = 150;

    if (level <= 0.5f) {
        // interpolate green -> yellow
        float t = level / 0.5f;
        int r = (int)(R1 + t * (R2 - R1));
        int g = (int)(G1 + t * (G2 - G1));
        return strip.Color(r, g, 0);
    } else {
        // interpolate yellow -> red
        float t = (level - 0.5f) / 0.5f;
        int r = (int)(R2 + t * (R3 - R2));
        int g = (int)(G2 + t * (G3 - G2));
        return strip.Color(r, g, 0);
    }
}

void EQMeter::update(){
    if (listener == nullptr) return;

    int32_t* buffer = listener->getBuffer();
    size_t bytesRead = listener->getBytesRead();
    int samples = bytesRead / 4;
    if (samples <= 0) return;

    // Compute band energies using FFT
    float energies[EQ_PIXELS];
    computeBandEnergies(buffer, samples, energies);

    // For each band compute magnitude
    for (int b = 0; b < EQ_PIXELS; ++b) {
        float mag = energies[b];

        // For the first three bands apply a slow-adapting noise-floor subtraction
        float magAdjusted = mag;
        if (b < 3) {
            // update noise floor: rise slower, decay slightly faster
            if (mag > noiseFloor[b]) {
                noiseFloor[b] = floorRise * noiseFloor[b] + (1.0f - floorRise) * mag;
            } else {
                noiseFloor[b] = floorDecay * noiseFloor[b] + (1.0f - floorDecay) * mag;
            }
            magAdjusted = mag - noiseFloor[b];
            if (magAdjusted < 0.0f) magAdjusted = 0.0f;
        }

        // update peak tracker on adjusted magnitude (so normalization ignores floor)
        if (magAdjusted > peak[b]) {
            peak[b] = peakAttack * peak[b] + (1.0f - peakAttack) * magAdjusted;
        } else {
            peak[b] = peak[b] * peakDecay;
            if (peak[b] < 1.0f) peak[b] = 1.0f;
        }

        // normalized level using adjusted magnitude
        float level = magAdjusted / (peak[b] + 1e-12f);
        if (level > 1.0f) level = 1.0f;

        // temporal smoothing to avoid flicker
        smoothedLevel[b] = smoothingAlpha * level + (1.0f - smoothingAlpha) * smoothedLevel[b];

        // hysteresis: require a lower value to turn off
        if (!bandActive[b]) {
            if (smoothedLevel[b] >= displayThreshold) bandActive[b] = true;
        } else {
            if (smoothedLevel[b] < displayThreshold * hysteresisLow) bandActive[b] = false;
        }

        if (!bandActive[b]) {
            strip.setPixelColor(b, 0);
        } else {
            // use smoothedLevel for color to be stable
            uint32_t color = levelToColor(smoothedLevel[b]);
            strip.setPixelColor(b, color);
        }
    }

    strip.show();
}
