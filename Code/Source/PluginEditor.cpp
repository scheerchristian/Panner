#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <math.h>
//#include <include_juce_audio_formats.cpp>


//==============================================================================
//                          Constructor
//==============================================================================

PannerAudioProcessorEditor::PannerAudioProcessorEditor (PannerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    xyPad = std::make_unique<XYPad>();
    // connect the ParameterAttachments of the XYPad to the APVTS in the AudioProcessor. These keep the knob-position of the XYPad up to date.
    xyPad->connectXtoParameter(*audioProcessor.apvts.getParameter("xValue"));
    xyPad->connectYtoParameter(*audioProcessor.apvts.getParameter("zValue"));
    xyPad->connectRadiusToParameter(*audioProcessor.apvts.getParameter("radius"));
    xyPad->connectAzimutToParameter(*audioProcessor.apvts.getParameter("azimut"));
    xyPad->connectCircleModeToParameter(*audioProcessor.apvts.getParameter("circleMode"));


    xyPad->setRoomSize(audioProcessor.getRoomSize(0), 0);
    xyPad->setRoomSize(audioProcessor.getRoomSize(1), 1);
    addAndMakeVisible(xyPad.get());

    startTimerHz(1);
    setLookAndFeel(&chrystisLookAndFeel);
    

    
    /*
    setWantsKeyboardFocus(true);
    xCoordinateTextEditor.setWantsKeyboardFocus(true);
    zCoordinateTextEditor.setWantsKeyboardFocus(true);
    */
    
    // connect the ParameterAttachments of the PluginEditor to the APVTS in the AudioProcessor. These keep the value-TextEditors up to date.
    xAttachment = std::make_unique<juce::ParameterAttachment>(*audioProcessor.apvts.getParameter("xValue"),
                                                            [this](float newXValue)
                                                            {
                                                                // callback lambda function. Called whenever parameter changes.
          
                                                                    xyPad->setValue(newXValue, 0);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newXValue, 0), 0);
                                                                    
                                                                    updateTextBox((std::to_string(floor(newXValue * 100.0 + 0.5) / 100.0)), 0);
                                                                    //viewButton->setToggleState(xyPad->isCircleModeOn(), dontSendNotification);
                                                                    xyPad->resized();


                                                                    repaint();
                                                            }, nullptr
                                                            );

    zAttachment = std::make_unique<juce::ParameterAttachment>(*audioProcessor.apvts.getParameter("zValue"),
                                                            [this](float newZValue)
                                                            {
                                                                // callback function whenever the zParameter gets changed
                                                               
                                                                    xyPad->setValue(newZValue, 1);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newZValue, 1), 1);
                                                                    
                                                                    updateTextBox((std::to_string(floor(newZValue * 100.0 + 0.5) / 100.0)), 1);
                                                                    xyPad->resized();
                                                                    repaint();


                                                            }, nullptr
                                                            );

    circleModeAttachment = std::make_unique<ParameterAttachment>(*audioProcessor.apvts.getParameter("circleMode"),
                                                            [this](bool newValue)
                                                            {
                                                                // callback lambda function. Called whenever parameter changes.

                                                                if (newValue == true)
                                                                {
                                                                    xAttachment->sendInitialUpdate();
                                                                    zAttachment->sendInitialUpdate();
                                                                    audioProcessor.lastRadiusValue = xyPad->translateValueToRadiusAndAzimut(audioProcessor.lastXValue, audioProcessor.lastZValue, true);
                                                                    audioProcessor.lastAzimutValue = xyPad->translateValueToRadiusAndAzimut(audioProcessor.lastXValue, audioProcessor.lastZValue, false);
                                                                    radiusAttachment->setValueAsCompleteGesture(audioProcessor.lastRadiusValue);
                                                                    azimutAttachment->setValueAsCompleteGesture(audioProcessor.lastAzimutValue);
                                                                    xyPad->setCircleMode(newValue);
                                                                }
                                                                else
                                                                {
                                                                    radiusAttachment->sendInitialUpdate();
                                                                    azimutAttachment->sendInitialUpdate();
                                                                    
                                                                    audioProcessor.lastXValue = audioProcessor.translateRadiusAndAzimutToValues(0, audioProcessor.lastRadiusValue, audioProcessor.lastAzimutValue);
                                                                    audioProcessor.lastZValue = audioProcessor.translateRadiusAndAzimutToValues(1, audioProcessor.lastRadiusValue, audioProcessor.lastAzimutValue);
                                                                    xAttachment->setValueAsCompleteGesture(audioProcessor.lastXValue);
                                                                    zAttachment->setValueAsCompleteGesture(audioProcessor.lastZValue);
                                                                    xyPad->setCircleMode(newValue);
                                                                }

                                                                viewButton->setToggleState(newValue, dontSendNotification);
                                                                //repaint();
                                                            }, nullptr);

    radiusAttachment = std::make_unique<ParameterAttachment>(*audioProcessor.apvts.getParameter("radius"),
                                                            [this](float newRadius)
                                                            {
                                                                // callback lambda function. Called whenever parameter changes.
                                                             
                                                                    
                                                                    float newXValue = audioProcessor.translateRadiusAndAzimutToValues(0, newRadius, audioProcessor.lastAzimutValue);
                                                                    float newZValue = audioProcessor.translateRadiusAndAzimutToValues(1, newRadius, audioProcessor.lastAzimutValue);
                                                                
                                                                    xyPad->setValue(newXValue, 0);
                                                                    xyPad->setValue(newZValue, 1);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newXValue, 0), 0);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newZValue, 1), 1);
                                                                    xyPad->resized();
                                                                    
                                                                    updateTextBox((std::to_string(floor(newRadius * 100.0 + 0.5) /100.0)), 0);
                         
                                                                    repaint();
                                                            },  nullptr
                                                            );              
    azimutAttachment = std::make_unique<ParameterAttachment>(*audioProcessor.apvts.getParameter("azimut"),
                                                            [this](float newAzimut)
                                                            {
                                                                // callback lambda function. Called whenever parameter changes.
                                                                
                                                                    audioProcessor.lastAzimutValue = newAzimut;
                                                                    float newXValue = audioProcessor.translateRadiusAndAzimutToValues(0, audioProcessor.lastRadiusValue, newAzimut);
                                                                    float newZValue = audioProcessor.translateRadiusAndAzimutToValues(1, audioProcessor.lastRadiusValue, newAzimut);
                                                                    xyPad->setValue(newXValue, 0);
                                                                    xyPad->setValue(newZValue, 1);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newXValue, 0), 0);
                                                                    xyPad->setCoordinate(xyPad->translateValueToCoordinate(newZValue, 1), 1);
                                                                    xyPad->resized();
                                                                    updateTextBox((std::to_string(floor(newAzimut * 100.0 + 0.5) / 100.0)), 1);

                                                                    repaint();
                     
                                                            }, nullptr
                                                            );

    oscChannelAttachment = std::make_unique<ParameterAttachment>(*audioProcessor.apvts.getParameter("oscChannel"),
                                                            [this](int newChannel)
                                                            {
                                                                
                                                            });





    addAndMakeVisible(xCoordinateTextEditor);
        xCoordinateTextEditor.setColour(TextEditor::ColourIds::textColourId, Colour(BLUE));
        chrystisLookAndFeel.GraphikSemibold.setHeight(22);
        xCoordinateTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
        xCoordinateTextEditor.setJustification(Justification::left);
        xCoordinateTextEditor.setKeyboardType(TextInputTarget::decimalKeyboard);
        xCoordinateTextEditor.setInputRestrictions(6, "0123456789.-");
        xCoordinateTextEditor.addListener(this);
        xCoordinateTextEditor.setSelectAllWhenFocused(true);
        xCoordinateTextEditor.onReturnKey = [this]
        {
            if (!xyPad->knob.isMouseOverOrDragging())
            {
                if (xyPad->isCircleModeOn() == false)
                {
                    xAttachment->setValueAsCompleteGesture(xCoordinateTextEditor.getText().getFloatValue());
                }

                else
                {
                    radiusAttachment->setValueAsCompleteGesture(xCoordinateTextEditor.getText().getFloatValue());
                    azimutAttachment->setValueAsCompleteGesture(zCoordinateTextEditor.getText().getFloatValue());
                }
            }
            // work around for a juce bug
            if (xyPad->isCircleModeOn() == true)
            {
                xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
                xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
            }
            else
            {
                xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
                xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
            }
        };

    addAndMakeVisible(zCoordinateTextEditor);
        zCoordinateTextEditor.setColour(TextEditor::ColourIds::textColourId, Colour(BLUE));
        zCoordinateTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
        chrystisLookAndFeel.GraphikSemibold.setHeight(22);
        zCoordinateTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
        zCoordinateTextEditor.setJustification(Justification::left);
        zCoordinateTextEditor.setKeyboardType(TextInputTarget::decimalKeyboard);
        zCoordinateTextEditor.setInputRestrictions(6, "0123456789.-");
        zCoordinateTextEditor.addListener(this);
        zCoordinateTextEditor.setSelectAllWhenFocused(true);
        zCoordinateTextEditor.onReturnKey = [this]
        {
            if (!xyPad->knob.isMouseOverOrDragging())
            {
                if (xyPad->isCircleModeOn() == false)
                {     
                    zAttachment->setValueAsCompleteGesture(zCoordinateTextEditor.getText().getFloatValue());   
                }
                else
                {
                    radiusAttachment->setValueAsCompleteGesture(xCoordinateTextEditor.getText().getFloatValue());
                    azimutAttachment->setValueAsCompleteGesture(zCoordinateTextEditor.getText().getFloatValue());
                }
            }
            // work around for a juce bug
            if (xyPad->isCircleModeOn() == true)
            {
                xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
                xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
            }
            else
            {
                xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
                xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
            }
        };
    
    viewButton = std::make_unique<ViewButton>("");
    addAndMakeVisible(viewButton.get());
    viewButton->onClick = [this]
    {
        if (viewButton->getToggleState() == false)
        {
            //viewButton->setToggleState(true, dontSendNotification);
            circleModeAttachment->setValueAsCompleteGesture(true);
            //repaint();
        }
        else
        {
            //viewButton->setToggleState(false, dontSendNotification);
            circleModeAttachment->setValueAsCompleteGesture(false);
            //repaint();
        }

        // work around for a juce bug
        if (xyPad->isCircleModeOn() == true)
        {
            xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
            xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
        }
        else
        {
            xyPad->circleModeAttachment->setValueAsCompleteGesture(true);
            xyPad->circleModeAttachment->setValueAsCompleteGesture(false);
        }
    };


    // work around for a juce bug
    if (audioProcessor.isCircleModeOn == true)
    {
        circleModeAttachment->setValueAsCompleteGesture(false);
        circleModeAttachment->setValueAsCompleteGesture(true);
    }
    else
    {
        circleModeAttachment->setValueAsCompleteGesture(true);
        circleModeAttachment->setValueAsCompleteGesture(false);
    }
    
    
    settingsPage = std::make_unique<SettingsPage>(&audioProcessor);

    settingsButton = std::make_unique<SettingsButton>("");
    addAndMakeVisible(settingsButton.get());
    settingsButton->onClick = [this]
    {
        addAndMakeVisible(settingsPage.get());
        settingsPage->startTimerHz(1);
    };
    
    settingsWheelShadow.colour = Colour(SHADOW);
    settingsWheelShadow.radius = 40;
    settingsWheelShadow.offset = {0, 10};
    
    infoPage = std::make_unique<InfoPage>();
    infoPage->setBounds(0, 0, 375, 1876);
    
    infoButton = std::make_unique<InfoButton>("");
    addAndMakeVisible(infoButton.get());
    infoButton->onClick = [this]
    {
        addAndMakeVisible(infoPage.get());
    };

    setSize (375, 827); // iphone 11 display size
    setResizable(true, true);
    setResizeLimits(375, 827, 714, 1644);
    
}

   

