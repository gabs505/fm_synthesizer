/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GeneratorWt1AudioProcessorEditor  : public AudioProcessorEditor, private Timer, private Slider::Listener
{
public:
    GeneratorWt1AudioProcessorEditor (GeneratorWt1AudioProcessor&);
    ~GeneratorWt1AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GeneratorWt1AudioProcessor& processor;
	void timerCallback() override
	{
		keyboardComponent.grabKeyboardFocus();
		stopTimer();
	}
	Slider attackSlider;
	Slider decaySlider;
	Slider sustainSlider;
	Slider releaseSlider;

	Label attack;
	Label decay;
	Label sustain;
	Label release;

	void sliderValueChanged(Slider* slider) override;
	MidiKeyboardComponent keyboardComponent;

	Slider modulationIndex;
	Slider freqRatio;
	Slider mod2Freq;
	Slider mod2Amp;

	Label mod1Label;
	Label mod2Label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneratorWt1AudioProcessorEditor)
};
