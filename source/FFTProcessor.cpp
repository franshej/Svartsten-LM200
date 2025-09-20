#include "FFTProcessor.h"
#include <cstring>

FFTProcessor::FFTProcessor(size_t numChannels)
    : numChannels(numChannels),
    forwardFFT(fftOrder),
    window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    fifo.resize(numChannels);
    fftData.resize(numChannels);
    fftDataSmooth.resize(numChannels);
    xData.resize(numChannels);
    fifoIndex.resize(numChannels);
    nextFFTBlockReady.resize(numChannels);

    for (auto& f : fifo)
        f.resize(fifoSize);
    for (auto& f : fftData)
        f.resize(fftSize);
    for (auto& f : fftDataSmooth)
        f.resize(fftSize);
    for (auto& x : xData)
        x.resize(fftSize);
    
    std::fill(fifoIndex.begin(), fifoIndex.end(), 0);
    std::fill(nextFFTBlockReady.begin(), nextFFTBlockReady.end(), false);
}

void FFTProcessor::prepare(double sampleRate, size_t numChannels)
{
    std::lock_guard<std::mutex> lock(readyMutex);
    this->numChannels = numChannels;

    fifo.resize(numChannels);
    fftData.resize(numChannels);
    fftDataSmooth.resize(numChannels);
    xData.resize(numChannels);
    fifoIndex.resize(numChannels);
    nextFFTBlockReady.resize(numChannels);

    for (auto& f : fifo)
        f.resize(fifoSize);
    for (auto& f : fftData)
        f.resize(fftSize);
    for (auto& f : fftDataSmooth)
        f.resize(fftSize);
    for (auto& x : xData)
        x.resize(fftSize);

    std::fill(fifoIndex.begin(), fifoIndex.end(), 0);
    std::fill(nextFFTBlockReady.begin(), nextFFTBlockReady.end(), false);

    for (auto& x : xData) {
        float delta = static_cast<float>(sampleRate + 1) / static_cast<float>(fftSize) * 2;
        for (size_t i = 0; i < x.size(); ++i)
            x[i] = i * delta;
    }
}

void FFTProcessor::pushNextSample(float sample, size_t channel)
{
    if (fifoIndex[channel] == fifoSize)
    {
        if (!nextFFTBlockReady[channel])
        {
            std::copy(fifo[channel].begin(), fifo[channel].end(), fftData[channel].begin());
            calculateFrequencyResponse(channel);
            nextFFTBlockReady[channel] = true;
        } else {
            return;
        }
        fifoIndex[channel] = 0;
    }

    fifo[channel][fifoIndex[channel]++] = sample;
}

void FFTProcessor::pushSamples(const float* samples, int numSamples, size_t channel)
{
    if (channel >= numChannels || samples == nullptr || numSamples <= 0)
        return;
    
    int samplesRemaining = numSamples;
    int sampleIndex = 0;
    
    while (samplesRemaining > 0)
    {
        if (fifoIndex[channel] == fifoSize)
        {
            if (!nextFFTBlockReady[channel])
            {
                std::memcpy(fftData[channel].data(), fifo[channel].data(), fifoSize * sizeof(float));
                calculateFrequencyResponse(channel);
                nextFFTBlockReady[channel] = true;
            } else {
                return;
            }
            fifoIndex[channel] = 0;
        }
        
        int samplesToCopy = std::min(samplesRemaining, static_cast<int>(fifoSize - fifoIndex[channel]));
        std::memcpy(&fifo[channel][fifoIndex[channel]], &samples[sampleIndex], samplesToCopy * sizeof(float));
        
        fifoIndex[channel] += samplesToCopy;
        sampleIndex += samplesToCopy;
        samplesRemaining -= samplesToCopy;
    }
}

void FFTProcessor::calculateFrequencyResponse(size_t channel)
{
    window.multiplyWithWindowingTable(fftData[channel].data(), fftSize);
    forwardFFT.performFrequencyOnlyForwardTransform(fftData[channel].data());

    constexpr auto scale = 1.0f / float(fftSize);
    constexpr auto smoothingFactor = 0.5f;

    auto itSmooth = fftDataSmooth[channel].begin();
    for (auto& s : fftData[channel])
    {
        s = s * scale;
        *itSmooth++ = s = (*itSmooth + s) * smoothingFactor;

        if (s < 1e-7f)
        {
            s = -70;
            continue;
        }

        s = 10.0f * log10f(s);
    }
} 