//==============================================================================
//                          Deonstructor
//==============================================================================
PannerAudioProcessorEditor::~PannerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}
//==============================================================================
//                          Paint
//==============================================================================

void PannerAudioProcessorEditor::paint (juce::Graphics& g)  //paint the main screen
{
    resized();

    g.fillAll (Colour(BG)); // setting the main BG Colour
      
    //settingsButton->repaint();
    Path shadowOrigin;
    shadowOrigin.addEllipse(settingsSection.toFloat());
    settingsWheelShadow.drawForPath(g, shadowOrigin); // draw drop shadow for the settings button
    shadowOrigin.clear();
    
    // write title
    g.setColour(Colour(BLACK));
    chrystisLookAndFeel.GraphikBold.setHeight(titleSection.getHeight());
    g.setFont(chrystisLookAndFeel.GraphikBold);
    g.drawText("Panner", titleSection, Justification::left);
    
    //write view description
    chrystisLookAndFeel.GraphikRegular.setHeight(viewTitleSection.getHeight() - 18);
    g.setFont(chrystisLookAndFeel.GraphikRegular);
    g.drawText("View", viewTitleSection, Justification::left);
    
    // write coordinates title
    g.setColour(Colour(BLACK));
    chrystisLookAndFeel.GraphikBold.setHeight(coordinatesTitleSection.getHeight());
    g.setFont(chrystisLookAndFeel.GraphikBold);
    g.drawText("Coordinates", coordinatesTitleSection, Justification::left);
    
    chrystisLookAndFeel.GraphikRegular.setHeight(xSection.getHeight() - 18);
    g.setFont(chrystisLookAndFeel.GraphikRegular);
    if (xyPad->isCircleModeOn() == false)
    {
        g.drawText("X", xSection, Justification::centredLeft);
        g.drawText("Z", zSection, Justification::centredLeft);
    }
    else
    {
        g.drawText("radius", xSection, Justification::centredLeft);
        g.drawText("angle", zSection, Justification::centredLeft);
    }
    chrystisLookAndFeel.GraphikRegular.setHeight(15);
    g.setFont(chrystisLookAndFeel.GraphikRegular);
    g.setColour(Colour(GREY4));
   
    g.drawText("m", mX, Justification::centredRight);

    if (xyPad->isCircleModeOn() == false)
    {
        g.drawText("m", mZ, Justification::centredRight);
    }
    else
    {
        g.drawText("deg", mZ, Justification::centredRight);
    }
}
//==============================================================================
//                          Resized
//==============================================================================
void PannerAudioProcessorEditor::resized()  // gets called when main screen is resized
{
    if (infoPage->isVisible())
        infoPage->setBounds(getLocalBounds());

    if (settingsPage->isVisible())
        settingsPage->setBounds(getLocalBounds());
    
    r = getLocalBounds().reduced(26, 26);   // creates rectangle for whole screen excluding the surrounding safery ring of 26 px

        
    
    // sets rectangle for the  section of the settigs icon
    settingsSection = r.removeFromTop(81);
    infoSection = settingsSection;
    settingsSection.removeFromLeft(settingsSection.getWidth() - settingsSection.getHeight());
    //settingsWheelShadow.offset = {settingsSection.getX(), settingsSection.getY() + 10}; // make the drop shadow follow the button
    settingsButton->setBounds(settingsSection); // make the button snap into its section
    
    infoSection.removeFromTop(25);
    infoSection.removeFromBottom(25);
    infoSection.removeFromRight(r.getWidth() - 30);
    infoButton->setBounds(infoSection);
    
    r.removeFromTop(26);    //remove space to next component downwards
    
    titleSection = r.removeFromTop(55); // create rectangle for title section
    
    r.removeFromTop(13);
    viewTitleSection = r.removeFromTop(40);
    viewButtonSection = viewTitleSection.removeFromRight(107);
    viewButton->setBounds(viewButtonSection);
    r.removeFromTop(52);

    r.removeFromBottom(26);
    zSection = r.removeFromBottom(40);
    r.removeFromBottom(14);
    xSection = r.removeFromBottom(40);
    r.removeFromBottom(11);
    coordinatesTitleSection = r.removeFromBottom(28);

    mX = xSection;
    mX.removeFromRight(5);
    mZ = zSection;
    mZ.removeFromRight(5);


    r.removeFromBottom(52);

    pannerSection = r;
    pannerSection.reduce(13, 0);
    pannerSection.setCentre(getLocalBounds().getWidth() / 2, pannerSection.getCentreY());   // makes the panner central at any window size
    xyPad->setBounds(pannerSection);
    
    
    xCoordinateTextEditor.setBounds(getLocalBounds().getWidth() - 26 - 107, xSection.getY(), 107, xSection.getHeight());
    zCoordinateTextEditor.setBounds(getLocalBounds().getWidth() - 26 - 107, zSection.getY(), 107, zSection.getHeight());
    if (xyPad->isCircleModeOn() == false)
     {
         xAttachment->sendInitialUpdate();
         zAttachment->sendInitialUpdate();
     }
     else
     {
         radiusAttachment->sendInitialUpdate();
         azimutAttachment->sendInitialUpdate();
     }
    /*
    xAttachment->sendInitialUpdate();
    zAttachment->sendInitialUpdate();
    
    xyPad->setCoordinate(xyPad->translateValueToCoordinate(xyPad->getValue(0), 0), 0);
    xyPad->setCoordinate(xyPad->translateValueToCoordinate(xyPad->getValue(1), 1), 1);
    */
    
    xyPad->resized();
}

