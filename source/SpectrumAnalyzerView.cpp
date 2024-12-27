#include "SpectrumAnalyzerView.h"
#include "AmpColours.h"


SpectrumAnalyzerView::SpectrumAnalyzerView()
{
    setupPlot();
    addAndMakeVisible(m_plot);
}

void SpectrumAnalyzerView::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(AmpColours::blue));
    g.setOpacity(0.5f);
    g.drawRoundedRectangle(m_plot.getBounds().toFloat(), 10.0f, 3.0f);
    g.setOpacity(1.0f);
    g.setColour(juce::Colour(AmpColours::grey));
    g.drawRoundedRectangle(m_plot.getBounds().toFloat(), 10.0f, 1.0f);
}

void SpectrumAnalyzerView::resized()
{
    m_plot.setBounds(getLocalBounds());
}

void SpectrumAnalyzerView::updatePlot(const std::vector<std::vector<float>>& fftData,
                                     const std::vector<std::vector<float>>& xData,
                                     bool firstUpdate)
{
    if (firstUpdate)
    {
        cmp::GraphAttributeList attr(2);
        attr[0].gradient_colours = {
            juce::Colour(juce::Colours::aqua).withAlpha(0.70f),
            juce::Colour(juce::Colours::whitesmoke).withAlpha(0.40f)};
        attr[1].gradient_colours = {
            juce::Colour(juce::Colours::rebeccapurple).withAlpha(0.70f),
            juce::Colour(juce::Colours::navajowhite).withAlpha(0.40f)};
            
        m_plot.plot(fftData, xData, attr);
    }
    else
    {
        m_plot.plotUpdateYOnly(fftData);
    }
}

void SpectrumAnalyzerView::setupPlot()
{
    m_plot.setDownsamplingType(cmp::DownsamplingType::x_downsampling);
    m_plot.setTitle("Left & Right input frequency information");
    m_plot.setYLabel("Power [dB]");
    m_plot.setXLabel("Frequency [Hz]");
    m_plot.yLim(-60.0f, 10.0f);
    m_plot.xLim(100.0f, 22'000.0f);
    m_plot.setLegend({"Left input", "Right input"});
} 