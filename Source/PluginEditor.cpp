#include "PluginProcessor.h"
#include "PluginEditor.h"

PhonkSynthAudioProcessorEditor::PhonkSynthAudioProcessorEditor (PhonkSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&laf);

    titleLabel.setText ("PHONK SYNTH", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (28.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xff3cc9ff));
    addAndMakeVisible (titleLabel);

    addSectionLabel (osc1Label,      "OSC 1");
    addSectionLabel (osc2Label,      "OSC 2");
    addSectionLabel (subNoiseLabel,  "SUB / NOISE");
    addSectionLabel (voiceLabel,     "VOICE / GLIDE");
    addSectionLabel (ampEnvLabel,    "AMP ENV");
    addSectionLabel (filterLabel,    "FILTER");
    addSectionLabel (filterEnvLabel, "FILTER ENV");
    addSectionLabel (driveLabel,     "DRIVE");
    addSectionLabel (lofiLabel,      "LO-FI");
    addSectionLabel (chorusLabel,    "TAPE WOBBLE");
    addSectionLabel (reverbLabel,    "REVERB");

    juce::StringArray waveNames { "Saw", "Square", "Sine", "Triangle" };
    addCombo (osc1WaveCombo, SynthParamIDs::osc1Wave, "Wave", waveNames);
    addKnob  (osc1LevelKnob, SynthParamIDs::osc1Level, "Level");

    addCombo (osc2WaveCombo, SynthParamIDs::osc2Wave, "Wave", waveNames);
    addKnob  (osc2LevelKnob, SynthParamIDs::osc2Level, "Level");
    addKnob  (osc2DetuneKnob, SynthParamIDs::osc2Detune, "Detune");

    addKnob (subLevelKnob, SynthParamIDs::subLevel, "Sub");
    addKnob (noiseLevelKnob, SynthParamIDs::noiseLevel, "Noise");

    addCombo (voiceModeCombo, SynthParamIDs::voiceMode, "Mode", juce::StringArray { "Poly", "Mono" });
    addKnob  (glideTimeKnob, SynthParamIDs::glideTime, "Glide");

    addKnob (ampAttackKnob,  SynthParamIDs::ampAttack,  "Attack");
    addKnob (ampDecayKnob,   SynthParamIDs::ampDecay,   "Decay");
    addKnob (ampSustainKnob, SynthParamIDs::ampSustain, "Sustain");
    addKnob (ampReleaseKnob, SynthParamIDs::ampRelease, "Release");

    addCombo (filterTypeCombo, SynthParamIDs::filterType, "Type", juce::StringArray { "Low Pass", "High Pass", "Band Pass" });
    addKnob (filterCutoffKnob,   SynthParamIDs::filterCutoff,   "Cutoff");
    addKnob (filterResoKnob,     SynthParamIDs::filterReso,     "Resonance");
    addKnob (filterEnvAmtKnob,   SynthParamIDs::filterEnvAmt,   "Env Amt");
    addKnob (filterKeyTrackKnob, SynthParamIDs::filterKeyTrack, "KeyTrack");

    addKnob (filterAttackKnob,  SynthParamIDs::filterAttack,  "Attack");
    addKnob (filterDecayKnob,   SynthParamIDs::filterDecay,   "Decay");
    addKnob (filterSustainKnob, SynthParamIDs::filterSustain, "Sustain");
    addKnob (filterReleaseKnob, SynthParamIDs::filterRelease, "Release");

    addKnob (driveAmountKnob, SynthParamIDs::driveAmount, "Amount");
    addCombo (driveModeCombo, SynthParamIDs::driveMode, "Mode", juce::StringArray { "Tape", "Fuzz", "Crush" });
    addKnob (driveToneKnob,   SynthParamIDs::driveTone, "Tone");

    addKnob (bitDepthKnob,     SynthParamIDs::bitDepth,     "Bit Depth");
    addKnob (sampleReduceKnob, SynthParamIDs::sampleReduce, "SR Reduce");
    addKnob (lofiMixKnob,      SynthParamIDs::lofiMix,      "Mix");

    addKnob (chorusRateKnob,  SynthParamIDs::chorusRate,  "Rate");
    addKnob (chorusDepthKnob, SynthParamIDs::chorusDepth, "Depth");
    addKnob (chorusMixKnob,   SynthParamIDs::chorusMix,   "Mix");

    addKnob (reverbSizeKnob,    SynthParamIDs::reverbSize,    "Size");
    addKnob (reverbDampingKnob, SynthParamIDs::reverbDamping, "Damping");
    addKnob (reverbMixKnob,     SynthParamIDs::reverbMix,     "Mix");

    addKnob (outputGainKnob, SynthParamIDs::outputGain, "Output");

    setSize (1200, 800);
}

