#pragma once
#include <JuceHeader.h>

// ============================================================================
// PhonkDrive
// Saturation/distorsion inspirée des textures "tape/lofi" typiques du phonk :
// - Tape   : saturation douce asymétrique (chaleur analogique)
// - Fuzz   : clipping dur avec repli (sale, agressif)
// - Crush  : waveshaping en escalier (numérique, sale)
// ============================================================================
class PhonkDrive
{
public:
    enum class Mode { Tape = 0, Fuzz = 1, Crush = 2 };

    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        toneFilter.prepare (spec);
        toneFilter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        reset();
    }

    void reset() { toneFilter.reset(); }

    void setMode (Mode m)        { mode = m; }
    void setDrive (float amount) { drive = juce::jlimit (0.0f, 1.0f, amount); }   // 0..1
    void setTone  (float hz)     { toneFilter.setCutoffFrequency (juce::jlimit (200.0f, 20000.0f, hz)); }

    float processSample (float x)
    {
        const float driveGain = 1.0f + drive * 24.0f;
        float y = x * driveGain;

        switch (mode)
        {
            case Mode::Tape:
            {
                // Saturation asymétrique douce (tanh + petit offset pour le grain analogique)
                y = std::tanh (y * 0.9f + 0.05f) - std::tanh (0.05f);
                break;
            }
            case Mode::Fuzz:
            {
                // Clipping dur avec un léger repli (foldback) pour un son plus sale
                y = juce::jlimit (-1.0f, 1.0f, y);
                if (std::abs (y) > 0.8f)
                    y = juce::jlimit (-1.0f, 1.0f, y - 0.3f * (y - 0.8f * (y > 0 ? 1.0f : -1.0f)));
                break;
            }
            case Mode::Crush:
            {
                // Waveshaping en escalier (quantification douce du signal)
                const float steps = 6.0f + (1.0f - drive) * 20.0f;
                y = std::round (std::tanh (y) * steps) / steps;
                break;
            }
        }

        // Compensation de niveau + passage dans le filtre de tonalité
        y *= 1.0f / (1.0f + drive * 1.5f);
        return toneFilter.processSample (0, y);
    }

private:
    Mode mode { Mode::Tape };
    float drive { 0.3f };
    juce::dsp::StateVariableTPTFilter<float> toneFilter;
};
