#pragma once
#include <JuceHeader.h>
#include "PhonkSynthSound.h"

enum class WaveShape { Saw = 0, Square = 1, Sine = 2, Triangle = 3 };

// ============================================================================
// PhonkSynthVoice
// Une voix : OSC1 + OSC2 (détuné) + sub-oscillateur (sine, -1 octave) + bruit,
// filtre résonant modulable par enveloppe, glide/portamento pour les slides 808.
// ============================================================================
class PhonkSynthVoice : public juce::SynthesiserVoice
{
public:
    void prepare (const juce::dsp::ProcessSpec& spec);

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<PhonkSynthSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Appelé pour les notes legato en mode Mono : change la hauteur cible sans
    // redéclencher les enveloppes (c'est ce qui crée l'effet de "slide").
    void glideToNote (int midiNoteNumber);

    // --- Réglages (appliqués depuis le processor à chaque bloc) ------------
    void setOscParams (WaveShape osc1Wave, float osc1Level,
                        WaveShape osc2Wave, float osc2Level, float osc2DetuneCents,
                        float subLevel, float noiseLevel);

    void setFilterParams (int filterType, float cutoffHz, float resonance,
                           float envAmountHz, float keyTrackAmount);

    void setAmpEnvParams (const juce::ADSR::Parameters& params)    { ampEnv.setParameters (params); }
    void setFilterEnvParams (const juce::ADSR::Parameters& params) { filterEnv.setParameters (params); }

    void setGlideTimeMs (float ms) { glideTimeMs = juce::jmax (0.0f, ms); }

private:
    static float renderWave (WaveShape shape, float phase);

    double sampleRate { 44100.0 };

    // Oscillateurs
    double osc1Phase { 0.0 }, osc2Phase { 0.0 }, subPhase { 0.0 };
    WaveShape osc1Wave { WaveShape::Saw }, osc2Wave { WaveShape::Saw };
    float osc1Level { 0.8f }, osc2Level { 0.0f }, osc2DetuneCents { 0.0f };
    float subLevel { 0.3f }, noiseLevel { 0.0f };
    juce::Random random;

    // Fréquence avec glide (portamento)
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> freqSmoothed;
    float glideTimeMs { 0.0f };
    float targetFrequency { 440.0f };

    // Filtre + enveloppe de filtre
    juce::dsp::StateVariableTPTFilter<float> filter;
    int filterType { 0 };
    float baseCutoff { 4000.0f }, resonance { 0.7f }, filterEnvAmount { 0.0f }, keyTrack { 0.0f };
    juce::ADSR filterEnv;

    // Enveloppe d'amplitude
    juce::ADSR ampEnv;

    float velocityGain { 1.0f };
    int currentMidiNote { 60 };
};
