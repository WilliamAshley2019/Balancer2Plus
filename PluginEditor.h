#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
// AlphaAudio Look & Feel
// Dark charcoal + amber-gold palette, consistent with AlphaAudio brand.
//==============================================================================
class AlphaLAF : public juce::LookAndFeel_V4
{
public:
    // ── Colours ────────────────────────────────────────────────────────────
    static constexpr uint32_t colBg        = 0xff1e1e24;   // Deep charcoal
    static constexpr uint32_t colPanel     = 0xff28282f;   // Slightly lighter panel
    static constexpr uint32_t colBorder    = 0xff3a3a44;   // Subtle panel border
    static constexpr uint32_t colAccent    = 0xffcf9b3a;   // Amber gold
    static constexpr uint32_t colAccentDim = 0xff8a6420;   // Dim amber
    static constexpr uint32_t colText      = 0xffe8e0cc;   // Warm off-white
    static constexpr uint32_t colTextDim   = 0xff7a7060;   // Dim label text
    static constexpr uint32_t colThumb     = 0xffe0c060;   // Slider thumb
    static constexpr uint32_t colTrack     = 0xff3d3d48;   // Slider track

    AlphaLAF()
    {
        // ComboBox colours
        setColour (juce::ComboBox::backgroundColourId,   juce::Colour (colPanel));
        setColour (juce::ComboBox::outlineColourId,      juce::Colour (colBorder));
        setColour (juce::ComboBox::textColourId,         juce::Colour (colText));
        setColour (juce::ComboBox::arrowColourId,        juce::Colour (colAccent));
        setColour (juce::PopupMenu::backgroundColourId,  juce::Colour (colPanel));
        setColour (juce::PopupMenu::textColourId,        juce::Colour (colText));
        setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (colAccent).withAlpha (0.4f));