PhonkSynthAudioProcessorEditor::~PhonkSynthAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void PhonkSynthAudioProcessorEditor::addKnob (SynthKnob& k, const juce::String& paramID, const juce::String& text)
{
    k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
    addAndMakeVisible (k.slider);
    k.label.setText (text, juce::dontSendNotification);
    k.label.setJustificationType (juce::Justification::centred);
    k.label.setFont (juce::Font (12.0f));
    addAndMakeVisible (k.label);
    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, paramID, k.slider);
}

void PhonkSynthAudioProcessorEditor::addCombo (SynthCombo& c, const juce::String& paramID, const juce::String& text,
                                                const juce::StringArray& items)
{
    c.box.addItemList (items, 1);
    addAndMakeVisible (c.box);
    c.label.setText (text, juce::dontSendNotification);
    c.label.setJustificationType (juce::Justification::centred);
    c.label.setFont (juce::Font (12.0f));
    addAndMakeVisible (c.label);
    c.attachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processorRef.apvts, paramID, c.box);
}

void PhonkSynthAudioProcessorEditor::addSectionLabel (juce::Label& label, const juce::String& text)
{
    label.setText (text, juce::dontSendNotification);
    label.setFont (juce::Font (13.0f, juce::Font::bold));
    label.setJustificationType (juce::Justification::centredLeft);
    label.setColour (juce::Label::textColourId, juce::Colour (0xff3cc9ff));
    addAndMakeVisible (label);
}

void PhonkSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0b0b14));
    g.setColour (juce::Colour (0xff17182f));
    for (int i = 0; i < getWidth(); i += 40)
        g.drawVerticalLine (i, 0.0f, (float) getHeight());
}

// Place un knob dans un rectangle donné (utilitaire local)
static void placeKnobIn (juce::Rectangle<int> slot, SynthKnob& k, int knobSize, int labelHeight)
{
    k.slider.setBounds (slot.withSizeKeepingCentre (knobSize, knobSize).withY (slot.getY()));
    k.label.setBounds (slot.getX(), slot.getY() + knobSize, slot.getWidth(), labelHeight);
}

static void placeComboIn (juce::Rectangle<int> slot, SynthCombo& c, int labelHeight)
{
    c.label.setBounds (slot.removeFromTop (labelHeight));
    c.box.setBounds (slot.removeFromTop (24).reduced (2, 0));
}

void PhonkSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (16);
    const int knobSize = 68;
    const int labelHeight = 16;
    const int sectionHeight = 140;

    auto header = area.removeFromTop (40);
    titleLabel.setBounds (header.removeFromLeft (400));
    placeKnobIn (header.removeFromRight (100), outputGainKnob, knobSize, labelHeight);

    area.removeFromTop (8);

    // --- Ligne 1 : OSC1, OSC2, SUB/NOISE, VOICE ---
    auto row1 = area.removeFromTop (sectionHeight);
    int col1W = row1.getWidth() / 4;

    {
        auto col = row1.removeFromLeft (col1W);
        osc1Label.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        auto comboSlot = col.removeFromLeft (col.getWidth() / 2);
        placeComboIn (comboSlot, osc1WaveCombo, labelHeight);
        placeKnobIn (col, osc1LevelKnob, knobSize, labelHeight);
    }
    {
        auto col = row1.removeFromLeft (col1W);
        osc2Label.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        auto comboSlot = col.removeFromLeft (col.getWidth() / 3);
        placeComboIn (comboSlot, osc2WaveCombo, labelHeight);
        auto slotW = col.getWidth() / 2;
        placeKnobIn (col.removeFromLeft (slotW), osc2LevelKnob, knobSize, labelHeight);
        placeKnobIn (col, osc2DetuneKnob, knobSize, labelHeight);
    }
    {
        auto col = row1.removeFromLeft (col1W);
        subNoiseLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        auto slotW = col.getWidth() / 2;
        placeKnobIn (col.removeFromLeft (slotW), subLevelKnob, knobSize, labelHeight);
        placeKnobIn (col, noiseLevelKnob, knobSize, labelHeight);
    }
    {
        auto col = row1;
        voiceLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        auto comboSlot = col.removeFromLeft (col.getWidth() / 2);
        placeComboIn (comboSlot, voiceModeCombo, labelHeight);
        placeKnobIn (col, glideTimeKnob, knobSize, labelHeight);
    }

    area.removeFromTop (12);

    // --- Ligne 2 : AMP ENV, FILTER, FILTER ENV ---
    auto row2 = area.removeFromTop (sectionHeight);
    int ampW = row2.getWidth() * 3 / 10;
    int filterW = row2.getWidth() * 4 / 10;

    {
        auto col = row2.removeFromLeft (ampW);
        ampEnvLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 4;
        placeKnobIn (col.removeFromLeft (w), ampAttackKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), ampDecayKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), ampSustainKnob, knobSize, labelHeight);
        placeKnobIn (col, ampReleaseKnob, knobSize, labelHeight);
    }
    {
        auto col = row2.removeFromLeft (filterW);
        filterLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        auto comboSlot = col.removeFromLeft (col.getWidth() / 5);
        placeComboIn (comboSlot, filterTypeCombo, labelHeight);
        int w = col.getWidth() / 4;
        placeKnobIn (col.removeFromLeft (w), filterCutoffKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), filterResoKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), filterEnvAmtKnob, knobSize, labelHeight);
        placeKnobIn (col, filterKeyTrackKnob, knobSize, labelHeight);
    }
    {
        auto col = row2;
        filterEnvLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 4;
        placeKnobIn (col.removeFromLeft (w), filterAttackKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), filterDecayKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), filterSustainKnob, knobSize, labelHeight);
        placeKnobIn (col, filterReleaseKnob, knobSize, labelHeight);
    }

    area.removeFromTop (12);

    // --- Ligne 3 : DRIVE, LO-FI, CHORUS, REVERB ---
    auto row3 = area.removeFromTop (sectionHeight);
    int col3W = row3.getWidth() / 4;

    {
        auto col = row3.removeFromLeft (col3W);
        driveLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 3;
        placeKnobIn (col.removeFromLeft (w), driveAmountKnob, knobSize, labelHeight);
        placeComboIn (col.removeFromLeft (w), driveModeCombo, labelHeight);
        placeKnobIn (col, driveToneKnob, knobSize, labelHeight);
    }
    {
        auto col = row3.removeFromLeft (col3W);
        lofiLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 3;
        placeKnobIn (col.removeFromLeft (w), bitDepthKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), sampleReduceKnob, knobSize, labelHeight);
        placeKnobIn (col, lofiMixKnob, knobSize, labelHeight);
    }
    {
        auto col = row3.removeFromLeft (col3W);
        chorusLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 3;
        placeKnobIn (col.removeFromLeft (w), chorusRateKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), chorusDepthKnob, knobSize, labelHeight);
        placeKnobIn (col, chorusMixKnob, knobSize, labelHeight);
    }
    {
        auto col = row3;
        reverbLabel.setBounds (col.removeFromTop (labelHeight));
        col.removeFromTop (4);
        int w = col.getWidth() / 3;
        placeKnobIn (col.removeFromLeft (w), reverbSizeKnob, knobSize, labelHeight);
        placeKnobIn (col.removeFromLeft (w), reverbDampingKnob, knobSize, labelHeight);
        placeKnobIn (col, reverbMixKnob, knobSize, labelHeight);
    }
}
