#pragma once
#include <JuceHeader.h>
#include "Synth/PhonkSynthVoice.h"
#include "Synth/PhonkSynthSound.h"
#include "DSP/PhonkDrive.h"
#include "DSP/LofiCrusher.h"

namespace SynthParamIDs
{
    constexpr auto osc1Wave  = "osc1Wave";
    constexpr auto osc1Level = "osc1Level";
    constexpr auto osc2Wave  = "osc2Wave";
    constexpr auto osc2Level = "osc2Level";
    constexpr auto osc2Detune = "osc2Detune";
    constexpr auto subLevel   = "subLevel";
    constexpr auto noiseLevel = "noiseLevel";

    constexpr auto ampAttack  = "ampAttack";
    constexpr auto ampDecay   = "ampDecay";
    constexpr auto ampSustain = "ampSustain";
    constexpr auto ampRelease = "ampRelease";

    constexpr auto filterType     = "filterType";
    constexpr auto filterCutoff   = "filterCutoff";
    constexpr auto filterReso     = "filterReso";
    constexpr auto filterEnvAmt   = "filterEnvAmt";
    constexpr auto filterKeyTrack = "filterKeyTrack";

    constexpr auto filterAttack  = "filterAttack";
    constexpr auto filterDecay   = "filterDecay";
    constexpr auto filterSustain = "filterSustain";
    constexpr auto filterRelease = "filterRelease";

    constexpr auto voiceMode = "voiceMode"; // 0 = Poly, 1 = Mono
    constexpr auto glideTime = "glideTime";

    constexpr auto driveAmount = "driveAmount";
    constexpr auto driveMode   = "driveMode";
    constexpr auto driveTone   = "driveTone";

    constexpr auto bitDepth     = "bitDepth";
    constexpr auto sampleReduce = "sampleReduce";
    constexpr auto lofiMix      = "lofiMix";

    constexpr auto chorusRate  = "chorusRate";
    constexpr auto chorusDepth = "chorusDepth";
    constexpr auto chorusMix   = "chorusMix";

    constexpr auto reverbSize    = "reverbSize";
    constexpr auto reverbDamping = "reverbDamping";
    constexpr auto reverbMix     = "reverbMix";

    constexpr auto outputGain = "outputGain";
}

class PhonkSynthAudioProcessor : public juce::AudioProcessor
{
public:
    PhonkSynthAudioProcessor();
    ~PhonkSynthAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Phonk Synth"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void rebuildVoicesIfNeeded (bool mono);
    void updateVoiceParameters();
    juce::MidiBuffer buildMonoMidi (const juce::MidiBuffer& incoming);

    static constexpr int kMaxPolyVoices = 8;

    juce::Synthesiser synth;
    PhonkSynthVoice* monoVoice = nullptr; // valide seulement quand on est en mode Mono
    bool currentlyMono = false;

    // Pile des notes tenues, utilisée pour le legato en mode Mono
    std::vector<int> heldNotes;

    juce::dsp::ProcessSpec spec;

    // Chaîne d'effets maître (identique dans l'esprit à Phonk Engine)
    std::array<PhonkDrive, 2> drive;
    std::array<LofiCrusher, 2> lofiCrusher;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Reverb reverb;
    juce::Reverb::Parameters reverbParams;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhonkSynthAudioProcessor)
};