void PannerAudioProcessorEditor::textEditorTextChanged(TextEditor& editor)
{
}

void PannerAudioProcessorEditor::textEditorReturnKeyPressed(TextEditor& editor)
{
}

void PannerAudioProcessorEditor::updateTextBox(String s, int axis)
{
    if (axis == 0)
    {
        if (!xCoordinateTextEditor.hasKeyboardFocus(false))
        {
            String xCoordinateString = s;
            xCoordinateTextEditor.setText(xCoordinateString.trimCharactersAtEnd("0"));
        }
    }
    
    else        
    {
        if (!zCoordinateTextEditor.hasKeyboardFocus(false))
        {
            String zCoordinateString = s;
            zCoordinateTextEditor.setText(zCoordinateString.trimCharactersAtEnd("0"));
        }
    }
}
/*
void PannerAudioProcessorEditor::updateTextBoxToDeg(float newValue, int axis)
{
    if (!xCoordinateTextEditor.hasKeyboardFocus(false) && !zCoordinateTextEditor.hasKeyboardFocus(false))
    {
        float rad;
        float angle;
        float r;
        float currentX;
        float currentZ;

        if (axis == 0)
        {
            lastXValue = newValue;
            currentX = newValue;
            currentZ = lastZValue;
        }
        else
        {
            lastZValue = newValue;
            currentX = lastXValue;
            currentZ = newValue;
        }
            // von oben rechts entgegen dem Uhrzeigersinn

            if (currentX >= 0 && currentZ < 0)  // 1. Quadrant
            {
                if (currentX == 0)
                {
                    rad = 0;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentZ = sqrt(currentZ * currentZ);
                    rad = atan(currentX / currentZ);
                }
            }
            else if (currentX < 0 && currentZ <= 0) // 2. Quadrant
            {
                if (currentZ == 0)
                {
                    rad = -M_PI / 2;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentZ = sqrt(currentZ * currentZ);
                    rad = -atan(currentX / currentZ);
                }
            }
            else if (currentX <= 0 && currentZ > 0) // 3. Quadrant
            {
                if (currentX == 0)
                {
                    rad = M_PI;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentZ = sqrt(currentZ * currentZ);
                    rad = -(M_PI - atan(currentX / currentZ));
                }
            }
            else if (currentX > 0 && currentZ >= 0)    // 4. Quadrant
            {
                if (currentZ == 0)
                {
                    rad = M_PI / 2;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentZ = sqrt(currentZ * currentZ);
                    rad = M_PI - atan(currentX / currentZ);
                }
            }
    
            angle = rad * 180 / M_PI;
            double roundedAngle = floor(angle * 100.0 + .5) / 100.0;
            r = sqrt(pow(currentX, 2) + pow(currentZ, 2));
            r = floor(r * 100.0 + .5) / 100.0;
            String rString = std::to_string(r);
            String angleString = std::to_string(roundedAngle);

            xCoordinateTextEditor.setText(rString.trimCharactersAtEnd("0"));
            zCoordinateTextEditor.setText(angleString.trimCharactersAtEnd("0"));
            repaint();
      } 
}

void PannerAudioProcessorEditor::updateTextBoxFromDeg(float radius, float azimut)
{
    float rad = (azimut * M_PI) / 180;
    float x = lastXValue;
    float z = lastZValue;
    // von oben rechts im Uhrzeigersinn

    if (azimut >= 0.0f && azimut < 90.0f) // 1. Quadrant
    {
        if (azimut == 0)
        {
            x = 0.0f;
            z = -radius;
        }
        else
        {
            x = cos(rad) * radius;
            z = -sin(rad) * radius;
        }
    }

    if (azimut >= 90 && azimut <= 180) // 2. Quadrant
    {
        if (azimut == 90)
        {
            x = radius;
            z = 0.0f;
        }
        else if (azimut = 180)
        {
            x = 0.0f;
            z = radius;
        }
        else
        {
            x = cos(rad) * radius;
            z = sin(rad) * radius;
        }
    }
    else if (azimut >= -180 && azimut < -90) // 3. Quadrant
    {
        if (azimut == 180)
        {
            x = 0;
            z = radius;
        }
        else
        {
            x = -cos(rad) * radius;
            z = sin(rad) * radius;
        }
    }

    else if (azimut >= -90 && azimut < 0) // 4. Quadrant
    {
        if (azimut == -90)
        {
            x = -radius;
            z = 0.0f;
        }
        else
        {
            x = -cos(rad) * radius;
            z = -sin(rad) * radius;
        }
    }
    lastXValue = x;
    lastZValue = z;
    String xString = std::to_string(x);
    String zString = std::to_string(z);
    xCoordinateTextEditor.setText(xString.trimCharactersAtEnd("0"));
    zCoordinateTextEditor.setText(zString.trimCharactersAtEnd("0"));
}

void PannerAudioProcessorEditor::readTextBoxDeg()
{
    float radius = xCoordinateTextEditor.getText().getFloatValue();
    float angle = zCoordinateTextEditor.getText().getFloatValue();
    float rad = angle * M_PI / 180;
    float xValue;
    float zValue;
    //von oben rechts im Uhrzeigersinn
    if (angle >= 0 && angle < 90)   // 1. Quadrant
    {
        if (angle == 0)
        {
            xValue = 0;
            zValue = -radius;
        }
        else
        {
            xValue = sin(rad) * radius;
            zValue = -cos(rad) * radius;
        }
    }
    else if (angle >= 90 && angle <= 180)   // 2. Quadrant
    {
        if (angle == 90)
        {
            xValue = radius;
            zValue = 0;
        }
        else if (angle == 180)
        {
            xValue = 0;
            zValue = radius;
        }
        else 
        {
            xValue = sin(rad) * radius;
            zValue = -cos(rad) * radius;
        }
    }
    else if (angle >= -180 && angle < -90)  // 3. Quadrant
    {
        if (angle == -180)
        {
            xValue = 0;
            zValue = radius;
        }
        else
        {
            xValue = sin(rad) * radius;
            zValue = -cos(rad) * radius;
        }
    }
    else if (angle >= -90 && angle < -0)    // 4. Quadrant
    {
        if (angle == -90)
        {
            xValue = -radius;
            zValue = 0;
        }
        else
        {
            xValue = sin(rad) * radius;
            zValue = -cos(rad) * radius;
        }
    }

    xAttachment->setValueAsCompleteGesture(xValue);
    zAttachment->setValueAsCompleteGesture(zValue);
}
*/
void PannerAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if (xCoordinateTextEditor.isMouseButtonDown())
    {
        setWantsKeyboardFocus(true);
        xCoordinateTextEditor.setWantsKeyboardFocus(true);
    }
    else if (zCoordinateTextEditor.isMouseButtonDown())
    {
        setWantsKeyboardFocus(true);
        zCoordinateTextEditor.setWantsKeyboardFocus(true);
    }
}
void PannerAudioProcessorEditor::timerCallback()
{
    xyPad->setRoomSize(audioProcessor.getRoomSize(0), 0);
    xyPad->setRoomSize(audioProcessor.getRoomSize(1), 1);
    //circleModeAttachment->sendInitialUpdate();
}

