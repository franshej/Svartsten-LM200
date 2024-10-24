#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include <random>

class AmpButton : public juce::ToggleButton {
 public:
  AmpButton(const juce::String& buttonName, const juce::Image& down_icon_image,
            const juce::Image& up_icon_image)
      : juce::ToggleButton(buttonName),
        m_down_icon(down_icon_image),
        m_up_icon(up_icon_image) {
    if (m_down_icon.isNull()) {
      DBG("Failed to load image");
    }
  }

  void paintButton(juce::Graphics& g, bool isMouseOverButton,
                   bool isButtonDown) override {
    auto bounds = getLocalBounds().toFloat();
    g.setColour(isButtonDown ? juce::Colours::darkgrey
                             : juce::Colours::lightgrey);
    g.fillRect(bounds);

    auto iconBounds = bounds.reduced(10.0f);
    if (this->getToggleState()) {
      g.drawImageWithin(m_down_icon, iconBounds.getX(), iconBounds.getY(),
                        iconBounds.getWidth(), iconBounds.getHeight(),
                        juce::RectanglePlacement::centred);
    } else {
      g.drawImageWithin(m_up_icon, iconBounds.getX(), iconBounds.getY(),
                        iconBounds.getWidth(), iconBounds.getHeight(),
                        juce::RectanglePlacement::centred);
    }
  }

 private:
  juce::Image m_down_icon, m_up_icon;
};
