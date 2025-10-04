/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.2.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
namespace hise { using namespace juce;
//[/Headers]

#include "AudioLooperEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AudioLooperEditor::AudioLooperEditor (ProcessorEditor *p)
    : ProcessorEditorBody(p)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (sampleBufferContent = new MultiChannelAudioBufferDisplay ());
    sampleBufferContent->setName ("new component");
	sampleBufferContent->setAudioFile(dynamic_cast<AudioSampleProcessor*>(getProcessor())->getAudioFileUnchecked(0));

    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("LOOPER")));
    label->setFont (Font ("Arial", 24.00f, Font::plain).withTypefaceStyle ("Bold"));
    label->setJustificationType (Justification::centredRight);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour (0x52ffffff));
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (syncToHost = new HiComboBox ("Mode Selection"));
    syncToHost->setTooltip (TRANS("Sync the loop to the host tempo"));
    syncToHost->setEditableText (false);
    syncToHost->setJustificationType (Justification::centredLeft);
    syncToHost->setTextWhenNothingSelected (TRANS("Sync to Tempo"));
    syncToHost->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    syncToHost->addItem (TRANS("Free running"), 1);
    syncToHost->addItem (TRANS("1 Beat"), 2);
    syncToHost->addItem (TRANS("2 Beats"), 3);
    syncToHost->addItem (TRANS("1 Bar"), 4);
    syncToHost->addItem (TRANS("2 Bars"), 5);
    syncToHost->addItem (TRANS("4 Bars"), 6);
    syncToHost->addItem (TRANS("8 Bars"), 7);
    syncToHost->addItem (TRANS("12 Bars"), 8);
    syncToHost->addItem (TRANS("16 Bars"), 9);
    syncToHost->addSeparator();
    syncToHost->addListener (this);

    addAndMakeVisible (pitchButton = new HiToggleButton ("FM Synthesiser"));
    pitchButton->setTooltip (TRANS("Enables FM Modulation\n"));
    pitchButton->setButtonText (TRANS("Pitch Tracking"));
    pitchButton->addListener (this);
    pitchButton->setColour (ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (loopButton = new HiToggleButton ("FM Synthesiser"));
    loopButton->setTooltip (TRANS("Enables FM Modulation\n"));
    loopButton->setButtonText (TRANS("Loop"));
    loopButton->addListener (this);
    loopButton->setColour (ToggleButton::textColourId, Colours::white);

    addAndMakeVisible (rootNote = new HiSlider ("Root Note"));
    rootNote->setRange (0, 127, 1);
    rootNote->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    rootNote->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
    rootNote->addListener (this);

    addAndMakeVisible (startModSlider = new HiSlider ("StartMod"));
    startModSlider->setRange (0, 127, 1);
    startModSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    startModSlider->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
    startModSlider->addListener (this);

    addAndMakeVisible (reverseButton = new HiToggleButton ("FM Synthesiser"));
    reverseButton->setTooltip (TRANS("Reverse the playback"));
    reverseButton->setButtonText (TRANS("Reverse"));
    reverseButton->addListener (this);
    reverseButton->setColour (ToggleButton::textColourId, Colours::white);
    
    addAndMakeVisible (crossfadeSlider = new HiSlider ("Crossfade"));
    crossfadeSlider->setTooltip(TRANS("Loop crossfade length as percentage of loop"));
    crossfadeSlider->setRange(0, 100, 0.1);
    crossfadeSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    crossfadeSlider->setTextBoxStyle (Slider::TextBoxRight, false, 40, 20);
    crossfadeSlider->addListener (this);

    //[UserPreSize]

	AudioSampleProcessor *asp = dynamic_cast<AudioSampleProcessor*>(getProcessor());

	sampleBufferContent->setAudioFile(&asp->getBuffer());

	startModSlider->setup(getProcessor(), AudioLooper::SampleStartMod, "Random Start");
    startModSlider->setMode(HiSlider::Discrete, NormalisableRange(0.0, 20000.0, 1.0).withCentreSkew(1000.0));


	syncToHost->setup(getProcessor(), AudioLooper::SyncMode, "Sync to host");
	loopButton->setup(getProcessor(), AudioLooper::LoopEnabled, "Loop Enabled");
	pitchButton->setup(getProcessor(), AudioLooper::PitchTracking, "Pitch Tracking");
	rootNote->setup(getProcessor(), AudioLooper::RootNote, "Root Note");

	reverseButton->setup(getProcessor(), AudioLooper::Reversed, "Reversed");
    
    crossfadeSlider->setup(getProcessor(), AudioLooper::LoopCrossfade, "Crossfade");
    crossfadeSlider->setMode(HiSlider::Discrete, NormalisableRange<double>(0.0, 100.0, 0.1).withCentreSkew(25.0));
    
#if JUCE_DEBUG
	startTimer(30);
#else
	startTimer(30);
#endif

    //[/UserPreSize]

    setSize (830, 250);


    //[Constructor] You can add your own custom stuff here..

	h = getHeight();
    //[/Constructor]
}