        // ToggleButton colours
        setColour (juce::ToggleButton::textColourId,     juce::Colour (colText));
        setColour (juce::ToggleButton::tickColourId,     juce::Colour (colAccent));
        setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (colTextDim));

        // Slider colours
        setColour (juce::Slider::thumbColourId,          juce::Colour (colThumb));
        setColour (juce::Slider::trackColourId,          juce::Colour (colTrack));
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (colAccent));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (colTrack));
        setColour (juce::Slider::backgroundColourId,     juce::Colour (colTrack));
        setColour (juce::Slider::textBoxTextColourId,    juce::Colour (colText));
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (colBg));
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (colBorder));
        setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (colAccent).withAlpha (0.5f));

        // Label
        setColour (juce::Label::textColourId,            juce::Colour (colText));
    }

    //==========================================================================
    // Rotary knob (amber arc)
    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& /*slider*/) override
    {
        const float radius  = juce::jmin (width / 2.0f, height / 2.0f) - 4.0f;
        const float centreX = x + width  * 0.5f;
        const float centreY = y + height * 0.5f;

        // Track arc
        juce::Path track;
        track.addArc (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f,
                      rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colour (colTrack));
        g.strokePath (track, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));

        // Value arc
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        juce::Path value;
        value.addArc (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f,
                      rotaryStartAngle, angle, true);
        g.setColour (juce::Colour (colAccent));
        g.strokePath (value, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));

        // Knob body
        const float knobR = radius * 0.65f;
        g.setColour (juce::Colour (0xff2e2e38));
        g.fillEllipse (centreX - knobR, centreY - knobR, knobR * 2.0f, knobR * 2.0f);
        g.setColour (juce::Colour (colBorder));
        g.drawEllipse (centreX - knobR, centreY - knobR, knobR * 2.0f, knobR * 2.0f, 1.5f);

        // Pointer dot
        const float dotR  = 3.0f;
        const float dotDist = knobR * 0.62f;
        const float dotX  = centreX + dotDist * std::sin (angle);
        const float dotY  = centreY - dotDist * std::cos (angle);
        g.setColour (juce::Colour (colAccent));
        g.fillEllipse (dotX - dotR, dotY - dotR, dotR * 2.0f, dotR * 2.0f);
    }

    //==========================================================================
    // Vertical slider
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style != juce::Slider::LinearVertical && style != juce::Slider::LinearHorizontal)
        {
            LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
            return;
        }

        const bool isVertical = (style == juce::Slider::LinearVertical);

        const float trackW   = 4.0f;
        const float thumbW   = 14.0f;
        const float thumbH   = 22.0f;
        const float cx       = x + width  * 0.5f;
        const float cy       = y + height * 0.5f;

        if (isVertical)
        {
            // Track
            g.setColour (juce::Colour (colTrack));
            g.fillRoundedRectangle (cx - trackW * 0.5f, (float)y, trackW, (float)height, 2.0f);

            // Active portion (zero-centred)
            const float zeroPos = juce::jmap (0.5f, 0.0f, 1.0f, (float)(y + height), (float)y);
            const float top     = juce::jmin (sliderPos, zeroPos);
            const float bot     = juce::jmax (sliderPos, zeroPos);
            g.setColour (juce::Colour (colAccent));
            g.fillRoundedRectangle (cx - trackW * 0.5f, top, trackW, bot - top, 2.0f);

            // Thumb
            g.setColour (juce::Colour (colThumb));
            g.fillRoundedRectangle (cx - thumbW * 0.5f, sliderPos - thumbH * 0.5f, thumbW, thumbH, 4.0f);
            g.setColour (juce::Colour (colBorder));
            g.drawRoundedRectangle (cx - thumbW * 0.5f, sliderPos - thumbH * 0.5f, thumbW, thumbH, 4.0f, 1.0f);
        }
        else
        {
            // Horizontal variant
            g.setColour (juce::Colour (colTrack));
            g.fillRoundedRectangle ((float)x, cy - trackW * 0.5f, (float)width, trackW, 2.0f);
            g.setColour (juce::Colour (colAccent));
            g.fillRoundedRectangle ((float)x, cy - trackW * 0.5f, sliderPos - (float)x, trackW, 2.0f);
            g.setColour (juce::Colour (colThumb));
            g.fillRoundedRectangle (sliderPos - thumbH * 0.5f, cy - thumbW * 0.5f, thumbH, thumbW, 4.0f);
            g.setColour (juce::Colour (colBorder));
            g.drawRoundedRectangle (sliderPos - thumbH * 0.5f, cy - thumbW * 0.5f, thumbH, thumbW, 4.0f, 1.0f);
        }
    }

    //==========================================================================
    // Toggle button (LED-style dot)
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/) override
    {
        const float ledR   = 6.0f;
        const float ledX   = 8.0f;
        const float ledY   = button.getHeight() * 0.5f;
        const bool  isOn   = button.getToggleState();
        const bool  isRed  = (button.getName() == "bypass");

        juce::Colour ledCol = isOn
            ? (isRed ? juce::Colour (0xffee4444) : juce::Colour (colAccent))
            : juce::Colour (colTrack);

        if (shouldDrawButtonAsHighlighted)
            ledCol = ledCol.brighter (0.2f);

        // Glow
        if (isOn)
        {
            g.setColour (ledCol.withAlpha (0.25f));
            g.fillEllipse (ledX - ledR * 1.7f, ledY - ledR * 1.7f, ledR * 3.4f, ledR * 3.4f);
        }

        // LED dot
        g.setColour (ledCol);
        g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);
        g.setColour (juce::Colour (colBorder));
        g.drawEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f, 1.0f);

        // Label text
        g.setColour (isOn ? juce::Colour (colText) : juce::Colour (colTextDim));
        g.setFont (juce::FontOptions (12.0f));
        g.drawText (button.getButtonText(),
                    (int)(ledX + ledR + 6), 0,
                    button.getWidth() - (int)(ledX + ledR + 6), button.getHeight(),
                    juce::Justification::centredLeft, false);
    }
};

//==============================================================================
// Editor
//==============================================================================
class GainMonoAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit GainMonoAudioProcessorEditor (GainMonoAudioProcessor&);
    ~GainMonoAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized ()                override;
    void timerCallback ()          override;

private:
    //==========================================================================
    // Helper: draws a titled section panel
    void drawSectionPanel (juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);

    //==========================================================================
    GainMonoAudioProcessor& audioProcessor;

    AlphaLAF alphaLAF;   // Lifetime must exceed all controls

    // ── Controls ─────────────────────────────────────────────────────────────
    juce::Slider        gainSlider;
    juce::ComboBox      modeBox;
    juce::ComboBox      sumLawBox;
    juce::ToggleButton  bassMonoToggle  { "Bass Focus" };
    juce::Slider        bassFreqSlider;
    juce::ToggleButton  bypassToggle    { "Bypass" };

    // Labels (non-owning, just for decorative section headers painted manually)

    // ── APVTS Attachments ────────────────────────────────────────────────────
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using BoxAttachment    = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<BoxAttachment>    modeAttachment;
    std::unique_ptr<BoxAttachment>    sumLawAttachment;
    std::unique_ptr<ButtonAttachment> bassMonoAttachment;
    std::unique_ptr<SliderAttachment> bassFreqAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;

    // Pre-computed layout rects (set in resized(), used in paint())
    juce::Rectangle<int> gainSection;
    juce::Rectangle<int> modeSection;
    juce::Rectangle<int> bassSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainMonoAudioProcessorEditor)
};
