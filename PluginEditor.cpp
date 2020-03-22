/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeneratorWt1AudioProcessorEditor::GeneratorWt1AudioProcessorEditor (GeneratorWt1AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
	keyboardComponent(processor.keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
   
	addAndMakeVisible(keyboardComponent);
	setSize(600, 300);
	startTimer(400);



	attackSlider.setSliderStyle(Slider::LinearBarVertical);
	attackSlider.setRange(0.1, 1.0, 0.001);
	attackSlider.setValue(0.2);

	decaySlider.setSliderStyle(Slider::LinearBarVertical);
	decaySlider.setRange(0.0, 1.0, 0.001);
	decaySlider.setValue(0.0);

	sustainSlider.setSliderStyle(Slider::LinearBarVertical);
	sustainSlider.setRange(0.0, 1.0, 0.1);
	sustainSlider.setValue(0.0);

	releaseSlider.setSliderStyle(Slider::LinearBarVertical);
	releaseSlider.setRange(0.01, 10.0, 0.01);
	releaseSlider.setValue(0.3);

	//Labels
	addAndMakeVisible(attack);
	attack.setText("Attack", dontSendNotification);
	addAndMakeVisible(decay);
	decay.setText("Decay", dontSendNotification);
	addAndMakeVisible(sustain);
	sustain.setText("Sustain", dontSendNotification);
	addAndMakeVisible(release);
	release.setText("Release", dontSendNotification);

	//FM labels
	addAndMakeVisible(mod1Label);
	mod1Label.setText("Sine FM Modulation", dontSendNotification);
	addAndMakeVisible(mod2Label);
	mod2Label.setText("Saw FM modulation", dontSendNotification);
	

	attack.attachToComponent(&attackSlider, true);
	decay.attachToComponent(&decaySlider, true);
	sustain.attachToComponent(&sustainSlider, true);
	release.attachToComponent(&releaseSlider, true);

	

	addAndMakeVisible(&attackSlider);
	addAndMakeVisible(&decaySlider);
	addAndMakeVisible(&sustainSlider);
	addAndMakeVisible(&releaseSlider);

	attackSlider.addListener(this);
	decaySlider.addListener(this);
	sustainSlider.addListener(this);
	releaseSlider.addListener(this);

	//pots for FM synthesis
	modulationIndex.setSliderStyle(Slider::Rotary);
	modulationIndex.setRange(0.0, 10.0, 0.1);
	modulationIndex.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	modulationIndex.setPopupDisplayEnabled(true, false, this);
	modulationIndex.setTextValueSuffix("Modulation index");
	modulationIndex.setValue(1.0);
	addAndMakeVisible(&modulationIndex);
	modulationIndex.addListener(this);

	freqRatio.setSliderStyle(Slider::Rotary);
	freqRatio.setRange(1.0, 10.0, 0.5);
	freqRatio.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	freqRatio.setPopupDisplayEnabled(true, false, this);
	freqRatio.setTextValueSuffix("Frequency ratio");
	freqRatio.setValue(1.0);
	addAndMakeVisible(&freqRatio);
	freqRatio.addListener(this);

	mod2Amp.setSliderStyle(Slider::Rotary);
	mod2Amp.setRange(0, 10.0, 0.1);
	mod2Amp.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	mod2Amp.setPopupDisplayEnabled(true, false, this);
	mod2Amp.setTextValueSuffix("Saw wave amplitude");
	mod2Amp.setValue(0.0);
	addAndMakeVisible(&mod2Amp);
	mod2Amp.addListener(this);

	mod2Freq.setSliderStyle(Slider::Rotary);
	mod2Freq.setRange(5.0, 1000.0, 1.0);
	mod2Freq.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
	mod2Freq.setPopupDisplayEnabled(true, false, this);
	mod2Freq.setTextValueSuffix("Saw wave frequency");
	mod2Freq.setValue(500.0);
	addAndMakeVisible(&mod2Freq);
	mod2Freq.addListener(this);
}

GeneratorWt1AudioProcessorEditor::~GeneratorWt1AudioProcessorEditor()
{
}

//==============================================================================
void GeneratorWt1AudioProcessorEditor::paint (Graphics& g)
{
    
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (40.0f);
    
}

void GeneratorWt1AudioProcessorEditor::resized()
{
	int adsrX = 180;
	keyboardComponent.setBounds(10, 10, getWidth() - 100, 100);
	attackSlider.setBounds(adsrX, 170, 10,50 );
	decaySlider.setBounds(adsrX+80, 170, 10, 50);
	sustainSlider.setBounds(adsrX + 160, 170, 10, 50);
	releaseSlider.setBounds(adsrX + 240, 170, 10, 50);

	modulationIndex.setBounds(30, 160, 70, 50);
	freqRatio.setBounds(30, 220, 70, 50);
	mod2Freq.setBounds(500, 160, 70, 50);
	mod2Amp.setBounds(500, 220, 70, 50);

	mod1Label.setBounds(10, 110 , 70, 50);
	mod2Label.setBounds(480, 110, 70, 50);
    
}

void  GeneratorWt1AudioProcessorEditor::sliderValueChanged(Slider* slider) {
	if (slider == &modulationIndex || slider == &freqRatio || slider == &mod2Amp || slider== &mod2Freq) {
	
		processor.synthAudioSource.updateVoiceFM(modulationIndex.getValue(), freqRatio.getValue(),mod2Amp.getValue(),mod2Freq.getValue());
	}
	else {
		processor.synthAudioSource.updateVoice(attackSlider.getValue(), decaySlider.getValue(), sustainSlider.getValue(), releaseSlider.getValue());
	}
	
}
