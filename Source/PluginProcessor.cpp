#include "PluginProcessor.h"
#include "PluginEditor.h"

PhonkSynthAudioProcessor::PhonkSynthAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    synth.addSound (new PhonkSynthSound());
}

PhonkSynthAudioProcessor::~PhonkSynthAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout PhonkSynthAudioProcessor::createParameterLayout()
{
    using namespace SynthParamIDs;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto pctRange   = juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f);
    auto envMsRange = juce::NormalisableRange<float> (1.0f, 4000.0f, 1.0f, 0.3f);
    juce::StringArray waveNames { "Saw", "Square", "Sine", "Triangle" };

    // --- Oscillateurs ---
    params.push_back (std::make_unique<juce::AudioParameterChoice> (osc1Wave, "Osc1 Wave", waveNames, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>  (osc1Level, "Osc1 Level", pctRange, 80.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (osc2Wave, "Osc2 Wave", waveNames, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>  (osc2Level, "Osc2 Level", pctRange, 0.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>  (osc2Detune, "Osc2 Detune",
                        juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f), 7.0f, "ct"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>  (subLevel, "Sub Level", pctRange, 50.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat>  (noiseLevel, "Noise Level", pctRange, 0.0f, "%"));

    // --- Enveloppe d'amplitude ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (ampAttack,  "Amp Attack",  envMsRange, 5.0f, "ms"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (ampDecay,   "Amp Decay",   envMsRange, 200.0f, "ms"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (ampSustain, "Amp Sustain", pctRange, 80.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (ampRelease, "Amp Release", envMsRange, 300.0f, "ms"));

    // --- Filtre ---
    params.push_back (std::make_unique<juce::AudioParameterChoice> (filterType, "Filter Type",
                        juce::StringArray { "Low Pass", "High Pass", "Band Pass" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterCutoff, "Filter Cutoff",
                        juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.3f), 6000.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterReso, "Filter Resonance",
                        juce::NormalisableRange<float> (0.1f, 10.0f, 0.01f), 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterEnvAmt, "Filter Env Amount",
                        juce::NormalisableRange<float> (-10000.0f, 10000.0f, 1.0f), 3000.0f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterKeyTrack, "Filter KeyTrack", pctRange, 20.0f, "%"));

    // --- Enveloppe de filtre ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterAttack,  "Filter Attack",  envMsRange, 5.0f, "ms"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterDecay,   "Filter Decay",   envMsRange, 300.0f, "ms"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterSustain, "Filter Sustain", pctRange, 30.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (filterRelease, "Filter Release", envMsRange, 300.0f, "ms"));

    // --- Voix / Glide ---
    params.push_back (std::make_unique<juce::AudioParameterChoice> (voiceMode, "Voice Mode",
                        juce::StringArray { "Poly", "Mono" }, 1));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (glideTime, "Glide Time",
                        juce::NormalisableRange<float> (0.0f, 1000.0f, 1.0f, 0.4f), 90.0f, "ms"));

    // --- Drive ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (driveAmount, "Drive Amount", pctRange, 25.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (driveMode, "Drive Mode",
                        juce::StringArray { "Tape", "Fuzz", "Crush" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (driveTone, "Drive Tone",
                        juce::NormalisableRange<float> (200.0f, 18000.0f, 1.0f, 0.3f), 9000.0f, "Hz"));

    // --- Lo-Fi ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (bitDepth, "Bit Depth",
                        juce::NormalisableRange<float> (2.0f, 16.0f, 0.1f), 16.0f, "bit"));
    params.push_back (std::make_unique<juce::AudioParameterInt>   (sampleReduce, "Sample Reduce", 1, 40, 1));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (lofiMix, "Lo-Fi Mix", pctRange, 0.0f, "%"));

    // --- Chorus ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (chorusRate, "Chorus Rate",
                        juce::NormalisableRange<float> (0.05f, 5.0f, 0.01f), 0.4f, "Hz"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (chorusDepth, "Chorus Depth", pctRange, 20.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (chorusMix, "Chorus Mix", pctRange, 0.0f, "%"));

    // --- Reverb ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (reverbSize, "Reverb Size", pctRange, 50.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (reverbDamping, "Reverb Damping", pctRange, 50.0f, "%"));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (reverbMix, "Reverb Mix", pctRange, 15.0f, "%"));

    // --- Sortie ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (outputGain, "Output Gain",
                        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f, "dB"));

    return { params.begin(), params.end() };
}

