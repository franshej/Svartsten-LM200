#pragma once
#include "FFTProcessor.h"
#include "SpectrumAnalyzerView.h"
#include "AmpButton.h"
#include <juce_audio_utils/juce_audio_utils.h>


class AudioSpectrumViewer : public juce::AudioAppComponent,
                            private juce::Timer
{
public:
    AudioSpectrumViewer();
    ~AudioSpectrumViewer() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Channel selection methods
    void setSelectedChannels(const std::vector<int>& channels);
    const std::vector<int>& getSelectedChannels() const { return selectedChannels; }

private:
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void timerCallback() override;
    void setupAudioPermissions();
    void setupDefaultAudioDevice();
    void onSettingsToggled(bool isOn);
    void onMoodeToggled(bool isOn);
    void onRcaToggled(bool isOn);
    void onLeftChannelToggled(bool isOn);
    void onRightChannelToggled(bool isOn);
    void updateChannelButtonStates();

    juce::AudioDeviceSelectorComponent audioSetupComp;
    size_t numChannels {2};
    FFTProcessor fftProcessor;
    SpectrumAnalyzerView analyzerView;
    bool updatePlotXData {true};
    bool settingsVisible {false};
    std::vector<int> selectedChannels {0};

    AmpButton m_button_on_off{"On/Off", "img/on_off_icon_on.png", "img/on_off_icon_off.png"};
    AmpButton m_button_rca{"RCA", "img/rca_icon_on.png", "img/rca_icon_off.png"};
    AmpButton m_button_moode{"Moode", "img/moode_icon_on.png", "img/moode_icon_off.png"};
    AmpButton m_button_settings{"Settings", "img/settings_icon_on.png", "img/settings_icon_off.png"};
    
    AmpButton m_button_left_channel{"Left", "img/on_off_icon_on.png", "img/on_off_icon_off.png"};
    AmpButton m_button_right_channel{"Right", "img/on_off_icon_on.png", "img/on_off_icon_off.png"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSpectrumViewer)
}; 