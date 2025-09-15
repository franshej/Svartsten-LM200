#include "AudioSpectrumViewer.h"
#include "AmpColours.h"
#include "FFTProcessor.h"

AudioSpectrumViewer::AudioSpectrumViewer()
    : fftProcessor(2), 
    audioSetupComp(deviceManager, 0, 2, 0, 0, false, false, false, false)
{
    setupAudioPermissions();

    addAndMakeVisible(audioSetupComp);
    addAndMakeVisible(analyzerView);
    addAndMakeVisible(m_button_on_off);
    addAndMakeVisible(m_button_rca);
    addAndMakeVisible(m_button_moode);
    addAndMakeVisible(m_button_settings);

    m_button_settings.setToggleCallback([this](bool isOn) {
        onSettingsToggled(isOn);
    });

    m_button_moode.setToggleCallback([this](bool isOn) {
        onMoodeToggled(isOn);
    });
    
    m_button_rca.setToggleCallback([this](bool isOn) {
        onRcaToggled(isOn);
    });

    m_button_moode.setToggleState(true, juce::dontSendNotification);
    m_button_rca.setToggleState(false, juce::dontSendNotification);

    audioSetupComp.setVisible(false);

    startTimerHz(30);
    setSize(800, 480);
}

AudioSpectrumViewer::~AudioSpectrumViewer()
{
    stopTimer();
    
    shutdownAudio();
}

void AudioSpectrumViewer::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate)
{
    numChannels = audioSetupComp.deviceManager.getAudioDeviceSetup().inputChannels.countNumberOfSetBits();
    fftProcessor.prepare(sampleRate, numChannels);
    updatePlotXData = true;
}


void AudioSpectrumViewer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (bufferToFill.buffer == nullptr)
    {
        return;
    }
    
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            const auto* channelData = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
            if (channelData != nullptr)
            {
                for (auto i = 0; i < bufferToFill.numSamples; ++i)
                {
                    fftProcessor.pushNextSample(channelData[i], channel);
                }
            }
        }
    }
    bufferToFill.clearActiveBufferRegion();
}

void AudioSpectrumViewer::timerCallback()
{
    if (!audioSetupComp.deviceManager.getCurrentAudioDevice())
        return;
        
    bool allChannelsReady = true;
    
    if (numChannels == 0)
        return;
    
    for (size_t i = 0; i < numChannels; ++i)
        allChannelsReady &= fftProcessor.isNextFFTBlockReady(i);

    if (allChannelsReady)
    {
        analyzerView.updatePlot(fftProcessor.getFrequencyData(), 
                               fftProcessor.getXData(), 
                               updatePlotXData);
        for (size_t i = 0; i < numChannels; ++i)
            fftProcessor.clearFFTBlockReady(i);
        updatePlotXData = false;
    }
}

void AudioSpectrumViewer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(AmpColours::background));

    if (settingsVisible && audioSetupComp.isVisible())
    {
        g.setColour(juce::Colour(AmpColours::blue));
        g.setOpacity(0.5f);
        g.drawRoundedRectangle(audioSetupComp.getBounds().toFloat(), 4.0f, 4.0f);
        g.setOpacity(1.0f);
        g.setColour(juce::Colour(AmpColours::grey));
        g.drawRoundedRectangle(audioSetupComp.getBounds().toFloat(), 2.0f, 2.0f);
    }
}

void AudioSpectrumViewer::resized()
{
    auto rect = getLocalBounds();

    if (settingsVisible)
    {
        analyzerView.setBounds(rect.removeFromLeft(proportionOfWidth(0.7f))
                              .removeFromTop(proportionOfHeight(0.7f)));
        audioSetupComp.setBounds(rect.removeFromRight(proportionOfWidth(0.3f))
                                .removeFromTop(proportionOfHeight(0.7f)));
    }
    else
    {
        analyzerView.setBounds(rect.removeFromTop(proportionOfHeight(0.7f)));
    }

    auto rect_button = getLocalBounds()
                          .removeFromBottom(proportionOfHeight(0.3f))
                          .removeFromLeft(proportionOfWidth(0.25f));
    m_button_on_off.setBounds(rect_button);

    rect_button.setX(rect_button.getRight());
    m_button_rca.setBounds(rect_button);

    rect_button.setX(rect_button.getRight());
    m_button_moode.setBounds(rect_button);

    rect_button.setX(rect_button.getRight());
    m_button_settings.setBounds(rect_button);
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

void AudioSpectrumViewer::onSettingsToggled(bool isOn)
{
    settingsVisible = isOn;
    audioSetupComp.setVisible(isOn);
    resized();
}

void AudioSpectrumViewer::onMoodeToggled(bool isOn)
{
    m_button_rca.setToggleState(!isOn, juce::dontSendNotification);
}

void AudioSpectrumViewer::onRcaToggled(bool isOn)
{
    m_button_moode.setToggleState(!isOn, juce::dontSendNotification);
}

void AudioSpectrumViewer::releaseResources()
{
    // Nothing to do here
} 