//==============================================================================
//                          Settings Page
//==============================================================================

SettingsPage::SettingsPage(PannerAudioProcessor* parent)
{
    m_parent = parent;
    chrystisLookAndFeel.cornerRadius = 10;
    addMouseListener(&mouseListener, true);
    
    closeButton = std::make_unique<CloseButton>("");
    udpPortTextEditor = std::make_unique<TextEditor>();
    hostIPTextEditor = std::make_unique<TextEditor>();
    oscChannelTextEditor = std::make_unique<TextEditor>();
    addAndMakeVisible(closeButton.get());
    closeButton->onClick = [this]
    {
        stopTimer();
        setVisible(false);
        getParentComponent()->repaint();
        //settingsButton->setVisible(true);
    };
    
    chrystisLookAndFeel.GraphikSemibold.setHeight(20);
    
    oscChannelAttachment = std::make_unique<juce::ParameterAttachment>(*parent->apvts.getParameter("oscChannel"),
                                        [=, this](int newValue) 
                                        {
                                            oscChannelTextEditor->setFont(chrystisLookAndFeel.GraphikSemibold);  
                                            oscChannelTextEditor->setText(std::to_string(newValue)); 
                                        }, nullptr);

    oscChannelAttachment->sendInitialUpdate();
    addAndMakeVisible(oscChannelTextEditor.get());
        oscChannelTextEditor->setFont(chrystisLookAndFeel.GraphikSemibold);
        oscChannelTextEditor->setJustification(Justification::centredRight);
        oscChannelTextEditor->setInputRestrictions( 3, "01234567890");

    addAndMakeVisible(hostIPTextEditor.get());
        hostIPTextEditor->setFont(chrystisLookAndFeel.GraphikSemibold);
        hostIPTextEditor->setJustification(Justification::centredRight);
        hostIPTextEditor->setInputRestrictions(15, "0123456789.");
        hostIPTextEditor->setText(parent->getIP());
        hostIPTextEditor->onTextChange = [=, this]
            {
                //ipValue = hostIPTextEditor.getText();
                parent->setIP(hostIPTextEditor->getText());
            };
    addAndMakeVisible(udpPortTextEditor.get());
        udpPortTextEditor->setFont(chrystisLookAndFeel.GraphikSemibold);
        udpPortTextEditor->setJustification(Justification::centredRight);
        udpPortTextEditor->setInputRestrictions(4, "0123456789");
        //udpPortTextEditor.setText(std::to_string(udpValue));
        udpPortTextEditor->setText(std::to_string(parent->getUDP()));
        udpPortTextEditor->onTextChange = [=, this]
        {
            parent->setUDP(udpPortTextEditor->getText().getIntValue());
        };
    
    saveButton = std::make_unique<SaveButton>("");
    addAndMakeVisible(saveButton.get());
    saveButton->onClick = [=, this]
    {
        // change the value of the osc parameter
        oscChannelAttachment->setValueAsCompleteGesture(oscChannelTextEditor->getText().getIntValue());
        
        // reconnect the osc sender with the new ip adress and udp port
        parent->setSettings();

        // resize mainComponent so that the current value gets sent again
        getParentComponent()->resized();
    };
    
    saveButtonShadow.colour = Colour(SHADOW);
    saveButtonShadow.offset = {0, 10};
    saveButtonShadow.radius = 40;
}
SettingsPage::~SettingsPage()
{
    setLookAndFeel(nullptr);
    
}


