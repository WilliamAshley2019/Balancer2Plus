#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainMonoAudioProcessor::GainMonoAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    smoothedGain.reset (44100.0, 0.02);
    apvts.addParameterListener (ParamIDs::bassFreq, this);
}

GainMonoAudioProcessor::~GainMonoAudioProcessor()
{
    apvts.removeParameterListener (ParamIDs::bassFreq, this);
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout GainMonoAudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParamIDs::gainDb, "Gain (dB)",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f, 1.0f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        ParamIDs::mode, "Monitor Mode",
        juce::StringArray { "Stereo", "Mono (Mid)", "Solo Side" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        ParamIDs::sumLaw, "Summing Law",
        juce::StringArray { "-6dB (Voltage)", "-3dB (Power)" }, 0));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        ParamIDs::bassMono, "Bass Focus (Mono Lows)", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        ParamIDs::bassFreq, "Bass Freq (Hz)",
        juce::NormalisableRange<float> (20.0f, 300.0f, 1.0f, 0.5f), 120.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        ParamIDs::bypass, "Bypass", false));

    return { params.begin(), params.end() };
}

//==============================================================================
void GainMonoAudioProcessor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == ParamIDs::bassFreq)
    {
        currentBassFreq = newValue;
        updateSideFilter();
    }
}

void GainMonoAudioProcessor::updateSideFilter()
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, currentBassFreq);
    *lpfSide.coefficients = *coeffs;
}

//==============================================================================
bool GainMonoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainIn  = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainIn.isDisabled() || mainOut.isDisabled()) return false;
    if (!(mainIn  == juce::AudioChannelSet::mono() || mainIn  == juce::AudioChannelSet::stereo())) return false;
    if (!(mainOut == juce::AudioChannelSet::mono() || mainOut == juce::AudioChannelSet::stereo())) return false;
    return true;
}

//==============================================================================
void GainMonoAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;
    smoothedGain.reset (sampleRate, 0.02f);
    currentBassFreq = apvts.getRawParameterValue (ParamIDs::bassFreq)->load();
    updateSideFilter();
    lpfSide.reset();
}

//==============================================================================
void GainMonoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused (midi);
    const int numCh   = buffer.getNumChannels();
    const int numSamp = buffer.getNumSamples();

    // Pure mono fallback: just apply gain
    if (numCh < 2)
    {
        auto* data = buffer.getWritePointer (0);
        const float targetLin = juce::Decibels::decibelsToGain (
            apvts.getRawParameterValue (ParamIDs::gainDb)->load());
        smoothedGain.setTargetValue (targetLin);
        for (int n = 0; n < numSamp; ++n)
            data[n] *= smoothedGain.getNextValue();
        return;
    }

    const bool isBypassed = apvts.getRawParameterValue (ParamIDs::bypass)->load() > 0.5f;
    if (isBypassed) return;

    const int  mode     = static_cast<int> (apvts.getRawParameterValue (ParamIDs::mode)->load());
    const int  sumLaw   = static_cast<int> (apvts.getRawParameterValue (ParamIDs::sumLaw)->load());
    const bool bassMono = apvts.getRawParameterValue (ParamIDs::bassMono)->load() > 0.5f;

    const float targetLin = juce::Decibels::decibelsToGain (
        apvts.getRawParameterValue (ParamIDs::gainDb)->load());
    smoothedGain.setTargetValue (targetLin);

    // Voltage (-6 dB) vs Power (-3 dB) summing constant
    const float monoScale = (sumLaw == 0) ? 0.5f : 0.70710678118f;

    auto* L = buffer.getWritePointer (0);
    auto* R = buffer.getWritePointer (1);

    for (int n = 0; n < numSamp; ++n)
    {
        float inL  = L[n];
        float inR  = R[n];
        float outL = inL;
        float outR = inR;

        // ── 1. Bass Focus (Elliptical EQ via M/S subtraction) ─────────────
        if (bassMono)
        {
            float mid  = (inL + inR) * 0.5f;
            float side = (inL - inR) * 0.5f;

            float lowSide  = lpfSide.processSample (side);
            float highSide = side - lowSide;   // Phase-coherent HP via subtraction

            outL = mid + highSide;
            outR = mid - highSide;
        }

        // ── 2. Monitor mode (Mid/Side matrix) ──────────────────────────────
        if (mode == 1)          // Mono (Mid)
        {
            float mid = (outL + outR) * monoScale;
            outL = outR = mid;
        }
        else if (mode == 2)     // Solo Side
        {
            float side = (outL - outR) * monoScale;
            outL = outR = side;
        }

        // ── 3. Gain staging ────────────────────────────────────────────────
        const float g = smoothedGain.getNextValue();
        L[n] = outL * g;
        R[n] = outR * g;
    }
}

//==============================================================================
void GainMonoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos (destData, true);
    apvts.state.writeToStream (mos);
}

void GainMonoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData (data, static_cast<size_t> (sizeInBytes));
    if (tree.isValid())
        apvts.replaceState (tree);
}

//==============================================================================
juce::AudioProcessorEditor* GainMonoAudioProcessor::createEditor()
{
    return new GainMonoAudioProcessorEditor (*this);
}
