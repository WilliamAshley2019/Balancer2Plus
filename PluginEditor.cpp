#include "PluginEditor.h"

//==============================================================================
GainMonoAudioProcessorEditor::GainMonoAudioProcessorEditor (GainMonoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel (&alphaLAF);

    //==========================================================================
    // Gain slider  ─────────────────────────────────────────────────────────────
    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 18);
    gainSlider.setTextValueSuffix (" dB");
    addAndMakeVisible (gainSlider);
    gainAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts, ParamIDs::gainDb, gainSlider);

    //==========================================================================
    // Monitor Mode dropdown  ────────────────────────────────────────────────────
    modeBox.addItem ("Stereo",      1);
    modeBox.addItem ("Mono (Mid)",  2);
    modeBox.addItem ("Solo Side",   3);
    modeBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (modeBox);
    modeAttachment = std::make_unique<BoxAttachment> (
        audioProcessor.apvts, ParamIDs::mode, modeBox);

    //==========================================================================
    // Summing Law dropdown  ─────────────────────────────────────────────────────
    sumLawBox.addItem ("-6 dB  (Voltage)",  1);
    sumLawBox.addItem ("-3 dB  (Power)",    2);
    sumLawBox.setSelectedId (1, juce::dontSendNotification);
    addAndMakeVisible (sumLawBox);
    sumLawAttachment = std::make_unique<BoxAttachment> (
        audioProcessor.apvts, ParamIDs::sumLaw, sumLawBox);

    //==========================================================================
    // Bass Focus section  ───────────────────────────────────────────────────────
    bassMonoToggle.setButtonText ("Mono Bass");
    bassMonoToggle.setName ("bassMono");
    addAndMakeVisible (bassMonoToggle);
    bassMonoAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.apvts, ParamIDs::bassMono, bassMonoToggle);

    bassFreqSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    bassFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 18);
    bassFreqSlider.setTextValueSuffix (" Hz");
    addAndMakeVisible (bassFreqSlider);
    bassFreqAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.apvts, ParamIDs::bassFreq, bassFreqSlider);

    //==========================================================================
    // Bypass toggle  ────────────────────────────────────────────────────────────
    bypassToggle.setButtonText ("BYPASS");
    bypassToggle.setName ("bypass");    // Used by AlphaLAF to colour the LED red
    addAndMakeVisible (bypassToggle);
    bypassAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.apvts, ParamIDs::bypass, bypassToggle);

    //==========================================================================
    setSize (440, 380);
    startTimerHz (30);
}

GainMonoAudioProcessorEditor::~GainMonoAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void GainMonoAudioProcessorEditor::drawSectionPanel (juce::Graphics& g,
                                                      juce::Rectangle<int> bounds,
                                                      const juce::String& title)
{
    // Panel background
    g.setColour (juce::Colour (AlphaLAF::colPanel));
    g.fillRoundedRectangle (bounds.toFloat(), 6.0f);

    // Border
    g.setColour (juce::Colour (AlphaLAF::colBorder));
    g.drawRoundedRectangle (bounds.toFloat(), 6.0f, 1.0f);

    // Title bar strip
    auto titleBar = bounds.removeFromTop (22).toFloat();
    g.setColour (juce::Colour (AlphaLAF::colAccentDim).withAlpha (0.55f));
    g.fillRoundedRectangle (titleBar, 6.0f);
    // Square off the bottom corners of the title bar
    g.fillRect (titleBar.getX(), titleBar.getBottom() - 6.0f, titleBar.getWidth(), 6.0f);

    g.setColour (juce::Colour (AlphaLAF::colAccent));
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText (title, titleBar.toNearestInt(), juce::Justification::centred, false);
}

