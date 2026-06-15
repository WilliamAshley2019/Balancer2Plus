#pragma once
#include <JuceHeader.h>

//==============================================================================
// All parameter IDs in one place — processor and editor both include this header,
// so there is one source of truth.
//==============================================================================
namespace ParamIDs
{
    static constexpr auto gainDb   = "gainDb";
    static constexpr auto mode     = "mode";      // 0=Stereo, 1=Mono(Mid), 2=Solo Side
    static constexpr auto sumLaw   = "sumLaw";    // 0=-6dB, 1=-3dB
    static constexpr auto bassMono = "bassMono";  // Elliptical EQ on/off
    static constexpr auto bassFreq = "bassFreq";  // Crossover frequency (Hz)
    static constexpr auto bypass   = "bypass";
}

//==============================================================================
class GainMonoAudioProcessor : public juce::AudioProcessor,
                               private juce::AudioProcessorValueTreeState::Listener
{
public:
    GainMonoAudioProcessor();
    ~GainMonoAudioProcessor() override;

    //==========================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==========================================================================
    const juce::String getName() const override { return "GainMono Pro"; }
    bool acceptsMidi()  const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int  getNumPrograms()  override { return 1; }
    int  getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==========================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    //==========================================================================
    // Public so the editor can attach to it
    juce::AudioProcessorValueTreeState apvts;

    // APVTS listener callback
    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    //==========================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void updateSideFilter();

    juce::LinearSmoothedValue<float> smoothedGain;

    // Single second-order LPF on the Side channel for the elliptical EQ
    using Filter = juce::dsp::IIR::Filter<float>;
    Filter lpfSide;

    double currentSampleRate = 44100.0;
    float  currentBassFreq   = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainMonoAudioProcessor)
};
