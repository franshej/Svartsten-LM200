#include "AudioSpectrumViewer.h"
#include "AmpColours.h"
#include "FFTProcessor.h"
#include <algorithm>

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
    
    addAndMakeVisible(m_button_left_channel);
    addAndMakeVisible(m_button_right_channel);
    m_button_left_channel.setVisible(false);
    m_button_right_channel.setVisible(false);

    m_button_settings.setToggleCallback([this](bool isOn) {
        onSettingsToggled(isOn);
    });

    m_button_moode.setToggleCallback([this](bool isOn) {
        onMoodeToggled(isOn);
    });
    
    m_button_rca.setToggleCallback([this](bool isOn) {
        onRcaToggled(isOn);
    });
    
    m_button_left_channel.setToggleCallback([this](bool isOn) {
        onLeftChannelToggled(isOn);
    });
    
    m_button_right_channel.setToggleCallback([this](bool isOn) {
        onRightChannelToggled(isOn);
    });

    m_button_moode.setToggleState(true, juce::dontSendNotification);
    m_button_rca.setToggleState(false, juce::dontSendNotification);
    
    // Initialize channel button states (default: left only)
    updateChannelButtonStates();

    audioSetupComp.setVisible(false);

    setupDefaultAudioDevice();

    startTimerHz(25);
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
    
    if (bufferToFill.buffer->getNumChannels() > 0 && !selectedChannels.empty())
    {
        // Only process channels that are selected for plotting
        for (int selectedChannel : selectedChannels)
        {
            if (selectedChannel >= 0 && selectedChannel < bufferToFill.buffer->getNumChannels())
            {
                const auto* channelData = bufferToFill.buffer->getReadPointer(selectedChannel, bufferToFill.startSample);
                if (channelData != nullptr)
                {
                    for (auto i = 0; i < bufferToFill.numSamples; ++i)
                    {
                        fftProcessor.pushNextSample(channelData[i], selectedChannel);
                    }
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
        
    // If no channels selected, skip processing (no plot updates)
    if (selectedChannels.empty())
        return;
    
    // Only check readiness for selected channels
    bool allSelectedChannelsReady = true;
    for (int channelIndex : selectedChannels)
    {
        if (channelIndex >= 0 && channelIndex < static_cast<int>(numChannels))
        {
            allSelectedChannelsReady &= fftProcessor.isNextFFTBlockReady(channelIndex);
        }
    }

    if (allSelectedChannelsReady)
    {
        // Get data for selected channels only
        std::vector<std::vector<float>> selectedFftData;
        std::vector<std::vector<float>> selectedXData;
        
        const auto& allFftData = fftProcessor.getFrequencyData();
        const auto& allXData = fftProcessor.getXData();
        
        for (int channelIndex : selectedChannels)
        {
            if (channelIndex >= 0 && channelIndex < static_cast<int>(allFftData.size()))
            {
                selectedFftData.push_back(allFftData[channelIndex]);
                if (!allXData.empty() && channelIndex < static_cast<int>(allXData.size()))
                {
                    selectedXData.push_back(allXData[channelIndex]);
                }
            }
        }
        
        analyzerView.updatePlot(selectedFftData, 
                               selectedXData, 
                               updatePlotXData,
                               selectedChannels);
        
        // Clear ready flags only for selected channels
        for (int channelIndex : selectedChannels)
        {
            if (channelIndex >= 0 && channelIndex < static_cast<int>(numChannels))
            {
                fftProcessor.clearFFTBlockReady(channelIndex);
            }
        }
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
        
        // Reserve space for channel buttons at the top of settings area
        auto channelButtonArea = settingsArea.removeFromTop(60);
        
        // Layout channel buttons horizontally
        auto channelMargin = 8;
        channelButtonArea.reduce(channelMargin, channelMargin);
        auto buttonWidth = (channelButtonArea.getWidth() - channelMargin) / 2;
        
        m_button_left_channel.setBounds(channelButtonArea.removeFromLeft(buttonWidth));
        channelButtonArea.removeFromLeft(channelMargin);
        m_button_right_channel.setBounds(channelButtonArea.removeFromLeft(buttonWidth));
        
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
            int numInputChannels = granted ? 2 : 0;
            setAudioChannels(numInputChannels, 0);
        });
}

void AudioSpectrumViewer::setupDefaultAudioDevice()
{
    deviceManager.initialise(2, 0, nullptr, true, "Loopback, Loopback PCM; Direct sample snooping device (2)", nullptr);
}

void AudioSpectrumViewer::onSettingsToggled(bool isOn)
{
    settingsVisible = isOn;
    audioSetupComp.setVisible(isOn);
    m_button_left_channel.setVisible(isOn);
    m_button_right_channel.setVisible(isOn);
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

void AudioSpectrumViewer::setSelectedChannels(const std::vector<int>& channels)
{
    auto previousChannels = selectedChannels;
    selectedChannels = channels;
    
    for (auto& channel : selectedChannels)
    {
        if (channel < 0 || channel >= static_cast<int>(numChannels))
        {
            selectedChannels = {0};
            break;
        }
    }
    
    selectedChannels.erase(std::remove_if(selectedChannels.begin(), selectedChannels.end(),
                                         [](int ch) { return ch < 0 || ch > 1; }),
                          selectedChannels.end());
    
    
    updateChannelButtonStates();
}

void AudioSpectrumViewer::onLeftChannelToggled(bool isOn)
{
    std::vector<int> newChannels;
    
    if (isOn)
        newChannels.push_back(0);

    if (std::find(selectedChannels.begin(), selectedChannels.end(), 1) != selectedChannels.end() && 
        m_button_right_channel.getToggleState())
        newChannels.push_back(1);
    
    setSelectedChannels(newChannels);
}

void AudioSpectrumViewer::onRightChannelToggled(bool isOn)
{
    std::vector<int> newChannels;
    
    if (std::find(selectedChannels.begin(), selectedChannels.end(), 0) != selectedChannels.end() && 
        m_button_left_channel.getToggleState())
        newChannels.push_back(0);
    
    if (isOn)
        newChannels.push_back(1);
    
    setSelectedChannels(newChannels);
}

void AudioSpectrumViewer::updateChannelButtonStates()
{
    bool leftSelected = std::find(selectedChannels.begin(), selectedChannels.end(), 0) != selectedChannels.end();
    bool rightSelected = std::find(selectedChannels.begin(), selectedChannels.end(), 1) != selectedChannels.end();
    
    m_button_left_channel.setToggleState(leftSelected, juce::dontSendNotification);
    m_button_right_channel.setToggleState(rightSelected, juce::dontSendNotification);
} 