void SettingsPage::paint(Graphics& g)
                              
{
    setLookAndFeel(&chrystisLookAndFeel);
    settingsBG.rescaled(getLocalBounds().getWidth(), getLocalBounds().getHeight());
    parentSizeChanged();
    g.drawImageWithin(settingsBG, -10, -10, getLocalBounds().getWidth() + 20, getLocalBounds().getHeight() + 20, RectanglePlacement::stretchToFit);

    
    g.setColour(Colour(BLUE));
    g.setOpacity(0.7);
    pathBlueBG.addRoundedRectangle(blueBG.getX(), blueBG.getY(), blueBG.getWidth(), blueBG.getHeight(), 10, 10, true, true, false, false);
    g.fillPath(pathBlueBG);
    
    g.setColour(Colour(BG));
    
    g.setFont(chrystisLookAndFeel.GraphikBold);
    g.setFont(settingsTitleSection.getHeight());
    g.drawText("Settings", settingsTitleSection, Justification::left);
    
    g.setFont(chrystisLookAndFeel.GraphikSemibold);
    g.setFont(roomSizeTitleSection.getHeight());
    g.drawText("Room Size", roomSizeTitleSection, Justification::left);
    g.setFont(chrystisLookAndFeel.GraphikRegular);
    g.setFont(20);

    g.drawText(std::to_string(m_parent->getRoomSize(0)).erase(4, 6), roomSizeXValueSection, Justification::right);
    g.drawText(std::to_string(m_parent->getRoomSize(1)).erase(4, 6), roomSizeZValueSection, Justification::right);
    g.setFont(chrystisLookAndFeel.GraphikRegular);
    g.setFont(22);
    g.drawText("X", xSection, Justification::centredLeft);
    
    g.drawText("Z", zSection, Justification::centredLeft);
    
    g.drawLine(seperationLine.toFloat());
    
    g.drawText("OSC Channel", oscSection, Justification::left);
    
    g.drawText("Host IP", ipSection, Justification::left);
    
    g.drawText("UDP Port", udpSection, Justification::left);
    
    saveButtonShadow.drawForRectangle(g, saveButton->getBounds());
    
    pathBlueBG.clear();
}
void SettingsPage::mouseDown(const MouseEvent& event)
{
    
}
void SettingsPage::parentSizeChanged()
{
    blueBG.setBounds(13, 26, getLocalBounds().getWidth() - 26, getLocalBounds().getHeight() - 26);
    r = blueBG.reduced(32, 32);
    
    closeSection = r.removeFromTop(16);
    closeSection.removeFromLeft(closeSection.getWidth() - 3 - closeSection.getHeight() - 3);
    r.removeFromTop(59);
    settingsTitleSection = r.removeFromTop(55);
    
    r.removeFromTop(25);
    roomSizeTitleSection = r.removeFromTop(28);
    
    r.removeFromTop(11);
    xSection = r.removeFromTop(40);
    roomSizeXValueSection.setBounds(xSection.getRight() - 107, xSection.getY(), 107, xSection.getHeight());
    
    r.removeFromTop(14);
    zSection = r.removeFromTop(40);
    roomSizeZValueSection.setBounds(zSection.getRight() - 107, zSection.getY(), 107, zSection.getHeight());


    r.removeFromTop(40);
    seperationLine = {r.getX(), r.getY(), r.getRight(), r.getY()};
    
    r.removeFromTop(36);
    oscSection = r.removeFromTop(40);
    oscChannelTextEditor->setBounds(oscSection.getRight() - 107, oscSection.getY(), 107, oscSection.getHeight());

    
    
    r.removeFromTop(17);
    ipSection = r.removeFromTop(40);
    hostIPTextEditor->setBounds(ipSection.getRight() - 148, ipSection.getY(), 148, ipSection.getHeight());


    
    r.removeFromTop(17);
    udpSection = r.removeFromTop(40);
    udpPortTextEditor->setBounds(udpSection.getRight() - 107, udpSection.getY(), 107, udpSection.getHeight());


    
    r.removeFromBottom(78);
    saveSection = r.removeFromBottom(60);
    saveSection.setWidth(blueBG.getWidth() - 182);
    saveSection.setCentre(blueBG.getCentreX(), saveSection.getCentreY());
    //saveSection.reduce(72, 0);
    saveButton->setBounds(saveSection);
    
    closeButton->setBounds(closeSection);
    
}

