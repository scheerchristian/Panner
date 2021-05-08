/*
***************************************The class of the two-dimensional slider object*********************************************
 
Please note that inside this class I generally tried to call the vertical dimension "y", as this class should be a blueprint for any
two-dimensional slider application. But when it comes to the RangedParameters which hold the savable values of the plugin inside the DAW
and are also automatable by the DAW, I used "z" instead, as this is the adequate designation in wave-field-synthesis.
 */

#pragma once
#include "PluginEditor.h"
#include <JuceHeader.h>

#define _USE_MATH_DEFINES // for C++
#include <cmath>

#define _USE_MATH_DEFINES // for C
#include <math.h>

//defining colour derivatives

#define BG 0xffEFEFF4
#define BLUE 0xff0000FF
#define GREY 0xffDEDEDE
#define GREY2 0xffD9D9E1
#define GREY3 0xffC2C2CE
#define GREY4 0xffB3B3C1
#define BLACK 0xff1E1E34
#define SHADOW 0x40000000
#define RED 0xffF10303


class XYPad : public Component  //It inherits from "Component", the base class for gui-objects in juce
{
public:

    XYPad()
    {
        addMouseListener(&mListener, false);
        addAndMakeVisible (knob);
        knobShadow.colour = Colour(SHADOW);
        knobShadow.radius = 40;
        knobShadow.offset = {0, 10};

    }
    
    void paint(Graphics& g) override    //override the paint method to draw our object's background and shape
    {
        r = getLocalBounds().toFloat(); // defines r as a rect of the outer shape in float values
        if (isCircleModeOn() == false)
        {
            // draws the background
            g.setColour(Colour(GREY4));
            g.fillRoundedRectangle(r, 10);
            g.setColour(Colour(GREY3));
            g.fillEllipse(getLocalBounds().getX() - 13, getLocalBounds().getY() - 13, getLocalBounds().getWidth() + 26, getLocalBounds().getHeight() + 26);
            g.setColour(Colour(GREY2));
            g.fillEllipse(getLocalBounds().getX() + 40, getLocalBounds().getY() + 40, getLocalBounds().getWidth() - 80, getLocalBounds().getHeight() - 80);
            g.setColour(Colour(BG));
            g.fillEllipse(getLocalBounds().getX() + 100, getLocalBounds().getY() + 100, getLocalBounds().getWidth() - 200, getLocalBounds().getHeight() - 200);
        }
        else
        {
            g.setColour(Colour(GREY3));
            g.fillEllipse(r);
            g.setColour(Colour(GREY2));
            g.fillEllipse(getLocalBounds().getX() + 49, getLocalBounds().getY() + 50, getLocalBounds().getWidth() - 98, getLocalBounds().getHeight() - 100);
            g.setColour(Colour(BG));
            g.fillEllipse(getLocalBounds().getX() + 105, getLocalBounds().getY() + 105, getLocalBounds().getWidth() - 209, getLocalBounds().getHeight() - 211);
        }
        // draws the knob shadow
        shadowOrigin.addEllipse(coordinateX, coordinateY, knobWidth, knobWidth);
        knobShadow.drawForPath(g, shadowOrigin);
        shadowOrigin.clear();
    }
    
    void resized() override // called when window is resized
    {
        /*
        xAttachment->sendInitialUpdate();
        zAttachment->sendInitialUpdate();
        radiusAttachment->sendInitialUpdate();
        azimutAttachment->sendInitialUpdate();
         */
        knob.setBounds (coordinateX, coordinateY, knobWidth, knobWidth);    //repositions the knob
        repaint(); // repaint the object
    }
    
    void mouseDoubleClick(const MouseEvent& event) override
    {
        // center the knob if double clicked inside the component
        if (isCircleModeOn() == true)
        {
            radiusAttachment->setValueAsCompleteGesture(getRoomSize(1) / 2.0f);
            azimutAttachment->setValueAsCompleteGesture(0.0f);
            circleModeAttachment->setValueAsCompleteGesture(false);
            circleModeAttachment->setValueAsCompleteGesture(true);
            
        }
        else
        {
            xAttachment->setValueAsCompleteGesture(0.0f);
            zAttachment->setValueAsCompleteGesture(getRoomSize(1) / -2.0f);
            circleModeAttachment->setValueAsCompleteGesture(true);
            circleModeAttachment->setValueAsCompleteGesture(false);
        }
  
    }
    
