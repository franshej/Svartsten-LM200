#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

class FFTProcessor
{
public:
    static constexpr auto fftOrder = 11;
    static constexpr auto fifoSize = 1 << fftOrder;
    static constexpr auto fftSize = fifoSize << 1;

    FFTProcessor();
    void prepare(double sampleRate);
    void pushNextSample(float sample, size_t channel);
    bool isNextFFTBlockReady(size_t channel) const { return nextFFTBlockReady[channel]; }
    void clearFFTBlockReady(size_t channel) { nextFFTBlockReady[channel] = false; }
    const std::vector<std::vector<float>>& getFrequencyData() const { return fftData; }
    const std::vector<std::vector<float>>& getXData() const { return xData; }

private:
    void calculateFrequencyResponse(size_t channel);

    static constexpr size_t numChannels = 2;
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

    std::vector<std::vector<float>> fifo = std::vector<std::vector<float>>(numChannels);
    std::vector<std::vector<float>> fftData = std::vector<std::vector<float>>(numChannels);
    std::vector<std::vector<float>> fftDataSmooth = std::vector<std::vector<float>>(numChannels);
    std::vector<std::vector<float>> xData = std::vector<std::vector<float>>(numChannels);
    std::array<size_t, numChannels> fifoIndex{};
    std::array<bool, numChannels> nextFFTBlockReady{};
}; 