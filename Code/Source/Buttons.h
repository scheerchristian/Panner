#pragma once
#include "svgPaths.h"
#include <JuceHeader.h>
#include "PluginEditor.h"

//==============================================================================
//                              Buttons
//==============================================================================


//==============================================================================
//                          Settings Button
//==============================================================================
// create own Button classes (inherited by Juce::Button) and override certain methods for custom GUI
class SettingsButton : public Button
{
private:
    
    Rectangle<float> r;
    Path pathIcon;
     

public:
    Path outline;
    
    virtual void paintButton(Graphics& g,
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
    
    explicit SettingsButton(const String& buttonName = String());
};

//==============================================================================
//                          Info Button
//==============================================================================
class InfoButton : public Button
{
private:
    Rectangle<float> r;
    Path pathIcon;
    
public:
    Path outline;
    virtual void paintButton(Graphics& g, bool schouldDrawButtonAsHighlightetd, bool shouldDrawButtonAsDown)
    {
        // paint the wheel symbol after a given SVG path file
        r = getLocalBounds().toFloat();
        pathIcon.loadPathFromData(pathDataInfo, sizeof(pathDataInfo));
        pathIcon.scaleToFit(r.getX(), r.getY(), r.getWidth(), r.getHeight(), true);
        g.setColour(Colour(BLUE));
        g.fillPath(pathIcon);
        pathIcon.clear();
        
    }
    explicit InfoButton(const String& buttonName = String());
};

class CloseButton : public Button
{
private:
    
    Rectangle<float> r;
    Path pathIcon;

public:
    Path outline;
    virtual void paintButton(Graphics& g,
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
    
    explicit CloseButton(const String& buttonName = String());
};

//==============================================================================
//                          Save Button
//==============================================================================
class SaveButton : public Button
{
private:
    
    Rectangle<float> r;
    Path pathIcon;
    
    PathStrokeType checkIconStrokeType = {3, PathStrokeType::JointStyle::curved};

    Rectangle<float> circleLeft;
    Rectangle<float> circleRight;
    Rectangle<int> midPart;
    Rectangle<int> checkPart;
    
    ChrystisLookAndFeel chrystisLookAndFeel;
    
    Path pathCircleLeft;
    Path pathCircleRight;
    Path pathMidPart;

public:
    Path outline;

    virtual void paintButton(Graphics& g,
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
    explicit SaveButton(const String& buttonName = String());
    
};

