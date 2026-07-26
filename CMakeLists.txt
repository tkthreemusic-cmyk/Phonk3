cmake_minimum_required(VERSION 3.22)

project(PhonkSynth VERSION 1.0.2)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(PhonkSynth
    COMPANY_NAME "TonStudio"
    PLUGIN_MANUFACTURER_CODE Tsto
    PLUGIN_CODE Psyn
    FORMATS VST3 Standalone
    PRODUCT_NAME "Phonk Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES "Instrument" "Synth"
)

juce_generate_juce_header(PhonkSynth)

target_sources(PhonkSynth PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Synth/PhonkSynthVoice.cpp
)

target_compile_definitions(PhonkSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(PhonkSynth PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_core
)