void SettingsPage::timerCallback()
{
    repaint(roomSizeXValueSection);
    repaint(roomSizeZValueSection);
}

//==============================================================================
//                          Info Page
//==============================================================================

InfoPage::InfoPage()
{
    viewport = std::make_unique<Viewport>();
    textContainer = std::make_unique<TextContainer>(chrystisLookAndFeel);
    textContainer->setBounds(0, 0, 285, 1725);
  
    addMouseListener(&mouseListener, true);
    closeButton = std::make_unique<CloseButton>("");

    closeButton->onClick = [this]
    {
        setVisible(false);
        getParentComponent()->repaint();
    };
    addAndMakeVisible(closeButton.get());
    viewport->setViewedComponent(textContainer.get(), false);
    addAndMakeVisible(viewport.get());
    viewport->setScrollBarsShown(true, false);
    viewport->setScrollBarThickness(5);
    
}

InfoPage::~InfoPage()
{
    
   setLookAndFeel(nullptr);
   
}

void InfoPage::paint(Graphics& g)
{
    setLookAndFeel(&chrystisLookAndFeel);
    g.drawImageWithin(settingsBG, -10, -10, getLocalBounds().getWidth() + 20, getLocalBounds().getHeight() + 40, RectanglePlacement::stretchToFit);
    g.setColour(Colour(BLUE));
    g.setOpacity(0.7);
    Path pathBlueBG;
    pathBlueBG.addRoundedRectangle(blueBG.getX(), blueBG.getY(), blueBG.getWidth(), blueBG.getHeight(), 10, 10, true, true, false, false);
    g.fillPath(pathBlueBG);
    pathBlueBG.clear();
    parentSizeChanged();
}

