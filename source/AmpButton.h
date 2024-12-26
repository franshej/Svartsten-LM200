#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class AmpButton : public juce::ToggleButton {
 public:
  AmpButton(const juce::String& buttonName, 
           const juce::String& onImagePath,
           const juce::String& offImagePath)
      : juce::ToggleButton(buttonName) {
    // Get the parent directory of the executable
    auto execDir = juce::File::getSpecialLocation(
                      juce::File::SpecialLocationType::currentExecutableFile)
                      .getParentDirectory();

    // Construct full paths
    auto onPath = execDir.getChildFile(onImagePath);
    auto offPath = execDir.getChildFile(offImagePath);

    // Load images
    m_down_icon = juce::ImageFileFormat::loadFrom(onPath);
    m_up_icon = juce::ImageFileFormat::loadFrom(offPath);

    if (m_down_icon.isNull() || m_up_icon.isNull()) {
      DBG("Failed to load image: " + onPath.getFullPathName() + " or " + offPath.getFullPathName());
    }
  }

  void paintButton(juce::Graphics& g, bool isMouseOverButton,
                   bool isButtonDown) override {
    auto bounds = getLocalBounds().toFloat();
    auto iconBounds = bounds.reduced(10.0f);
    if (this->getToggleState()) {
      g.drawImageWithin(m_down_icon, iconBounds.getX(), iconBounds.getY(),
                        iconBounds.getWidth(), iconBounds.getHeight(),
                        juce::RectanglePlacement::centred);
    } else {
      g.setOpacity(0.7f);
      g.drawImageWithin(m_up_icon, iconBounds.getX(), iconBounds.getY(),
                  iconBounds.getWidth(), iconBounds.getHeight(),
                  juce::RectanglePlacement::centred);
      g.setOpacity(1.0f);
    }
  }

 private:
  juce::Image m_down_icon, m_up_icon;
};
