#include "PhonkSynthVoice.h"

void PhonkSynthVoice::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    ampEnv.setSampleRate (sampleRate);
    filterEnv.setSampleRate (sampleRate);
    freqSmoothed.reset (sampleRate, 0.001);
}

float PhonkSynthVoice::renderWave (WaveShape shape, float phase)
{
    // phase attendue dans [0, 1)
    switch (shape)
    {
        case WaveShape::Saw:      return 2.0f * phase - 1.0f;
        case WaveShape::Square:   return phase < 0.5f ? 1.0f : -1.0f;
        case WaveShape::Sine:     return std::sin (phase * juce::MathConstants<float>::twoPi);
        case WaveShape::Triangle: return 4.0f * std::abs (phase - 0.5f) - 1.0f;
    }
    return 0.0f;
}

void PhonkSynthVoice::setOscParams (WaveShape o1w, float o1l, WaveShape o2w, float o2l, float o2det,
                                     float subL, float noiseL)
{
    osc1Wave = o1w; osc1Level = o1l;
    osc2Wave = o2w; osc2Level = o2l; osc2DetuneCents = o2det;
    subLevel = subL; noiseLevel = noiseL;
}

void PhonkSynthVoice::setFilterParams (int type, float cutoffHz, float reso, float envAmountHz, float keyTrackAmt)
{
    filterType = type;
    baseCutoff = cutoffHz;
    resonance = reso;
    filterEnvAmount = envAmountHz;
    keyTrack = keyTrackAmt;
}

void PhonkSynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    currentMidiNote = midiNoteNumber;
    velocityGain = velocity;
    targetFrequency = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

    // Nouvelle note "franche" : pas de glide, on part directement de la fréquence
    freqSmoothed.reset (sampleRate, 0.001);
    freqSmoothed.setCurrentAndTargetValue (targetFrequency);

    osc1Phase = osc2Phase = subPhase = 0.0;

    ampEnv.noteOn();
    filterEnv.noteOn();
}

void PhonkSynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampEnv.noteOff();
        filterEnv.noteOff();
    }
    else
    {
        clearCurrentNote();
        ampEnv.reset();
        filterEnv.reset();
    }
}

void PhonkSynthVoice::glideToNote (int midiNoteNumber)
{
    // Utilisé en mode Mono/legato : on change juste la cible de fréquence,
    // sans redéclencher les enveloppes -> effet de "slide" continu (typique 808).
    currentMidiNote = midiNoteNumber;
    targetFrequency = (float) juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

    double glideSeconds = juce::jmax (0.001f, glideTimeMs / 1000.0f);
    freqSmoothed.reset (sampleRate, glideSeconds);
    freqSmoothed.setTargetValue (targetFrequency);
}

void PhonkSynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! ampEnv.isActive() && ! isKeyDown())
    {
        // rien à faire, mais on laisse une chance à un ampEnv.noteOn tardif
    }

    for (int i = 0; i < numSamples; ++i)
    {
        // Applique le temps de glide courant à la rampe (peut être modifié en live)
        double glideSeconds = juce::jmax (0.001f, glideTimeMs / 1000.0f);
        if (freqSmoothed.getTargetValue() != targetFrequency)
            freqSmoothed.setTargetValue (targetFrequency);

        float freq = freqSmoothed.getNextValue();

        // --- Oscillateurs ---
        double osc1Inc = freq / sampleRate;
        double osc2Freq = freq * std::pow (2.0f, osc2DetuneCents / 1200.0f);
        double osc2Inc = osc2Freq / sampleRate;
        double subInc  = (freq * 0.5) / sampleRate;

        float osc1Sample = renderWave (osc1Wave, (float) osc1Phase) * osc1Level;
        float osc2Sample = renderWave (osc2Wave, (float) osc2Phase) * osc2Level;
        float subSample  = std::sin ((float) subPhase * juce::MathConstants<float>::twoPi) * subLevel;
        float noiseSample = (random.nextFloat() * 2.0f - 1.0f) * noiseLevel;

        osc1Phase += osc1Inc; if (osc1Phase >= 1.0) osc1Phase -= 1.0;
        osc2Phase += osc2Inc; if (osc2Phase >= 1.0) osc2Phase -= 1.0;
        subPhase  += subInc;  if (subPhase  >= 1.0) subPhase  -= 1.0;

        float mixed = (osc1Sample + osc2Sample + subSample + noiseSample) * 0.6f;

        // --- Filtre modulé par enveloppe + key tracking ---
        float filterEnvValue = filterEnv.getNextSample();
        float keyTrackOffset = (currentMidiNote - 60) * 20.0f * keyTrack;
        float modulatedCutoff = juce::jlimit (20.0f, 20000.0f,
                                    baseCutoff + filterEnvValue * filterEnvAmount + keyTrackOffset);

        filter.setType (filterType == 0 ? juce::dsp::StateVariableTPTFilterType::lowpass
                       : filterType == 1 ? juce::dsp::StateVariableTPTFilterType::highpass
                                          : juce::dsp::StateVariableTPTFilterType::bandpass);
        filter.setCutoffFrequency (modulatedCutoff);
        filter.setResonance (resonance);
        float filtered = filter.processSample (0, mixed);

        // --- Enveloppe d'amplitude ---
        float ampValue = ampEnv.getNextSample();
        float out = filtered * ampValue * velocityGain;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample + i, out);

        if (! ampEnv.isActive())
        {
            clearCurrentNote();
            break;
        }
    }
}
