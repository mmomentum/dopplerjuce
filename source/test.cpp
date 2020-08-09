/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "test.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
test::test ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

test::~test()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void test::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colours::black);

    {
        float x = 196.0f, y = 80.0f, width = 112.0f, height = 112.0f;
        juce::Colour fillColour1 = juce::Colours::red, fillColour2 = juce::Colours::green;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                             176.0f - 196.0f + x,
                                             80.0f - 80.0f + y,
                                             fillColour2,
                                             176.0f - 196.0f + x,
                                             184.0f - 80.0f + y,
                                             false));
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void test::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="test" componentName="" parentClasses="public juce::Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ff000000">
    <ROUNDRECT pos="196 80 112 112" cornerSize="10.0" fill="linear: 176 80, 176 184, 0=ffff0000, 1=ff008000"
               hasStroke="0"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