AudioLooperEditor::~AudioLooperEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampleBufferContent = nullptr;
    label = nullptr;
    syncToHost = nullptr;
    pitchButton = nullptr;
    loopButton = nullptr;
    rootNote = nullptr;
    startModSlider = nullptr;
    reverseButton = nullptr;
    crossfadeSlider = nullptr;

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioLooperEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = static_cast<float> ((getWidth() / 2) - ((getWidth() - 84) / 2)), y = 6.0f, width = static_cast<float> (getWidth() - 84), height = static_cast<float> (getHeight() - 16);
        Colour fillColour = Colour (0x30000000);
        Colour strokeColour = Colour (0x25ffffff);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 6.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 6.000f, 2.000f);
    }
    if (getProcessor()->getAttribute(AudioLooper::LoopCrossfade) > 0.0f)
    {
        paintOverChildren(g);
    }
    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioLooperEditor::paintOverChildren(Graphics& g)
{
    auto crossfadeValue = getProcessor()->getAttribute(AudioLooper::LoopCrossfade);
    float crossfadePercentage = (crossfadeValue / 100.0f) * 0.5f;  // Convert to 0-0.5
    if (crossfadePercentage <= 0.0f) return;
    
    auto audioLooper = dynamic_cast<AudioLooper*>(getProcessor());
    if (!audioLooper) return;
    
    auto waveformBounds = sampleBufferContent->getBounds();
    
    // Get the loop boundaries
    auto sampleRange = audioLooper->getBuffer().getCurrentRange();
    auto loopRange = audioLooper->getBuffer().getLoopRange();
    
    int offset = sampleRange.getStart();
    int loopStart = jmax<int>(offset, loopRange.getStart());
    int loopEnd = jmin<int>(loopRange.getEnd(), sampleRange.getEnd());
    
    int actualLoopLength = loopEnd - loopStart;
    if (actualLoopLength <= 0) return;
    
    // Apply 50% limit for visualization (same as audio processing)
    int crossfadeLength = (int)(actualLoopLength * crossfadePercentage);
    
    // Use TOTAL range for display calculations
    auto totalRange = audioLooper->getBuffer().getTotalRange();
    int displayLength = totalRange.getLength();
    if (displayLength <= 0) return;
    
    // Map sample positions to pixel positions
    float pixelsPerSample = (float)waveformBounds.getWidth() / (float)displayLength;
    
    int loopStartPixel = waveformBounds.getX() + (int)(loopStart * pixelsPerSample);
    int loopEndPixel = waveformBounds.getX() + (int)(loopEnd * pixelsPerSample);
    int crossfadePixels = (int)(crossfadeLength * pixelsPerSample);
    
    // Calculate crossfade region boundaries
    int beginStartX = loopStartPixel;
    int beginEndX = beginStartX + crossfadePixels;
    int tailStartX = loopEndPixel - crossfadePixels;
    int tailEndX = loopEndPixel;
    
    // Ensure crossfade regions don't overlap
    if (beginEndX > tailStartX)
    {
        int midPoint = (beginStartX + loopEndPixel) / 2;
        beginEndX = midPoint;
        tailStartX = midPoint;
    }
    
    // Colors
    Colour activeColour = Colour(0x66FFFFFF);
    Colour fadeoutColour = Colour(0x30000000);
    
    float waveTop = (float)waveformBounds.getY();
    float waveBottom = (float)waveformBounds.getBottom();
    float waveHeight = (float)waveformBounds.getHeight();
    
    // ========== FADE IN PATH (at loop start) ==========
    if (beginStartX < beginEndX && crossfadePixels > 0)
    {
        Path fadeInDark, fadeInLight;
        
        // Create the dark (top) region path
        fadeInDark.startNewSubPath((float)beginStartX, waveTop);
        
        for (int x = beginStartX; x <= beginEndX; ++x)
        {
            float ratio = (float)(x - beginStartX) / (float)crossfadePixels;
            float fadeIn = sinf(ratio * M_PI * 0.5f);
            float y = waveTop + (waveHeight * (1.0f - fadeIn));
            fadeInDark.lineTo((float)x, y);
        }
        
        fadeInDark.lineTo((float)beginEndX, waveTop);
        fadeInDark.closeSubPath();
        
        // Create the light (bottom) region path
        fadeInLight.startNewSubPath((float)beginStartX, waveBottom);
        
        for (int x = beginStartX; x <= beginEndX; ++x)
        {
            float ratio = (float)(x - beginStartX) / (float)crossfadePixels;
            float fadeIn = sinf(ratio * M_PI * 0.5f);
            float y = waveTop + (waveHeight * (1.0f - fadeIn));
            fadeInLight.lineTo((float)x, y);
        }
        
        fadeInLight.lineTo((float)beginEndX, waveBottom);
        fadeInLight.closeSubPath();
        
        // Draw the paths
        g.setColour(fadeoutColour);
        g.fillPath(fadeInDark);
        
        g.setColour(activeColour);
        g.fillPath(fadeInLight);
    }
    
    // ========== FADE OUT PATH (at loop end) ==========
    if (tailStartX < tailEndX && crossfadePixels > 0)
    {
        Path fadeOutDark, fadeOutLight;
        
        // Create the dark (top) region path
        fadeOutDark.startNewSubPath((float)tailStartX, waveTop);
        
        for (int x = tailStartX; x <= tailEndX; ++x)
        {
            float ratio = (float)(x - tailStartX) / (float)crossfadePixels;
            float fadeOut = cosf(ratio * M_PI * 0.5f);
            float y = waveTop + (waveHeight * (1.0f - fadeOut));
            fadeOutDark.lineTo((float)x, y);
        }
        
        fadeOutDark.lineTo((float)tailEndX, waveTop);
        fadeOutDark.closeSubPath();
        
        // Create the light (bottom) region path
        fadeOutLight.startNewSubPath((float)tailStartX, waveBottom);
        
        for (int x = tailStartX; x <= tailEndX; ++x)
        {
            float ratio = (float)(x - tailStartX) / (float)crossfadePixels;
            float fadeOut = cosf(ratio * M_PI * 0.5f);
            float y = waveTop + (waveHeight * (1.0f - fadeOut));
            fadeOutLight.lineTo((float)x, y);
        }
        
        fadeOutLight.lineTo((float)tailEndX, waveBottom);
        fadeOutLight.closeSubPath();
        
        // Draw the paths
        g.setColour(fadeoutColour);
        g.fillPath(fadeOutDark);
        
        g.setColour(activeColour);
        g.fillPath(fadeOutLight);
    }
    
    // ========== BOUNDARY INDICATORS ==========
    Colour indicatorColour = Colour(0xFFEEEEEE);
    
    // Loop boundary lines
    g.setColour(indicatorColour.withAlpha(0.8f));
    if (loopStartPixel >= waveformBounds.getX() && loopStartPixel < waveformBounds.getRight())
        g.drawVerticalLine(loopStartPixel, waveTop, waveBottom);
    if (loopEndPixel > waveformBounds.getX() && loopEndPixel <= waveformBounds.getRight())
        g.drawVerticalLine(loopEndPixel, waveTop, waveBottom);
    
    // Crossfade boundary lines (more subtle)
    g.setColour(indicatorColour.withAlpha(0.5f));
    if (beginEndX > waveformBounds.getX() && beginEndX < waveformBounds.getRight())
        g.drawVerticalLine(beginEndX, waveTop, waveBottom);
    if (tailStartX > waveformBounds.getX() && tailStartX < waveformBounds.getRight())
        g.drawVerticalLine(tailStartX, waveTop, waveBottom);
}

void AudioLooperEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    sampleBufferContent->setBounds ((getWidth() / 2) - ((getWidth() - 112) / 2), 77, getWidth() - 112, 144);
    label->setBounds (getWidth() - 52 - 264, 12, 264, 40);
    syncToHost->setBounds (56, 45, 134, 28);
    pitchButton->setBounds (354 - 128, 12, 128, 32);
    loopButton->setBounds (184 - 128, 11, 128, 32);
    rootNote->setBounds (358, 20, 128, 48);
    startModSlider->setBounds (515, 20, 128, 48);
    reverseButton->setBounds (354 - 128, 45, 128, 32);
    crossfadeSlider->setBounds (672, 20, 128, 48);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioLooperEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == syncToHost)
    {
        //[UserComboBoxCode_syncToHost] -- add your combo box handling code here..
        //[/UserComboBoxCode_syncToHost]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void AudioLooperEditor::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == pitchButton)
    {
        //[UserButtonCode_pitchButton] -- add your button handler code here..
		rootNote->setEnabled(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_pitchButton]
    }
    else if (buttonThatWasClicked == loopButton)
    {
        //[UserButtonCode_loopButton] -- add your button handler code here..
        //[/UserButtonCode_loopButton]
    }
    else if (buttonThatWasClicked == reverseButton)
    {
        //[UserButtonCode_reverseButton] -- add your button handler code here..
        //[/UserButtonCode_reverseButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void AudioLooperEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == rootNote)
    {
        //[UserSliderCode_rootNote] -- add your slider handling code here..
        //[/UserSliderCode_rootNote]
    }
    else if (sliderThatWasMoved == crossfadeSlider)
    {
        repaint(); // Force redraw when crossfade changes
    }


    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioLooperEditor" componentName=""
                 parentClasses="public ProcessorEditorBody, public Timer, public AudioDisplayComponent::Listener"
                 constructorParams="ProcessorEditor *p" variableInitialisers="ProcessorEditorBody(p)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="830" initialHeight="250">
  <BACKGROUND backgroundColour="ffffff">
    <ROUNDRECT pos="0Cc 6 84M 16M" cornerSize="6" fill="solid: 30000000" hasStroke="1"
               stroke="2, mitered, butt" strokeColour="solid: 25ffffff"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="new component" id="e2252e55bedecdc5" memberName="sampleBufferContent"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 77 112M 144" class="AudioSampleBufferComponent"
                    params="getProcessor()"/>
  <LABEL name="new label" id="bd1d8d6ad6d04bdc" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="52Rr 12 264 40" textCol="52ffffff"
         edTextCol="ff000000" edBkgCol="0" labelText="LOOPER" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Arial"
         fontsize="24" kerning="0" bold="1" italic="0" justification="34"
         typefaceStyle="Bold"/>
  <COMBOBOX name="Mode Selection" id="63f5b1527f75c45b" memberName="syncToHost"
            virtualName="HiComboBox" explicitFocusOrder="0" pos="56 45 134 28"
            tooltip="Sync the loop to the host tempo" editable="0" layout="33"
            items="Free running&#10;1 Beat&#10;2 Beats&#10;1 Bar&#10;2 Bars&#10;4 Bars&#10;8 Bars&#10;12 Bars&#10;16 Bars&#10;"
            textWhenNonSelected="Sync to Tempo" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="FM Synthesiser" id="e77edc03c117de85" memberName="pitchButton"
                virtualName="HiToggleButton" explicitFocusOrder="0" pos="354r 12 128 32"
                tooltip="Enables FM Modulation&#10;" txtcol="ffffffff" buttonText="Pitch Tracking"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="FM Synthesiser" id="3ef6a10c1e23368a" memberName="loopButton"
                virtualName="HiToggleButton" explicitFocusOrder="0" pos="184r 11 128 32"
                tooltip="Enables FM Modulation&#10;" txtcol="ffffffff" buttonText="Loop"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="Root Note" id="baa9524f7348f05" memberName="rootNote" virtualName="HiSlider"
          explicitFocusOrder="0" pos="358 20 128 48" min="0" max="127"
          int="1" style="RotaryHorizontalVerticalDrag" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="40" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <SLIDER name="StartMod" id="abb968f0edba4d8b" memberName="startModSlider"
          virtualName="HiSlider" explicitFocusOrder="0" pos="515 20 128 48"
          min="0" max="127" int="1" style="RotaryHorizontalVerticalDrag"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="40"
          textBoxHeight="20" skewFactor="1" needsCallback="1"/>
  <TOGGLEBUTTON name="FM Synthesiser" id="fa26ab9f9a450d63" memberName="reverseButton"
                virtualName="HiToggleButton" explicitFocusOrder="0" pos="354r 45 128 32"
                tooltip="Reverse the playback" txtcol="ffffffff" buttonText="Reverse"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
} // namespace hise
//[/EndFile]
