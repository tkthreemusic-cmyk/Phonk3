#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PhonkSynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PhonkSynthLookAndFeel()
    {
        setColour (juce::Slider::rotarySliderFillColourId,   juce::Colour (0xff3cc9ff));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff142a40));
        setColour (juce::Slider::thumbColourId,               juce::Colour (0xffff3caa));
        setColour (juce::Label::textColourId,                 juce::Colours::white);
        setColour (juce::ComboBox::backgroundColourId,        juce::Colour (0xff17182f));
        setColour (juce::ComboBox::textColourId,               juce::Colours::white);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (4.0f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centre = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour (juce::Colour (0xff0d0d1a));
        g.fillEllipse (bounds);

        juce::Path arc;
        arc.addCentredArc (centre.x, centre.y, radius - 3.0f, radius - 3.0f, 0.0f,
                            rotaryStartAngle, angle, true);
        g.setColour (juce::Colour (0xff3cc9ff));
        g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour (juce::Colour (0xff2a3a55));
        g.drawEllipse (bounds, 1.5f);

        juce::Path pointer;
        auto pointerLength = radius * 0.7f;
        pointer.addRectangle (-1.5f, -pointerLength, 3.0f, pointerLength);
        pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
        g.setColour (juce::Colours::white);
        g.fillPath (pointer);
    }
};

struct SynthKnob
{
    juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

struct SynthCombo
{
    juce::ComboBox box;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> attachment;
};

class PhonkSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PhonkSynthAudioProcessorEditor (PhonkSynthAudioProcessor&);
    ~PhonkSynthAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void addKnob (SynthKnob& k, const juce::String& paramID, const juce::String& text);
    void addCombo (SynthCombo& c, const juce::String& paramID, const juce::String& text, const juce::StringArray& items);
    void addSectionLabel (juce::Label& label, const juce::String& text);
    void layoutSection (juce::Label& sectionLabel, juce::Rectangle<int> bounds, std::initializer_list<juce::Component*> comps);

    PhonkSynthAudioProcessor& processorRef;
    PhonkSynthLookAndFeel laf;

    juce::Label titleLabel;
    juce::Label osc1Label, osc2Label, subNoiseLabel, voiceLabel,
                ampEnvLabel, filterLabel, filterEnvLabel,
                driveLabel, lofiLabel, chorusLabel, reverbLabel;

    SynthCombo osc1WaveCombo, osc2WaveCombo, filterTypeCombo, driveModeCombo, voiceModeCombo;

    SynthKnob osc1LevelKnob, osc2LevelKnob, osc2DetuneKnob, subLevelKnob, noiseLevelKnob;
    SynthKnob ampAttackKnob, ampDecayKnob, ampSustainKnob, ampReleaseKnob;
    SynthKnob filterCutoffKnob, filterResoKnob, filterEnvAmtKnob, filterKeyTrackKnob;
    SynthKnob filterAttackKnob, filterDecayKnob, filterSustainKnob, filterReleaseKnob;
    SynthKnob glideTimeKnob;
    SynthKnob driveAmountKnob, driveToneKnob;
    SynthKnob bitDepthKnob, sampleReduceKnob, lofiMixKnob;
    SynthKnob chorusRateKnob, chorusDepthKnob, chorusMixKnob;
    SynthKnob reverbSizeKnob, reverbDampingKnob, reverbMixKnob;
    SynthKnob outputGainKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhonkSynthAudioProcessorEditor)
};