void InfoPage::mouseDown(const MouseEvent& event)
{
    
}

void InfoPage::parentSizeChanged()
{
    
    blueBG.setBounds(13, 26, getLocalBounds().getWidth() - 26, getLocalBounds().getHeight() - 26);
    r = blueBG.reduced(32, 32);
    
    closeSection = r.removeFromTop(16);
    closeSection.removeFromLeft(closeSection.getWidth() - 3 - closeSection.getHeight() - 3);
    closeButton->setBounds(closeSection);
    r.removeFromTop(58);
    viewPortSection = r.removeFromTop(getLocalBounds().getHeight() - viewPortSection.getY());
    viewport->setBounds(viewPortSection);
}



//==============================================================================
//                          Settings Button
//==============================================================================
SettingsButton::SettingsButton(const String& buttonName): Button(buttonName)
{
}
void SettingsButton::paintButton(Graphics& g,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown)
{
    // paint the blue button background
    r = getLocalBounds().toFloat();
    outline.addEllipse(r);
    g.setColour(Colour(BLUE));
    g.fillPath(outline);
    
    // paint the wheel symbol after a given SVG path file
    
    r.reduce(24, 24);
    pathIcon.loadPathFromData (pathDataSettingsWheel, sizeof (pathDataSettingsWheel));
    pathIcon.scaleToFit(r.getX(), r.getY(), r.getWidth(), r.getHeight(), true);
    g.setColour(Colour(GREY));
    g.fillPath(pathIcon);
    pathIcon.clear();
    outline.clear();
}


//==============================================================================
//                          Info Button
//==============================================================================

