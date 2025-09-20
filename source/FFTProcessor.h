#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <mutex>

class FFTProcessor
{
public:
    static constexpr auto fftOrder = 11;
    static constexpr auto fifoSize = 1 << fftOrder;
    static constexpr auto fftSize = fifoSize << 1;

    FFTProcessor(size_t numChannels);
    void prepare(double sampleRate, size_t numChannels);
    void pushNextSample(float sample, size_t channel);
    void pushSamples(const float* samples, int numSamples, size_t channel);
    bool isNextFFTBlockReady(size_t channel) const { 
        return channel < numChannels ? nextFFTBlockReady[channel] : false; 
    }
    void clearFFTBlockReady(size_t channel) { 
        if (channel < numChannels) 
            nextFFTBlockReady[channel] = false; 
    }
    const std::vector<std::vector<float>>& getFrequencyData() const { return fftData; }
    const std::vector<std::vector<float>>& getXData() const { return xData; }
    void waitForReady();

private:
    void calculateFrequencyResponse(size_t channel);

    size_t numChannels {2};
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::vector<std::vector<float>> fifo;
    std::vector<std::vector<float>> fftData;
    std::vector<std::vector<float>> fftDataSmooth;
    std::vector<std::vector<float>> xData;
    std::vector<size_t> fifoIndex;
    std::vector<bool> nextFFTBlockReady;

    mutable std::mutex readyMutex;
}; 