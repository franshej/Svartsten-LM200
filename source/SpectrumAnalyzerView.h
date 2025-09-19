#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "cmp_plot.h"
#include <cmp_lookandfeel.h>

class SpectrumAnalyzerLookAndFeel : public cmp::PlotLookAndFeel
{
public:
    SpectrumAnalyzerLookAndFeel();
    
    void overridePlotColours() noexcept override;
        
    void drawFrame(juce::Graphics &g, const juce::Rectangle<int> bounds) override {};
    
    void drawGridLine(juce::Graphics &g, const cmp::GridLine &grid_line, const cmp::GridType grid_type) override;
    
    void drawGridLabels(juce::Graphics &g, const cmp::LabelVector &x_axis_labels, const cmp::LabelVector &y_axis_labels) override;
    
    juce::Font getXYTitleFont() const noexcept override;
    
    juce::Font getGridLabelFont() const noexcept override;
    
    std::size_t getMargin() const noexcept override;
};

class SpectrumAnalyzerView : public juce::Component
{
public:
    SpectrumAnalyzerView();
    ~SpectrumAnalyzerView() override;
    void paint(juce::Graphics& g) override;
    void resized() override;
    void updatePlot(const std::vector<std::vector<float>>& fftData,
                   const std::vector<std::vector<float>>& xData,
                   bool firstUpdate = false,
                   const std::vector<int>& selectedChannels = {0, 1});
    
private:
    void setupPlot();
    cmp::SemiLogX m_plot;
    SpectrumAnalyzerLookAndFeel m_lnf;
}; 