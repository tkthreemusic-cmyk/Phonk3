#pragma once
#include <JuceHeader.h>

// ============================================================================
// LofiCrusher
// Réduction de bit-depth + réduction de sample-rate (sample & hold),
// pour la texture lo-fi/sale caractéristique des samples phonk pitchés.
// ============================================================================
class LofiCrusher
{
public:
    void prepare (double sr) { sampleRate = sr; reset(); }
    void reset() { heldValue = 0.0f; holdCounter = 0; }

    void setBitDepth (float bits)        { bitDepth = juce::jlimit (2.0f, 16.0f, bits); }
    void setSampleRateDivider (int div)  { divider = juce::jmax (1, div); }

    float processSample (float x)
    {
        // Réduction de sample-rate : on ne renouvelle la valeur retenue
        // que tous les "divider" échantillons (effet sample & hold).
        if (holdCounter <= 0)
        {
            heldValue = x;
            holdCounter = divider;
        }
        --holdCounter;

        // Réduction de bit-depth : quantification du signal retenu.
        const float levels = std::pow (2.0f, bitDepth);
        return std::round (heldValue * levels) / levels;
    }

private:
    double sampleRate { 44100.0 };
    float bitDepth { 12.0f };
    int divider { 1 };
    float heldValue { 0.0f };
    int holdCounter { 0 };
};
