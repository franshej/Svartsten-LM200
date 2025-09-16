#include "SpectrumAnalyzerView.h"
#include "AmpColours.h"
#include "../externals/CMP/include/include_internal/cmp_utils.h"

SpectrumAnalyzerLookAndFeel::SpectrumAnalyzerLookAndFeel()
{
    overridePlotColours();
}

void SpectrumAnalyzerLookAndFeel::overridePlotColours() noexcept
{
    setColour(cmp::Plot::background_colour, juce::Colour(AmpColours::background));
    setColour(cmp::Plot::grid_colour, juce::Colour(AmpColours::grey).withAlpha(0.15f));
    setColour(cmp::Plot::x_label_colour, juce::Colour(AmpColours::grey).withAlpha(0.9f));
    setColour(cmp::Plot::y_label_colour, juce::Colour(AmpColours::grey).withAlpha(0.9f));
    setColour(cmp::Plot::frame_colour, juce::Colour(AmpColours::blue).withAlpha(0.3f));
}

void SpectrumAnalyzerLookAndFeel::drawGridLine(juce::Graphics &g, const cmp::GridLine &grid_line, const cmp::GridType grid_type)
{
    auto lineColour = juce::Colour(AmpColours::grey);
    
    if (grid_line.type == cmp::GridLine::Type::normal)
    {
        g.setColour(lineColour.withAlpha(0.25f));
        
        juce::Line<float> line;
        if (grid_line.direction == cmp::GridLine::Direction::horizontal)
        {
            line = juce::Line<float>(grid_line.position.x, grid_line.position.y,
                                    grid_line.position.x + grid_line.length, grid_line.position.y);
        }
        else
        {
            line = juce::Line<float>(grid_line.position.x, grid_line.position.y,
                                    grid_line.position.x, grid_line.position.y + grid_line.length);
        }
        g.drawLine(line, 1.0f);
    }
    else if (grid_line.type == cmp::GridLine::Type::translucent)
    {
        g.setColour(lineColour.withAlpha(0.1f));
        
        juce::Line<float> line;
        if (grid_line.direction == cmp::GridLine::Direction::horizontal)
        {
            line = juce::Line<float>(grid_line.position.x, grid_line.position.y,
                                    grid_line.position.x + grid_line.length, grid_line.position.y);
        }
        else
        {
            line = juce::Line<float>(grid_line.position.x, grid_line.position.y,
                                    grid_line.position.x, grid_line.position.y + grid_line.length);
        }
        g.drawLine(line, 0.5f);
    }
}

void SpectrumAnalyzerLookAndFeel::drawGridLabels(juce::Graphics &g, const cmp::LabelVector &x_axis_labels, const cmp::LabelVector &y_axis_labels)
{
    g.setFont(getGridLabelFont());
    g.setColour(juce::Colour(AmpColours::grey).withAlpha(0.8f));
    
    for (const auto& label : x_axis_labels)
    {
        g.drawText(label.first, label.second, juce::Justification::centred);
    }
    
    for (const auto& label : y_axis_labels)
    {
        g.drawText(label.first, label.second, juce::Justification::centred);
    }
}

juce::Font SpectrumAnalyzerLookAndFeel::getXYTitleFont() const noexcept
{
    return juce::Font("Arial", 14.0f, juce::Font::bold);
}

juce::Font SpectrumAnalyzerLookAndFeel::getGridLabelFont() const noexcept
{
    return juce::Font("Arial", 11.0f, juce::Font::plain);
}


std::size_t SpectrumAnalyzerLookAndFeel::getMargin() const noexcept
{
    return 12;
}


SpectrumAnalyzerView::SpectrumAnalyzerView()
{
    setupPlot();
    addAndMakeVisible(m_plot);
    m_plot.setLookAndFeel(&m_lnf);
}

SpectrumAnalyzerView::~SpectrumAnalyzerView()
{
    m_plot.setLookAndFeel(nullptr);
}

void SpectrumAnalyzerView::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(AmpColours::background));
}

void SpectrumAnalyzerView::resized()
{
    m_plot.setBounds(getLocalBounds());
}

void SpectrumAnalyzerView::updatePlot(const std::vector<std::vector<float>>& fftData,
                                     const std::vector<std::vector<float>>& xData,
                                     bool updatePlotXData)
{
    if (updatePlotXData)
    {
        cmp::GraphAttributeList attr(2);
        
        attr[0].gradient_colours = {
            juce::Colour(AmpColours::blue).withAlpha(0.8f),
            juce::Colour(AmpColours::blue).darker(0.3f).withAlpha(0.4f)};
            
        attr[1].gradient_colours = {
            juce::Colour(AmpColours::yellow).withAlpha(0.8f),
            juce::Colour(AmpColours::yellow).darker(0.4f).withAlpha(0.4f)};
            
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
    m_plot.setYLabel("Power [dB]");
    m_plot.setXLabel("Frequency [Hz]");
    m_plot.yLim(-60.0f, 10.0f);
    m_plot.xLim(100.0f, 22'000.0f);
    m_plot.setLegend({"Left input", "Right input"});
} 