    /*
     connects XYPad's ParameterAttachment xAttachment (which holds the value of the knobPosition in the x-dimension) with the
     RangedAudioParameter in the PluginProcessor. This method gets called in the constructor of the PluginEditor. The PluginEditor has a
     reference to the PluginProcessor and can access its Parameters through the AuioProcessorValueTreeState. Thus it passes the wanted
     Parameter into XYPad's connectXtoParameter method.
    */
    void connectXtoParameter (RangedAudioParameter& p)
    {


        xAttachment = std::make_unique<juce::ParameterAttachment>(p,
        [this](float newValue) { // callback lambda function. Called whenever parameter changes.
                /*
                                xyPad->setValue(newXValue, 0);
                                xyPad->setCoordinate(xyPad->translateValueToCoordinate(newXValue, 0), 0);
                                xyPad->resized();
                                */
                               }, nullptr);

    }
  
    // same as above for the y-dimension
    void connectYtoParameter (RangedAudioParameter& p)
    {
        zAttachment = std::make_unique<juce::ParameterAttachment>(p,
        [this](float newValue) {// callback lambda function. Called whenever parameter changes.
                /*
                                valueY = newValue; coordinateY = translateValueToCoordinate(valueY, 1);
                                resized();
                                */
                               }, nullptr);
    }
    void connectRadiusToParameter(RangedAudioParameter& p)
    {
        radiusAttachment = std::make_unique<juce::ParameterAttachment>(p,
        [this](float newValue) {// callback lambda function. Called whenever parameter changes.
               
                               }, nullptr);
                               
    }
    
    void connectAzimutToParameter(RangedAudioParameter& p)
    {
        
        azimutAttachment = std::make_unique<juce::ParameterAttachment>(p,
        [this](float newValue) {// callback lambda function. Called whenever parameter changes.
                
                               }, nullptr);
                               
    }
    void connectCircleModeToParameter(RangedAudioParameter& p)
     {
         
         circleModeAttachment = std::make_unique<juce::ParameterAttachment>(p,
         [this](float newValue) {// callback lambda function. Called whenever parameter changes.
                 
                                }, nullptr);
                                
     }
    
    // the value of the centre should be 0, 0 to display the positioning in the deviation from the centre. When drawing the button it's
    // necessary to know the coordinates with the origin being in the top left corner. This method takes a value and translates it into
    //  a coordinate. Depending on the axis.
    int translateValueToCoordinate (float value, int axis)
    {
        int coordinate = 0.0f;
        if (axis == 0) // if x-axis
        {
            float norm = (value + roomSizeX/2.0f) / roomSizeX; // norms the value between 0 an 1 depending on virtual roomSize
            coordinate = norm * getLocalBounds().getWidth() - knobWidth/2.0f; // calculates the coordinate depending on the width of the panner object
            //coordinateX = coordinate;
        }
        else if (axis == 1) // if y-axis
        {
            float norm = (value + roomSizeZ/2.0f) / roomSizeZ;
            coordinate = norm * getLocalBounds().getHeight() - knobWidth/2.0f;
            //coordinateY = coordinate;
        }
        return coordinate;
    }
    
    // takes coordinate and translates it into the corresponding value.
    float translateCoordinateToValue (int coordinate, int axis)
    {
        float value = 0.0f;
           if (axis == 0)
           {
               float norm = ((float)coordinate + knobWidth/2)/((float)getLocalBounds().getWidth()); // normalizes coordinate from 0 to 1
               value = (norm * roomSizeX) - roomSizeX / 2; // scales it on the room size
               //valueX = value;
               DBG("New X Value: " << value);
           }
           else if (axis == 1)
           {
               float norm = ((float)coordinate + knobWidth/2)/((float)getLocalBounds().getHeight());
               value = (norm * roomSizeZ) - roomSizeZ / 2;
               DBG("New Z Value: " << value);
               //valueY = value;
           }
           
           value = (float)((int)(value*100))/100;  // round to two decimal places
           return value;
    }
    /*
    
    
    float translateRadiusAndAzimutToValues(int axisToReturn, float radius, float azimut)
    {

        float rad = (azimut * M_PI) / 180;
        float x;
        float z;
        // von oben rechts im Uhrzeigersinn

        if (azimut >= 0.0f && azimut < 90.0f) // 1. Quadrant
        {
            rad = ((90 - azimut) * M_PI) / 180;
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

        else if (azimut >= 90 && azimut <= 180) // 2. Quadrant
        {
            rad = ((azimut - 90)* M_PI) / 180;
            if (azimut == 90)
            {
                x = radius;
                z = 0.0f;
            }
            else if (azimut == 180)
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
            rad = ((-azimut - 90) * M_PI) / 180;
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
            rad = ((90 + azimut) * M_PI) / 180;
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

        if (axisToReturn == 0)
            return x;
        else
            return z;
    }
    */
    float translateValueToRadiusAndAzimut(float currentX, float currentY, bool returnRadius)
    {
        float rad;
        float azimut;
        float radius;

        
            // von oben rechts entgegen dem Uhrzeigersinn

            if (currentX >= 0 && currentY < 0)  // 1. Quadrant
            {
                if (currentX == 0)
                {
                    rad = 0;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentY = sqrt(currentY * currentY);
                    rad = atan(currentX / currentY);
                }
            }
            else if (currentX < 0 && currentY <= 0) // 2. Quadrant
            {
                if (currentY == 0)
                {
                    rad = -M_PI / 2;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentY = sqrt(currentY * currentY);
                    rad = -atan(currentX / currentY);
                }
            }
            else if (currentX <= 0 && currentY > 0) // 3. Quadrant
            {
                if (currentX == 0)
                {
                    rad = M_PI;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentY = sqrt(currentY * currentY);
                    rad = -(M_PI - atan(currentX / currentY));
                }
            }
            else if (currentX > 0 && currentY >= 0)    // 4. Quadrant
            {
                if (currentY == 0)
                {
                    rad = M_PI / 2;
                }
                else
                {
                    currentX = sqrt(currentX * currentX);
                    currentY = sqrt(currentY * currentY);
                    rad = M_PI - atan(currentX / currentY);
                }
            }
    
            azimut = rad * 180 / M_PI;
            //double roundedAngle = floor(azimut * 100.0 + .5) / 100.0;
            radius = sqrt(pow(currentX, 2) + pow(currentY, 2));
            radius = floor(radius * 100.0 + .5) / 100.0;
            
            if (returnRadius == true)
                return radius;
            else
                return azimut;
    }

