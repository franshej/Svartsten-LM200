#include "AudioSpectrumViewer.h"

class AudioSpectrumViewerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Audio Spectrum Viewer"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override { mainWindow = nullptr; }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name, juce::Colours::darkgrey, DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new AudioSpectrumViewer(), true);
            setResizable(true, true);
            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void closeButtonPressed() override { JUCEApplication::getInstance()->systemRequestedQuit(); }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(AudioSpectrumViewerApplication) 