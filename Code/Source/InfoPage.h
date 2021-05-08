#pragma once
#include "PluginEditor.h"
#include <JuceHeader.h>

//==============================================================================
//                          Info Page
//==============================================================================

class InfoPage : public Component
{
public:
    MouseListener mouseListener;
    
    InfoPage (ChrystisLookAndFeel& chrystisLookAndFeel)
    {
        lookAndFeel = &chrystisLookAndFeel;
          setLookAndFeel(lookAndFeel);
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
    ~InfoPage()
    {
        setLookAndFeel(nullptr);
        lookAndFeel = nullptr;
    }
    
   
    
    virtual void paint(Graphics& g) override
    {
        g.drawImageWithin(settingsBG, -10, -10, getLocalBounds().getWidth() + 20, getLocalBounds().getHeight() + 40, RectanglePlacement::stretchToFit);
        g.setColour(Colour(BLUE));
        g.setOpacity(0.7);
        Path pathBlueBG;
        pathBlueBG.addRoundedRectangle(blueBG.getX(), blueBG.getY(), blueBG.getWidth(), blueBG.getHeight(), 10, 10, true, true, false, false);
        g.fillPath(pathBlueBG);
        pathBlueBG.clear();
        parentSizeChanged();
    }
   
    void parentSizeChanged() override
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
    
    void mouseDown(const MouseEvent& event) override;
    
    class TextContainer : public Component
    {
    public:
        TextContainer (ChrystisLookAndFeel& chrystisLookAndFeel)
        {
            lookAndFeel = &chrystisLookAndFeel;
            setLookAndFeel(lookAndFeel);
            mainText.setText(juce::CharPointer_UTF8 ("Dieses Plug-In steuert die Positionsdaten der \nWFS-Software \xc3\xbc""ber OSC \n(Netzwerk). \nPro Plugin l\xc3\xa4sst sic"
            "h eine \nvirtuelle Schallquelle in der \nWFS-Software steuern. \n\nDamit die \xc3\x9c""bertragung \nfunktioniert muss d"
            "er \xe2\x80\x9eOSC \nChannel\xe2\x80\x9c im Settings-Men\xc3\xbc \nmit dem der zu steuernden \nSchallquelle in der \nWFS-Software \xc3\xbc""berein"
            "stimmen. \n\nPer Default ist die IP-Adresse \ndes Rendering PCs \neingestellt, diese kann jedoch \nim Settings"
            " Men\xc3\xbc ver\xc3\xa4ndert \nwerden. \n \nDer UDP Port steht in Panner \nund WFS-Software per default \nauf 9001, was der "
            "empfohlene \nStandardport f\xc3\xbcr OSC ist. \nDamit die Positionswerte \nempf\xc3\xa4ngerseitig richtig \nangezeigt werd"
            "en, muss in \nder WFS-Software ein \nSpeaker-Setup geladen sein. \n\nDer Panner empf\xc3\xa4ngt die \n\xe2\x80\x9eRoom Size\xe2\x80\x9c per"
            " OSC aus der \nWFS-Software. Erscheint die \nWarnfarbe bedeutet dies, \ndass sich die virtuelle Quelle \nnun"
            " innerhalb des viereckigen \nLautsprecher-Arrays befindet. \nInnerhalb des Arrays wird die \nvirtuelle Que"
                                                     "lle zur \nfokussierten Quelle und ist \nnicht mehr ideal darstellbar."));
            
            creditsText.setText(juce::CharPointer_UTF8 ("Version 1.1 (Beta)\n"
                                                        "Designed by Niklas Schumacher\n"
                                                        "Developed by Christian Scheer\n"
                                                        "\n"
                                                        "\xc2\xa9 HSD 2021\n"));
        }
        ~TextContainer()
        {
            setLookAndFeel(nullptr);
            lookAndFeel = nullptr;
        };
        
        void paint(Graphics &g) override
        {

            parentResized();
            g.setFont(lookAndFeel->GraphikBold);
            g.setFont(50);
            g.setColour(Colour(BG));
            g.drawText("Info", titleSection, Justification::left, false);
            g.setFont(lookAndFeel->GraphikSemibold);
            g.setFont(25);
            g.drawText("Instructions", titleSection2, Justification::left, false);
            g.setFont(lookAndFeel->GraphikRegular);
            g.setFont(20);
            
            lookAndFeel->GraphikThin.setHeight(20);
            mainText.setJustification(Justification::topLeft);
            mainText.setFont(lookAndFeel->GraphikRegular);
            mainText.setLineSpacing(8);
            mainText.setColour(Colour(BG));
            mainText.draw(g, mainTextSection);
            
            creditsText.setJustification(Justification::centredTop);
            creditsText.setFont(lookAndFeel->GraphikRegular);
            creditsText.setLineSpacing(8);
            creditsText.setColour(Colour(BG));
            creditsText.draw(g, creditsSection);
            
            g.drawLine(seperationLine, 0.5);
            
            g.drawImageWithin(hsdLogo, logoSection.getX(), logoSection.getY(), logoSection.getWidth(), logoSection.getHeight(), RectanglePlacement::stretchToFit);
                        
            parentSizeChanged();
        };
        
        void parentResized()
        {
            r = getLocalBounds();
            titleSection = r.removeFromTop(55);
            r.removeFromTop(25);
            titleSection2 = r.removeFromTop(28);
            r.removeFromTop(45);
            mainTextSection = r.removeFromTop(1204).toFloat();
            r.removeFromTop(45);
            seperationLine.setStart(r.getX(), r.getY());
            seperationLine.setEnd(r.getRight(), r.getY());
            r.removeFromTop(45);
            creditsSection = r.removeFromTop(168.0).toFloat();
            r.removeFromTop(45);
            logoSection = r.removeFromTop(65);
            logoSection.setWidth(116);
            logoSection.setHeight(65);
            logoSection.setCentre(r.getRight() - r.getWidth()/2, logoSection.getCentreY());
        };
        
    private:
        Rectangle<int> r;
        Rectangle<int> titleSection;
        Rectangle<int> titleSection2;
        Rectangle<float> mainTextSection;
        Line<float> seperationLine;
        Rectangle<float> creditsSection;
        Rectangle<int> logoSection;
    
        AttributedString mainText;
        AttributedString creditsText;
        ChrystisLookAndFeel* lookAndFeel;
        
        Image hsdLogo = ImageFileFormat::loadFrom(BinaryData::hsdLogo_png, BinaryData::hsdLogo_pngSize);
         
    };

    Rectangle<int> blueBG;
    Rectangle<int> closeSection;
    Rectangle<int> viewPortSection;

private:

    std::unique_ptr<Viewport> viewport;
    
    std::unique_ptr <CloseButton> closeButton;
    
    std::unique_ptr<TextContainer> textContainer;
    
    ChrystisLookAndFeel* lookAndFeel;

    DrawableText mainDrawableText;
    Rectangle<int> r;
    Image settingsBG = ImageFileFormat::loadFrom(BinaryData::SettingsBG_png, BinaryData::SettingsBG_pngSize);
};