    // some setter and getter methods...
    
    void setCoordinate(int coordinate, int axis)
    {
        if (axis == 0)
            coordinateX = coordinate;
        if (axis == 1)
            coordinateY = coordinate;
    }
    
    int getCoordinate(int axis)
    {
        if (axis == 0)
            return coordinateX;
        else if (axis == 1)
            return coordinateY;
        
        else return 0;
    }
    
    void setValue(float value, int axis)
    {
        if (axis == 0)
            valueX = value;
        if (axis == 1)
            valueY = value;
    }
    
    float getValue(int axis)
    {
        if (axis == 0)
            return valueX;
        else if (axis == 1)
            return valueY;
        else return 0;
    }

    void setRoomSize(float value, int axis)
    {
        if (axis == 0)
            roomSizeX = value;
        if (axis == 1)
            roomSizeZ = value;
    }
    
    float getRoomSize(int axis)
    {
        if (axis == 0)
           return roomSizeX;
        if (axis == 1)
            return roomSizeZ;
    }
    void setCircleMode(bool newMode)
    {
        circleMode = newMode;
        resized();
    }
    
    bool isCircleModeOn()
    {

        return circleMode;
    }


private:
    
    // the Knob as a nested class of the XYPad
    class Knob : public juce::Component
    {
    public:
        Knob (XYPad& pad) : owner (pad) {}
        void mouseDown (const juce::MouseEvent&) override // when mouse gets pressed the ParameterAttachments begin a gesture
        {
            //owner.circleModeAttachment->setValueAsCompleteGesture(owner.isCircleModeOn());
            if (owner.isCircleModeOn() == false)
            {
                if (owner.xAttachment.get() != nullptr) owner.xAttachment->beginGesture();
                if (owner.zAttachment.get() != nullptr) owner.zAttachment->beginGesture();
            }
            else
            {
                if (owner.radiusAttachment.get() != nullptr) owner.radiusAttachment->beginGesture();
                if (owner.azimutAttachment.get() != nullptr) owner.azimutAttachment->beginGesture();
            }
        }
        
        void mouseUp (const juce::MouseEvent& event) override // when mouse gets pressed the ParameterAttachments end a gesture
        {
            owner.circleModeAttachment->setValueAsCompleteGesture(owner.isCircleModeOn());

            if (owner.isCircleModeOn() == false)
            {
                if (owner.xAttachment.get() != nullptr) owner.xAttachment->endGesture();
                if (owner.zAttachment.get() != nullptr) owner.zAttachment->endGesture();
            }
            else
            {
                if (owner.radiusAttachment.get() != nullptr) owner.radiusAttachment->endGesture();
                if (owner.azimutAttachment.get() != nullptr) owner.azimutAttachment->endGesture();
            }
            
            // work around for a juce bug
            if (owner.isCircleModeOn() == true)
            {
                owner.circleModeAttachment->setValueAsCompleteGesture(false);
                owner.circleModeAttachment->setValueAsCompleteGesture(true);
            }
            else
            {
                owner.circleModeAttachment->setValueAsCompleteGesture(true);
                owner.circleModeAttachment->setValueAsCompleteGesture(false);
            }

        }
        
