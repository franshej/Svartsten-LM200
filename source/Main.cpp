#include "AudioSpectrumViewer.h"
#include <thread>

#ifdef __linux__
    #include <sys/resource.h>
    #include <unistd.h>
#endif

class AudioSpectrumViewerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Audio Spectrum Viewer"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const juce::String&) override
    {
#ifdef __linux__
        setpriority(PRIO_PROCESS, 0, 10);
#endif
        
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