void PhonkSynthAudioProcessor::rebuildVoicesIfNeeded (bool mono)
{
    if (mono == currentlyMono && synth.getNumVoices() > 0)
        return;

    synth.clearVoices();
    const int numVoices = mono ? 1 : kMaxPolyVoices;
    for (int i = 0; i < numVoices; ++i)
    {
        auto* voice = new PhonkSynthVoice();
        voice->prepare (spec);
        synth.addVoice (voice);
    }
    monoVoice = mono ? dynamic_cast<PhonkSynthVoice*> (synth.getVoice (0)) : nullptr;
    currentlyMono = mono;
    heldNotes.clear();
}

void PhonkSynthAudioProcessor::updateVoiceParameters()
{
    using namespace SynthParamIDs;

    auto wave = [] (int idx) { return (WaveShape) idx; };

    auto o1w = wave ((int) apvts.getRawParameterValue (osc1Wave)->load());
    auto o1l = apvts.getRawParameterValue (osc1Level)->load() / 100.0f;
    auto o2w = wave ((int) apvts.getRawParameterValue (osc2Wave)->load());
    auto o2l = apvts.getRawParameterValue (osc2Level)->load() / 100.0f;
    auto o2d = apvts.getRawParameterValue (osc2Detune)->load();
    auto subL = apvts.getRawParameterValue (subLevel)->load() / 100.0f;
    auto noiseL = apvts.getRawParameterValue (noiseLevel)->load() / 100.0f;

    juce::ADSR::Parameters ampParams;
    ampParams.attack  = apvts.getRawParameterValue (ampAttack)->load() / 1000.0f;
    ampParams.decay   = apvts.getRawParameterValue (ampDecay)->load() / 1000.0f;
    ampParams.sustain = apvts.getRawParameterValue (ampSustain)->load() / 100.0f;
    ampParams.release = apvts.getRawParameterValue (ampRelease)->load() / 1000.0f;

    juce::ADSR::Parameters filtParams;
    filtParams.attack  = apvts.getRawParameterValue (filterAttack)->load() / 1000.0f;
    filtParams.decay   = apvts.getRawParameterValue (filterDecay)->load() / 1000.0f;
    filtParams.sustain = apvts.getRawParameterValue (filterSustain)->load() / 100.0f;
    filtParams.release = apvts.getRawParameterValue (filterRelease)->load() / 1000.0f;

    int fType = (int) apvts.getRawParameterValue (filterType)->load();
    float cutoff = apvts.getRawParameterValue (filterCutoff)->load();
    float reso = apvts.getRawParameterValue (filterReso)->load();
    float envAmt = apvts.getRawParameterValue (filterEnvAmt)->load();
    float keyTrk = apvts.getRawParameterValue (filterKeyTrack)->load() / 100.0f;

    float glideMs = apvts.getRawParameterValue (glideTime)->load();

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<PhonkSynthVoice*> (synth.getVoice (i)))
        {
            v->setOscParams (o1w, o1l, o2w, o2l, o2d, subL, noiseL);
            v->setFilterParams (fType, cutoff, reso, envAmt, keyTrk);
            v->setAmpEnvParams (ampParams);
            v->setFilterEnvParams (filtParams);
            v->setGlideTimeMs (glideMs);
        }
    }
}

void PhonkSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

    synth.setCurrentPlaybackSampleRate (sampleRate);
    currentlyMono = ! ((int) apvts.getRawParameterValue (SynthParamIDs::voiceMode)->load() == 0);
    // Force la reconstruction des voix au premier bloc
    synth.clearVoices();

    for (auto& d : drive)         d.prepare (spec);
    for (auto& c : lofiCrusher)   c.prepare (sampleRate);
    chorus.prepare (spec);
    reverb.prepare (spec);
}

void PhonkSynthAudioProcessor::releaseResources() {}

bool PhonkSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

juce::MidiBuffer PhonkSynthAudioProcessor::buildMonoMidi (const juce::MidiBuffer& incoming)
{
    juce::MidiBuffer out;

    for (const auto metadata : incoming)
    {
        auto msg = metadata.getMessage();
        int samplePos = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            int note = msg.getNoteNumber();
            bool wasEmpty = heldNotes.empty();

            // Évite les doublons si la même note est déjà tenue
            heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
            heldNotes.push_back (note);

            if (wasEmpty)
            {
                // Première note de la phrase : vraie attaque (enveloppes redéclenchées)
                out.addEvent (msg, samplePos);
            }
            else if (monoVoice != nullptr)
            {
                // Note legato : on glisse simplement vers la nouvelle hauteur
                monoVoice->glideToNote (note);
            }
        }
        else if (msg.isNoteOff())
        {
            int note = msg.getNoteNumber();
            heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());

            if (heldNotes.empty())
            {
                // Plus aucune note tenue : vrai relâchement (enveloppes en release)
                out.addEvent (msg, samplePos);
            }
            else if (monoVoice != nullptr)
            {
                // On retombe sur la note précédente encore tenue (glide retour)
                monoVoice->glideToNote (heldNotes.back());
            }
        }
        else
        {
            out.addEvent (msg, samplePos);
        }
    }

    return out;
}

void PhonkSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const bool mono = ((int) apvts.getRawParameterValue (SynthParamIDs::voiceMode)->load()) == 1;
    rebuildVoicesIfNeeded (mono);
    updateVoiceParameters();

    if (mono)
    {
        auto monoMidi = buildMonoMidi (midiMessages);
        synth.renderNextBlock (buffer, monoMidi, 0, buffer.getNumSamples());
    }
    else
    {
        synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
    }

    // --- Chaîne d'effets maître -------------------------------------------
    using namespace SynthParamIDs;
    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin (buffer.getNumChannels(), 2);

    const float driveAmt   = apvts.getRawParameterValue (driveAmount)->load() / 100.0f;
    const int   driveModeI = (int) apvts.getRawParameterValue (driveMode)->load();
    const float driveToneV = apvts.getRawParameterValue (driveTone)->load();
    const float bits       = apvts.getRawParameterValue (bitDepth)->load();
    const int   sReduce    = (int) apvts.getRawParameterValue (sampleReduce)->load();
    const float lofiMixV   = apvts.getRawParameterValue (lofiMix)->load() / 100.0f;
    const float chRate     = apvts.getRawParameterValue (chorusRate)->load();
    const float chDepth    = apvts.getRawParameterValue (chorusDepth)->load() / 100.0f;
    const float chMix      = apvts.getRawParameterValue (chorusMix)->load() / 100.0f;
    const float rvSize     = apvts.getRawParameterValue (reverbSize)->load() / 100.0f;
    const float rvDamping  = apvts.getRawParameterValue (reverbDamping)->load() / 100.0f;
    const float rvMix      = apvts.getRawParameterValue (reverbMix)->load() / 100.0f;
    const float outGainDb  = apvts.getRawParameterValue (outputGain)->load();

    for (auto& d : drive) { d.setMode ((PhonkDrive::Mode) driveModeI); d.setDrive (driveAmt); d.setTone (driveToneV); }
    for (auto& c : lofiCrusher) { c.setBitDepth (bits); c.setSampleRateDivider (sReduce); }

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* s = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float x = drive[(size_t) ch].processSample (s[i]);
            float crushed = lofiCrusher[(size_t) ch].processSample (x);
            s[i] = x * (1.0f - lofiMixV) + crushed * lofiMixV;
        }
    }

    if (chMix > 0.0f)
    {
        chorus.setRate (chRate);
        chorus.setDepth (chDepth);
        chorus.setMix (1.0f);
        chorus.setCentreDelay (7.0f);
        chorus.setFeedback (0.2f);

        juce::AudioBuffer<float> chorusBuffer;
        chorusBuffer.makeCopyOf (buffer);
        juce::dsp::AudioBlock<float> block (chorusBuffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        chorus.process (ctx);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* dst = buffer.getWritePointer (ch);
            auto* wet = chorusBuffer.getReadPointer (ch);
            for (int i = 0; i < numSamples; ++i)
                dst[i] = dst[i] * (1.0f - chMix) + wet[i] * chMix;
        }
    }

    if (rvMix > 0.0f)
    {
        reverbParams.roomSize = rvSize;
        reverbParams.damping  = rvDamping;
        reverbParams.wetLevel = 1.0f;
        reverbParams.dryLevel = 0.0f;
        reverbParams.width    = 1.0f;
        reverb.setParameters (reverbParams);

        // Create a buffer for reverb processing
        juce::AudioBuffer<float> reverbBuffer (numChannels, numSamples);
        reverbBuffer.copyFrom (0, 0, buffer, 0, numSamples);

        // Process reverb using JUCE 7 DSP pattern
        auto reverbBlock = juce::dsp::AudioBlock<float> (reverbBuffer);
        auto reverbContext = juce::dsp::ProcessContextReplacing<float> (reverbBlock);
        reverb.process (reverbContext);

        // Mix dry and wet signals
        for (int ch = 0; ch < numChannels; ++ch)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                float dry = buffer.getSample (ch, i);
                float wet = reverbBuffer.getSample (ch, i);
                buffer.setSample (ch, i, dry * (1.0f - rvMix) + wet * rvMix);
            }
        }
    }

    const float outGain = juce::Decibels::decibelsToGain (outGainDb);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* s = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
            s[i] = juce::jlimit (-1.0f, 1.0f, s[i] * outGain);
    }
}

juce::AudioProcessorEditor* PhonkSynthAudioProcessor::createEditor()
{
    return new PhonkSynthAudioProcessorEditor (*this);
}

void PhonkSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PhonkSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhonkSynthAudioProcessor();
}