InfoButton::InfoButton(const String& buttonName): Button(buttonName)
{
    
}
void InfoButton::paintButton(Graphics& g,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown)
{
    // paint the wheel symbol after a given SVG path file
    r = getLocalBounds().toFloat();
    pathIcon.loadPathFromData(pathDataInfo, sizeof(pathDataInfo));
    pathIcon.scaleToFit(r.getX(), r.getY(), r.getWidth(), r.getHeight(), true);
    g.setColour(Colour(BLUE));
    g.fillPath(pathIcon);
    pathIcon.clear();
    
}


//==============================================================================
//                          Close Button
//==============================================================================

CloseButton::CloseButton(const String& buttonName): Button(buttonName)
{
}

void CloseButton::paintButton(Graphics& g,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown)
{
    r = getLocalBounds().toFloat();
    pathIcon.loadPathFromData (pathDataCloseButton, sizeof (pathDataCloseButton));
    pathIcon.scaleToFit(r.getX(), r.getY(), r.getWidth(), r.getHeight(), true);
    g.setColour(Colour(GREY));
    g.fillPath(pathIcon);
    pathIcon.clear();
}

//==============================================================================
//                          Save Button
//==============================================================================
SaveButton::SaveButton(const String& buttonName): Button(buttonName)
{
}

void SaveButton::paintButton(Graphics& g,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown)
{
    circleLeft = getLocalBounds().toFloat();
    circleRight = getLocalBounds().toFloat();
    midPart = getLocalBounds();
    checkPart = getLocalBounds();
    r = getLocalBounds().toFloat();
    
    midPart.reduce(29, 0);
    circleLeft.removeFromRight(r.getWidth() - circleLeft.getHeight());
    circleRight.removeFromLeft(r.getWidth() - circleRight.getHeight());
    pathCircleLeft.addEllipse(circleLeft);
    pathCircleRight.addEllipse(circleRight);
    pathMidPart.addRectangle(midPart);
    g.setColour(Colour(BG));
    g.fillPath(pathMidPart);
    g.fillPath(pathCircleLeft);
    g.fillPath(pathCircleRight);
    
    g.setFont(chrystisLookAndFeel.GraphikSemibold);
    g.setFont(20);
    g.setColour(Colour(BLUE));
    g.drawText("Save", r, Justification::centred);
    
    checkPart.setWidth(15);
    checkPart.setHeight(18);
    checkPart.setCentre(34, r.getCentreY());

    pathIcon.loadPathFromData (pathDataCheck, sizeof(pathDataCheck));
    pathIcon.scaleToFit(checkPart.getX(), checkPart.getY() - 3, checkPart.getWidth() + 3, checkPart.getHeight() + 3, true);
    g.setColour(Colour(BLUE));
    g.strokePath(pathIcon, checkIconStrokeType);
    
    outline.clear();
    pathIcon.clear();
    pathCircleRight.clear();
    pathCircleRight.clear();
    pathMidPart.clear();
}

//==============================================================================
//                          View Buttons
//==============================================================================
ViewButton::ViewButton(const String& buttonName) : Button(buttonName)
{

}
void ViewButton::paintButton(Graphics& g,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    r = getLocalBounds().toFloat();
    g.setColour(Colour(GREY4));
    g.drawRoundedRectangle(r, 5, 1);
    Path shadowOutline;
    DropShadow viewButtonShadow;
    viewButtonShadow.offset = { 2, 2 };
    viewButtonShadow.radius = 5;
    viewButtonShadow.colour = Colour(SHAWOWVIEWBUTTON);
    if (this->getToggleState() == false)
    {
        shadowOutline.addRoundedRectangle(0.0f, 0.0f, r.getWidth() / 2, r.getHeight(), 3.0f);
        viewButtonShadow.drawForPath(g, shadowOutline);
        shadowOutline.clear();
        g.setColour(Colour(BLUE));
        g.fillRoundedRectangle(0.0f, 0.0f, r.getWidth() / 2, r.getHeight(), 3.0f);
        g.setColour(Colour(BG));
        g.drawRoundedRectangle(0.25 * r.getWidth() - 7.5, 0.5 * r.getHeight() - 7.5, 15, 15, 1, 2);
        g.setColour(Colour(BLUE));
        g.drawEllipse(0.75 * r.getWidth() - 7.5, 0.5 * r.getHeight() - 7.5, 15, 15, 2);

        
    }
    else
    {
        shadowOutline.addRoundedRectangle(r.getWidth() / 2, 0.0f, r.getWidth() / 2, r.getHeight(), 3.0f);
        viewButtonShadow.drawForPath(g, shadowOutline);
        shadowOutline.clear();
        g.setColour(Colour(BLUE));
        g.fillRoundedRectangle(r.getWidth() / 2, 0.0f, r.getWidth() / 2, r.getHeight(), 3.0f);
        g.setColour(Colour(BLUE));
        g.drawRoundedRectangle(0.25 * r.getWidth() - 7.5, 0.5 * r.getHeight() - 7.5, 15, 15, 1, 2);
        g.setColour(Colour(BG));
        g.drawEllipse(0.75 * r.getWidth() - 7.5, 0.5 * r.getHeight() - 7.5, 15, 15, 2);


    }
}