//==============================================================================
void GainMonoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Overall background
    g.fillAll (juce::Colour (AlphaLAF::colBg));

    // Subtle horizontal scanline texture
    g.setColour (juce::Colours::black.withAlpha (0.08f));
    for (int y = 0; y < getHeight(); y += 2)
        g.drawHorizontalLine (y, 0.0f, (float)getWidth());

    // Plugin title
    g.setColour (juce::Colour (AlphaLAF::colAccent));
    g.setFont (juce::FontOptions (17.0f, juce::Font::bold));
    g.drawText ("Balancer Plus", getLocalBounds().removeFromTop (36), juce::Justification::centred, false);

    // Sub-brand
    g.setColour (juce::Colour (AlphaLAF::colTextDim));
    g.setFont (juce::FontOptions (9.5f));
    g.drawText ("AlphaAudio - William Ashley Music",
                getLocalBounds().removeFromTop (50).removeFromBottom (14),
                juce::Justification::centred, false);

    // Section panels (bounds were computed in resized())
    drawSectionPanel (g, gainSection, "GAIN");
    drawSectionPanel (g, modeSection, "MONITOR");
    drawSectionPanel (g, bassSection, "BASS FOCUS");

    // Control labels beneath gain slider
    g.setColour (juce::Colour (AlphaLAF::colTextDim));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText ("Gain (dB)",
                gainSection.withTrimmedTop (22 + gainSlider.getHeight()),
                juce::Justification::centred, false);

    // Label above bass freq knob
    auto bassKnobLabel = bassFreqSlider.getBounds().withHeight (14).translated (0, -14);
    g.setColour (juce::Colour (AlphaLAF::colTextDim));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText ("Crossover", bassKnobLabel, juce::Justification::centred, false);
}

//==============================================================================
void GainMonoAudioProcessorEditor::resized()
{
    const int pad    = 10;
    const int margin = 8;

    // Top 52px reserved for title + sub-brand
    auto area = getLocalBounds().reduced (pad).withTrimmedTop (42);

    //──────────────────────────────────────────────────────────────────────────
    // Bypass toggle — top-right strip, 24 px tall
    auto topBar = area.removeFromTop (24);
    bypassToggle.setBounds (topBar.removeFromRight (100));
    area.removeFromTop (margin);

    //──────────────────────────────────────────────────────────────────────────
    // Three panels side by side
    const int gainW = 90;
    const int bassW = 148;
    const int modeW = area.getWidth() - gainW - bassW - margin * 2;

    gainSection = area.removeFromLeft (gainW);
    area.removeFromLeft (margin);
    modeSection = area.removeFromLeft (modeW);
    area.removeFromLeft (margin);
    bassSection = area;

    //── Gain slider inside gain panel ──────────────────────────────────────────
    {
        auto inner = gainSection.reduced (6).withTrimmedTop (22); // skip title bar
        gainSlider.setBounds (inner.withHeight (inner.getHeight() - 4));
    }

    //── Mode & Sum Law dropdowns inside mode panel ─────────────────────────────
    {
        auto inner = modeSection.reduced (8).withTrimmedTop (24);
        const int boxH = 24;
        const int gap  = 10;

        // "Monitor" label row — painted, not a component
        inner.removeFromTop (4);
        modeBox.setBounds (inner.removeFromTop (boxH));
        inner.removeFromTop (gap);

        // Small label for summing law — painted in paint()
        inner.removeFromTop (14);
        sumLawBox.setBounds (inner.removeFromTop (boxH));
    }

    //── Bass Focus section ──────────────────────────────────────────────────────
    {
        auto inner = bassSection.reduced (8).withTrimmedTop (24);

        bassMonoToggle.setBounds (inner.removeFromTop (22));
        inner.removeFromTop (6);

        // Knob: centred, leave 14px above for label (painted)
        inner.removeFromTop (14);
        const int knobSize = 72;
        auto knobArea = inner.removeFromTop (knobSize);
        bassFreqSlider.setBounds (knobArea.withWidth (knobSize)
                                           .withX (bassSection.getCentreX() - knobSize / 2));
    }
}

//==============================================================================
void GainMonoAudioProcessorEditor::timerCallback()
{
    // Keeps the bypass LED and any host-automation state visually fresh
    repaint();
}
