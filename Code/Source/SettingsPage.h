#pragma once
#include <JuceHeader.h>
#include "PluginEditor.h"
#include "Buttons.h"
//==============================================================================
//                          Settings Page
//==============================================================================


struct SettingsPage :   public Component,
                        public Timer
                        
{
    PannerAudioProcessor*    m_parent = nullptr;
    MouseListener mouseListener;
    ChrystisLookAndFeel chrystisLookAndFeel;
    
    TextEditor oscChannelTextEditor;
    TextEditor hostIPTextEditor;
    TextEditor udpPortTextEditor;
    
    std::unique_ptr <SaveButton> saveButton;
    std::unique_ptr <CloseButton> closeButton;
    
    
    Image settingsBG = ImageFileFormat::loadFrom(BinaryData::SettingsBG_png, BinaryData::SettingsBG_pngSize);
    Path pathBlueBG;
    DropShadow saveButtonShadow;
    

    std::unique_ptr<ParameterAttachment> oscChannelAttachment;
    
    
    Rectangle<int> blueBG;
    Rectangle<int> r;
    Rectangle<int> closeSection;
    Rectangle<int> settingsTitleSection;
    Rectangle<int> roomSizeTitleSection;
    Rectangle<int> roomSizeXValueSection;
    Rectangle<int> roomSizeZValueSection;
    Rectangle<int> xSection;
    Rectangle<int> zSection;
    Line<int> seperationLine;
    Rectangle<int> oscSection;
    Rectangle<int> ipSection;
    Rectangle<int> udpSection;
    Rectangle<int> saveSection;
    
    SettingsPage(PannerAudioProcessor* parent)
    {
        m_parent = parent;
           chrystisLookAndFeel.cornerRadius = 10;
           addMouseListener(&mouseListener, true);
           
           closeButton = std::make_unique<CloseButton>("");
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
                                                                              [=, this](int newValue) {oscChannelTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);                          oscChannelTextEditor.setText(std::to_string(newValue)); }, nullptr);
           oscChannelAttachment->sendInitialUpdate();
           addAndMakeVisible(oscChannelTextEditor);
               oscChannelTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
               oscChannelTextEditor.setJustification(Justification::centredRight);
               oscChannelTextEditor.setInputRestrictions( 3, "01234567890");
               oscChannelTextEditor.onTextChange = [this]
                   {
                       oscChannelAttachment->setValueAsCompleteGesture(oscChannelTextEditor.getText().getIntValue());
                   };
           addAndMakeVisible(hostIPTextEditor);
               hostIPTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
               hostIPTextEditor.setJustification(Justification::centredRight);
               hostIPTextEditor.setInputRestrictions(15, "0123456789.");
               hostIPTextEditor.setText(parent->getIP());
               hostIPTextEditor.onTextChange = [=, this]
                   {
                       //ipValue = hostIPTextEditor.getText();
                       parent->setIP(hostIPTextEditor.getText());
                   };
           addAndMakeVisible(udpPortTextEditor);
               udpPortTextEditor.setFont(chrystisLookAndFeel.GraphikSemibold);
               udpPortTextEditor.setJustification(Justification::centredRight);
               udpPortTextEditor.setInputRestrictions(4, "0123456789");
               //udpPortTextEditor.setText(std::to_string(udpValue));
               udpPortTextEditor.setText(std::to_string(parent->getUDP()));
               udpPortTextEditor.onTextChange = [=, this]
               {
                   //udpValue = udpPortTextEditor.getText().getIntValue();
                   parent->setUDP(udpPortTextEditor.getText().getIntValue());
               };
           
           saveButton = std::make_unique<SaveButton>("");
           addAndMakeVisible(saveButton.get());
           saveButton->onClick = [=, this]
           {
               parent->setSettings();
           };
           
           saveButtonShadow.colour = Colour(SHADOW);
           saveButtonShadow.offset = {0, 10};
           saveButtonShadow.radius = 40;
    }
    ~SettingsPage()
    {
        setLookAndFeel(nullptr);
    }
    
    
    virtual void paint(Graphics& g) override
    {
        setLookAndFeel(&chrystisLookAndFeel);
        settingsBG.rescaled(getLocalBounds().getWidth(), getLocalBounds().getHeight());
        parentSizeChanged();
        g.drawImageWithin(settingsBG, -10, -10, getLocalBounds().getWidth() + 20, getLocalBounds().getHeight() + 20, RectanglePlacement::stretchToFit);

        
        g.setColour(Colour(BLUE));
        g.setOpacity(0.7);
        pathBlueBG.addRoundedRectangle(blueBG.getX(), blueBG.getY(), blueBG.getWidth(), blueBG.getHeight(), 10, 10, true, true, false, false);
        g.fillPath(pathBlueBG);
        //g.fillRoundedRectangle(blueBG.toFloat(), 10);
        
        g.setColour(Colour(BG));
        
        g.setFont(chrystisLookAndFeel.GraphikBold);
        g.setFont(settingsTitleSection.getHeight());
        g.drawText("Settings", settingsTitleSection, Justification::left);
        
        g.setFont(chrystisLookAndFeel.GraphikSemibold);
        g.setFont(roomSizeTitleSection.getHeight());
        g.drawText("Room Size", roomSizeTitleSection, Justification::left);
        g.setFont(chrystisLookAndFeel.GraphikRegular);
        g.setFont(20);
        /*
        g.drawText(std::to_string(roomSizeXValue).erase(4, 6), roomSizeXValueSection, Justification::right);
        g.drawText(std::to_string(roomSizeZValue).erase(4, 6), roomSizeZValueSection, Justification::right);
        */
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
    virtual void timerCallback() override
    {
        repaint(roomSizeXValueSection);
        repaint(roomSizeZValueSection);
    }
    

    void parentSizeChanged() override
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
        oscChannelTextEditor.setBounds(oscSection.getRight() - 107, oscSection.getY(), 107, oscSection.getHeight());

        
        
        r.removeFromTop(17);
        ipSection = r.removeFromTop(40);
        hostIPTextEditor.setBounds(ipSection.getRight() - 148, ipSection.getY(), 148, ipSection.getHeight());


        
        r.removeFromTop(17);
        udpSection = r.removeFromTop(40);
        udpPortTextEditor.setBounds(udpSection.getRight() - 107, udpSection.getY(), 107, udpSection.getHeight());


        
        r.removeFromBottom(78);
        saveSection = r.removeFromBottom(60);
        saveSection.setWidth(blueBG.getWidth() - 182);
        saveSection.setCentre(blueBG.getCentreX(), saveSection.getCentreY());
        //saveSection.reduce(72, 0);
        saveButton->setBounds(saveSection);
        
        closeButton->setBounds(closeSection);
    }
    
    void mouseDown(const MouseEvent& event) override;

    
    
    
};