        void mouseDrag (const juce::MouseEvent& event) override // called when mouse does a drag movement
        {
            //owner.circleModeAttachment->setValueAsCompleteGesture(owner.isCircleModeOn());
            if (owner.isCircleModeOn() == false)
            {
     
                if (owner.xAttachment.get() != nullptr) // safety check
                {
                    if (event.getEventRelativeTo(&owner).getPosition().getX() >= 0 && event.getEventRelativeTo(&owner).getPosition().getX() <= owner.getLocalBounds().getWidth()) // if the mouse is inside the panner object. To avoid the knob from disappearing outside the objects' borders.
                    {
                        owner.xAttachment->setValueAsPartOfGesture(owner.translateCoordinateToValue(event.getEventRelativeTo(&owner).getPosition().getX() - owner.knobWidth / 2.0f, 0)); // submit the change by setting the value as part of gesture of the ParameterAttachment
                        //owner.coordinateX = event.getEventRelativeTo(&owner).getPosition().getX() - owner.knobWidth / 2; // sets the x-coordinate to the mouse position
                        //owner.valueX = owner.translateCoordinateToValue(owner.coordinateX, 0); // translate the coordinate to value
                    }
                }

                // same as above for y-axis...
                if (owner.zAttachment.get() != nullptr)
                {
                    if (event.getEventRelativeTo(&owner).getPosition().getY() >= 0 && event.getEventRelativeTo(&owner).getPosition().getY() <= owner.getLocalBounds().getHeight())
                    {
                        owner.zAttachment->setValueAsPartOfGesture(owner.translateCoordinateToValue(event.getEventRelativeTo(&owner).getPosition().getY() - owner.knobWidth / 2.0f, 1));
                        //owner.coordinateY = event.getEventRelativeTo(&owner).getPosition().getY() - owner.knobWidth / 2;
                        //owner.valueY = owner.translateCoordinateToValue(owner.coordinateY, 1);
                    }
                }
            }
            else if (owner.isCircleModeOn() == true)
            {
                if (owner.radiusAttachment.get() != nullptr && owner.azimutAttachment.get() != nullptr)
                {
                    if (event.getEventRelativeTo(&owner).getPosition().getX() >= 0 && event.getEventRelativeTo(&owner).getPosition().getX() <= owner.getLocalBounds().getWidth() && event.getEventRelativeTo(&owner).getPosition().getY() >= 0 && event.getEventRelativeTo(&owner).getPosition().getY() <= owner.getLocalBounds().getHeight()) // if the mouse is inside the panner object. To avoid the knob from disappearing outside the objects' borders.
                    {
                        float newCoordinateX = event.getEventRelativeTo(&owner).getPosition().getX() - owner.knobWidth / 2.0f;
                        float newCoordinateY = event.getEventRelativeTo(&owner).getPosition().getY() - owner.knobWidth / 2.0f;
                        float newValueX = owner.translateCoordinateToValue(newCoordinateX, 0);
                        float newValueY = owner.translateCoordinateToValue(newCoordinateY, 1);
                        owner.radiusAttachment->setValueAsPartOfGesture(owner.translateValueToRadiusAndAzimut(newValueX, newValueY, true));
                        owner.azimutAttachment->setValueAsPartOfGesture(owner.translateValueToRadiusAndAzimut(newValueX, newValueY, false));
                    }
                }
            }
        }
        
        void paint (juce::Graphics& g) override // paints the knob
        {
            
            if (!(owner.getValue(0) > -owner.speakerArray.getWidth()/2 && owner.getValue(0) < owner.speakerArray.getWidth()/2 && owner.getValue(1) > -owner.speakerArray.getHeight()/2 && owner.getValue(1) < owner.speakerArray.getHeight()/2)) // if the knob gets moved outside the area of the speaker array it paints blue.
                g.setColour(Colour(BLUE));
            else
                g.setColour(Colour(RED));   //otherwise (focused source) it gets painted red, as a warning.
                
            knobPath.addEllipse(getLocalBounds().toFloat());
            g.fillPath(knobPath);
            knobPath.clear();
        }
        
        Path knobPath;
    private:
        XYPad& owner;
    };
    
    MouseListener mListener;
    DropShadow knobShadow;
    float valueX;
    float valueY;
    int coordinateX;
    int coordinateY;
    
    float roomSizeX;
    float roomSizeZ;

    
    Rectangle<float> speakerArray = { 0, 0, 6.4, 5.1 }; // the actual size of the WFS speaker array
   

public:
    Knob knob { *this };
    Rectangle<float> r;
    std::unique_ptr<juce::ParameterAttachment> xAttachment;
    std::unique_ptr<juce::ParameterAttachment> zAttachment;
    std::unique_ptr<juce::ParameterAttachment> radiusAttachment;
    std::unique_ptr<juce::ParameterAttachment> azimutAttachment;
    std::unique_ptr<juce::ParameterAttachment> circleModeAttachment;

    Path shadowOrigin;
    int knobWidth = 46;
    bool circleMode;
};

