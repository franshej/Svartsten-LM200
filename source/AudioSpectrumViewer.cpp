#include "AudioSpectrumViewer.h"
#include "AmpColours.h"
#include "FFTProcessor.h"

AudioSpectrumViewer::AudioSpectrumViewer()
    : fftProcessor(1), 
    audioSetupComp(deviceManager, 0, 1, 0, 0, false, false, false, false)
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

    setupDefaultAudioDevice();

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

    // Add subtle separator line between main content and button area
    auto buttonAreaHeight = proportionOfHeight(0.2f);
    auto separatorY = getHeight() - buttonAreaHeight;
    
    g.setColour(juce::Colour(AmpColours::grey).withAlpha(0.3f));
    g.fillRect(0, separatorY - 1, getWidth(), 1);

    // Add subtle background for button area
    g.setColour(juce::Colour(AmpColours::background).brighter(0.05f));
    g.fillRect(0, separatorY, getWidth(), buttonAreaHeight);

    // Border around settings panel when visible
    if (settingsVisible && audioSetupComp.isVisible())
    {
        auto settingsBounds = audioSetupComp.getBounds().toFloat();
        
        // Add subtle drop shadow effect
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        g.fillRoundedRectangle(settingsBounds.translated(2, 2), 6.0f);
        
        // Main border
        g.setColour(juce::Colour(AmpColours::blue).withAlpha(0.4f));
        g.fillRoundedRectangle(settingsBounds, 6.0f);
        
        // Inner border
        g.setColour(juce::Colour(AmpColours::grey).withAlpha(0.8f));
        g.drawRoundedRectangle(settingsBounds.reduced(1), 5.0f, 1.5f);
    }

    // Add subtle border around analyzer area
    auto analyzerBounds = analyzerView.getBounds().toFloat();
    g.setColour(juce::Colour(AmpColours::grey).withAlpha(0.2f));
    g.drawRoundedRectangle(analyzerBounds, 4.0f, 1.0f);
}

void AudioSpectrumViewer::resized()
{
    auto rect = getLocalBounds();
    
    // Use less vertical space for buttons (20% instead of 30%)
    auto buttonAreaHeight = proportionOfHeight(0.2f);
    auto mainContentArea = rect.removeFromTop(getHeight() - buttonAreaHeight);

    if (settingsVisible)
    {
        auto analyzerArea = mainContentArea.removeFromLeft(proportionOfWidth(0.7f));
        auto settingsArea = mainContentArea.removeFromRight(proportionOfWidth(0.3f));
        
        // Add margins for better visual separation
        analyzerView.setBounds(analyzerArea.reduced(8, 8));
        audioSetupComp.setBounds(settingsArea.reduced(8, 8));
    }
    else
    {
        // Add margin when settings are hidden for better visual balance
        analyzerView.setBounds(mainContentArea.reduced(8, 8));
    }

    // Create button area with better spacing
    auto buttonArea = getLocalBounds().removeFromBottom(buttonAreaHeight);
    auto buttonMargin = 10;
    auto buttonSpacing = 8;
    
    buttonArea.reduce(buttonMargin, buttonMargin);
    
    auto singleButtonWidth = (buttonArea.getWidth() - (3 * buttonSpacing)) / 4;
    
    // Layout buttons with proper spacing
    m_button_on_off.setBounds(buttonArea.removeFromLeft(singleButtonWidth));
    buttonArea.removeFromLeft(buttonSpacing);
    
    m_button_rca.setBounds(buttonArea.removeFromLeft(singleButtonWidth));
    buttonArea.removeFromLeft(buttonSpacing);
    
    m_button_moode.setBounds(buttonArea.removeFromLeft(singleButtonWidth));
    buttonArea.removeFromLeft(buttonSpacing);
    
    m_button_settings.setBounds(buttonArea.removeFromLeft(singleButtonWidth));
}

void AudioSpectrumViewer::setupAudioPermissions()
{
    juce::RuntimePermissions::request(
        juce::RuntimePermissions::recordAudio,
        [this](bool granted) {
            int numInputChannels = granted ? 1 : 0;
            setAudioChannels(numInputChannels, 0);
        });
}

void AudioSpectrumViewer::setupDefaultAudioDevice()
{
    deviceManager.initialise(1, 0, nullptr, true, "Loopback, Loopback PCM; Direct sample snooping device (1)", nullptr);
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