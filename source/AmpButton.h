#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <functional>

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

  // Set callback for toggle state changes
  void setToggleCallback(std::function<void(bool)> callback) {
    m_toggleCallback = callback;
  }

  void clicked() override {
    juce::ToggleButton::clicked();
    if (m_toggleCallback) {
      m_toggleCallback(getToggleState());
    }
  }

  void paintButton(juce::Graphics& g, bool isMouseOverButton,
                   bool isButtonDown) override {
    auto bounds = getLocalBounds().toFloat();
    auto iconBounds = bounds.reduced(0.0f);
    
    if (this->getToggleState()) {
      // Create white gradient for "pressed" effect (bottom to top)
      juce::ColourGradient gradient(
        juce::Colours::white.withAlpha(0.3f), bounds.getBottomLeft(),
        juce::Colours::white.withAlpha(0.0f), bounds.getTopLeft(),
        false
      );
      
      // Fill the button with the gradient
      g.setGradientFill(gradient);
      g.fillRoundedRectangle(bounds, 8.0f);
      
      // Add a subtle inner glow effect
      g.setColour(juce::Colours::white.withAlpha(0.05f));
      g.drawRoundedRectangle(bounds.reduced(1.0f), 7.0f, 1.5f);
      
      // Draw the icon with full opacity
      g.setOpacity(1.0f);
      g.drawImageWithin(m_down_icon, iconBounds.getX(), iconBounds.getY(),
                        iconBounds.getWidth(), iconBounds.getHeight(),
                        juce::RectanglePlacement::centred);
    } else {
      // Normal state - just draw icon with reduced opacity
      g.setOpacity(0.7f);
      g.drawImageWithin(m_up_icon, iconBounds.getX(), iconBounds.getY(),
                  iconBounds.getWidth(), iconBounds.getHeight(),
                  juce::RectanglePlacement::centred);
      g.setOpacity(1.0f);
    }
  }

 private:
  juce::Image m_down_icon, m_up_icon;
  std::function<void(bool)> m_toggleCallback;
};
