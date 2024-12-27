#include "AudioSpectrumViewer.h"
#include "AmpColours.h"

AudioSpectrumViewer::AudioSpectrumViewer()
    : audioSetupComp(deviceManager, 0, 2, 0, 0, false, false, false, false)
{
    setupAudioPermissions();

    addAndMakeVisible(audioSetupComp);
    addAndMakeVisible(analyzerView);
    addAndMakeVisible(m_button_on_off);
    addAndMakeVisible(m_button_rca);
    addAndMakeVisible(m_button_moode);

    startTimerHz(30);
    setSize(800, 480);
}

AudioSpectrumViewer::~AudioSpectrumViewer()
{
}

void AudioSpectrumViewer::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate)
{
    fftProcessor.prepare(sampleRate);
}


void AudioSpectrumViewer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            const auto* channelData = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
            for (auto i = 0; i < bufferToFill.numSamples; ++i){
                fftProcessor.pushNextSample(channelData[i], channel);
            }
        }
    }
    bufferToFill.clearActiveBufferRegion();
}

void AudioSpectrumViewer::timerCallback()
{
    bool allChannelsReady = true;
    for (size_t i = 0; i < 2; ++i)
        allChannelsReady &= fftProcessor.isNextFFTBlockReady(i);

    if (allChannelsReady)
    {
        static bool firstUpdate = true;
        analyzerView.updatePlot(fftProcessor.getFrequencyData(), 
                               fftProcessor.getXData(), 
                               firstUpdate);
        fftProcessor.clearFFTBlockReady(0);
        fftProcessor.clearFFTBlockReady(1);
        firstUpdate = false;
    }
}

void AudioSpectrumViewer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(AmpColours::background));

    g.setColour(juce::Colour(AmpColours::blue));
    g.setOpacity(0.5f);
    g.drawRoundedRectangle(audioSetupComp.getBounds().toFloat(), 4.0f, 4.0f);
    g.setOpacity(1.0f);
    g.setColour(juce::Colour(AmpColours::grey));
    g.drawRoundedRectangle(audioSetupComp.getBounds().toFloat(), 2.0f, 2.0f);
}

void AudioSpectrumViewer::resized()
{
    auto rect = getLocalBounds();

    analyzerView.setBounds(rect.removeFromLeft(proportionOfWidth(0.7f))
                          .removeFromTop(proportionOfHeight(0.7f)));
    audioSetupComp.setBounds(rect.removeFromRight(proportionOfWidth(0.3f)));

    auto rect_button = getLocalBounds()
                          .removeFromBottom(proportionOfHeight(0.3f))
                          .removeFromLeft(proportionOfWidth(0.25f));
    m_button_on_off.setBounds(rect_button);

    rect_button.setX(rect_button.getRight());
    m_button_rca.setBounds(rect_button);

    rect_button.setX(rect_button.getRight());
    m_button_moode.setBounds(rect_button);
}

void AudioSpectrumViewer::setupAudioPermissions()
{
    juce::RuntimePermissions::request(
        juce::RuntimePermissions::recordAudio,
        [this](bool granted) {
            int numInputChannels = granted ? 2 : 0;
            setAudioChannels(numInputChannels, 0);
        });
}

void AudioSpectrumViewer::releaseResources()
{
    // Nothing to do here
} 