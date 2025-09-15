#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "cmp_plot.h"
#include <cmp_lookandfeel.h>

class SpectrumAnalyzerView : public juce::Component
{
public:
    SpectrumAnalyzerView();
    void paint(juce::Graphics& g) override;
    void resized() override;
    void updatePlot(const std::vector<std::vector<float>>& fftData,
                   const std::vector<std::vector<float>>& xData,
                   bool firstUpdate = false);
    
private:
    void setupPlot();
    cmp::SemiLogX m_plot;
    cmp::PlotLookAndFeel m_lnf;
}; 