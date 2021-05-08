#pragma once
#include <JuceHeader.h>
#include "XYPad.h"
#include "svgPaths.h"

//defining colour derivative
#define BG 0xffEFEFF4
#define BLUE 0xff0000FF
#define GREY 0xffDEDEDE
#define GREY2 0xffD9D9E1
#define GREY3 0xffC2C2CE
#define GREY4 0xffB3B3C1
#define BLACK 0xff1E1E34
#define SHADOW 0x40000000
#define SHAWOWVIEWBUTTON 0x30000000
#define RED 0xffF10303


//==============================================================================
//              Look And Feel
//==============================================================================
// create an own LookAndFeel and override the methods it needs to create custom GUI objects

struct ChrystisLookAndFeel : public LookAndFeel_V4
{
    
    // method to load custom fonts
    Typeface::Ptr getTypefaceForFont(const Font& f, String name) // returns a font from Binary
    {
        if (name == "GraphikBold")
        {
            static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::GraphikBold_otf, BinaryData::GraphikBold_otfSize);
            return myFont;
        }
        else if (name == "GraphikRegular")
        {
            
            static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::GraphikRegular_otf, BinaryData::GraphikRegular_otfSize);
            return myFont;
            
        }
        else if (name == "GraphikSemibold")
        {
            
            static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::GraphikSemibold_otf, BinaryData::GraphikSemibold_otfSize);
            
            return myFont;
            
        }
        else if (name == "GraphikThin")
        {
            static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::GraphikThin_otf, BinaryData::GraphikThin_otfSize);
            
            return myFont;
        }
        else
            DBG("Such Font does not exist.");
    }
    
   
    
    void drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor) override //draws the custom TextEditor (as used for panning infos)
    {
        if (textEditor.isEnabled())
        {
            g.setColour(Colour(GREY4));
            g.drawRoundedRectangle(0, 0, width, height, cornerRadius, 1);
        }
    }
    
    void fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& textEditor) override
    {
        // leave it empty so that textEditors don't have a BG
    }
    
    void drawLabel (Graphics& g, Label& label) override     // customize text labels
    {
        g.fillAll (Colour(GREY4));

        if (! label.isBeingEdited())
        {
            //auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            const Font font = getTypefaceForFont(font, "GraphikSemibold");

            g.setColour (Colour(BLUE));
            g.setFont (font);

            auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());

            g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                              jmax (1, (int) ((float) textArea.getHeight() / font.getHeight())),
                              label.getMinimumHorizontalScale());

            g.setColour(Colour(GREY4));
        }
        else if (label.isEnabled())
        {
            g.setColour(Colour(GREY4));
        }
        g.drawRoundedRectangle(label.getLocalBounds().getX(), label.getLocalBounds().getY(), label.getLocalBounds().getWidth(), label.getLocalBounds().getHeight(), 5, 1);
    }
    
    // change the scrollbar colour
    void drawScrollbar (Graphics& g, ScrollBar& scrollbar, int x, int y, int width, int height,
                                        bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override
    {
        ignoreUnused (isMouseDown);

        Rectangle<int> thumbBounds;

        if (isScrollbarVertical)
            thumbBounds = { x, thumbStartPosition, width, thumbSize };
        else
            thumbBounds = { thumbStartPosition, y, thumbSize, height };

        auto c = Colour(BG);
        g.setColour (isMouseOver ? c.brighter (0.25f) : c);
        g.fillRoundedRectangle (thumbBounds.reduced (1).toFloat(), 4.0f);
    }
    
    //assign the Fonts loade into Binary into Font variables
    Font GraphikBold = getTypefaceForFont(GraphikBold, "GraphikBold");
    Font GraphikRegular = getTypefaceForFont(GraphikRegular, "GraphikRegular");
    Font GraphikSemibold = getTypefaceForFont(GraphikSemibold, "GraphikSemibold");
    Font GraphikThin = getTypefaceForFont(GraphikThin, "GraphikThin");

    
    // set the wanted cornerRadius for TextEditors
    int cornerRadius = 5;
};


//==============================================================================
//                      Buttons
//==============================================================================

//==============================================================================
// create own Button classes (inherited by Juce::Button) and override certain methods for custom GUI
//==============================================================================


class SettingsButton : public Button
{
private:
    
    Rectangle<float> r;
    Path pathIcon;
     

public:
    Path outline;
    virtual void paintButton(Graphics& g,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown);
    explicit SettingsButton(const String& buttonName = String());
};

//==============================================================================

class InfoButton : public Button
{
private:
    Rectangle<float> r;
    Path pathIcon;
    
public:
    Path outline;
    virtual void paintButton(Graphics& g, bool schouldDrawButtonAsHighlightetd, bool shouldDrawButtonAsDown);
    explicit InfoButton(const String& buttonName = String());
};

//==============================================================================

class CloseButton : public Button
{
private:
    
    Rectangle<float> r;
    Path pathIcon;

public:
    Path outline;
    virtual void paintButton(Graphics& g,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown);
    explicit CloseButton(const String& buttonName = String());
};

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
                             bool shouldDrawButtonAsDown);
    explicit SaveButton(const String& buttonName = String());
    
};
//==============================================================================
class ViewButton : public Button
{
private:
    Rectangle<float> r;
    
public:
    Path outline;

    virtual void paintButton(Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown);
    explicit ViewButton(const String& buttonName = String());
};
/*
class ViewButtonCircle : public Button
{
private:

    Rectangle<float> r;
public:
    Path outline;

    virtual void paintButton(Graphics& g,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown);
    explicit ViewButtonCircle(const String& buttonName = String());
};
*/
//==============================================================================
//                          Settings Page
//==============================================================================


struct SettingsPage :   public Component,
                        public Timer
                        
{
    PannerAudioProcessor*    m_parent = nullptr;
    MouseListener mouseListener;
    ChrystisLookAndFeel chrystisLookAndFeel;
    Image settingsBG = ImageFileFormat::loadFrom(BinaryData::SettingsBG_png, BinaryData::SettingsBG_pngSize);
    Path pathBlueBG;
    DropShadow saveButtonShadow;
    
    std::unique_ptr <TextEditor> oscChannelTextEditor;
    std::unique_ptr <TextEditor> hostIPTextEditor;
    std::unique_ptr <TextEditor> udpPortTextEditor;
    std::unique_ptr <SaveButton> saveButton;
    std::unique_ptr <CloseButton> closeButton;
    
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
    
    virtual void paint(Graphics& g) override;
    virtual void timerCallback() override;
    
    void mouseDown(const MouseEvent& event) override;
    void parentSizeChanged() override;
    
    
    SettingsPage(PannerAudioProcessor* parent);
    ~SettingsPage();
};
/**
*/
//==============================================================================
//                          Info Page
//==============================================================================

class InfoPage : public Component
{
public:
    
    InfoPage ();
    ~InfoPage();
    
    MouseListener mouseListener;
    ChrystisLookAndFeel chrystisLookAndFeel;
    DrawableText mainDrawableText;
    Image settingsBG = ImageFileFormat::loadFrom(BinaryData::SettingsBG_png, BinaryData::SettingsBG_pngSize);
    Rectangle<int> r;
    Rectangle<int> blueBG;
    Rectangle<int> closeSection;
    Rectangle<int> viewPortSection;
    
    virtual void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;
    void parentSizeChanged() override;
    
    
        class TextContainer : public Component // the main text as a nested component-class to make it scrollable inside a viewport
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

    

    std::unique_ptr<Viewport> viewport;
    std::unique_ptr <CloseButton> closeButton;
    std::unique_ptr<TextContainer> textContainer;
};



//==============================================================================
//                          Plugin Editor
//==============================================================================

class PannerAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                    public TextEditor::Listener,
                                    public Timer
                                    

{
private:

    
    PannerAudioProcessor& audioProcessor;
    
    std::unique_ptr<XYPad> xyPad;
    std::unique_ptr<SettingsPage> settingsPage;
    std::unique_ptr<InfoPage> infoPage;

    TextEditor::Listener textEditorListener;
         
    DropShadow settingsWheelShadow;
    Point <int> shadowOffset = {0, 10};
    
    std::unique_ptr<juce::ParameterAttachment> xAttachment; // Attachment to connect the x-Value with the RangedAudioParameter in apvts in the PluginProcessor
    std::unique_ptr<juce::ParameterAttachment> zAttachment;  // Attachment to connect the Z-Value with the RangedAudioParameter in apvts in the PluginProcessor
    std::unique_ptr<juce::ParameterAttachment> circleModeAttachment;
    std::unique_ptr<juce::ParameterAttachment> radiusAttachment;
    std::unique_ptr<juce::ParameterAttachment> azimutAttachment;
    std::unique_ptr<ParameterAttachment> oscChannelAttachment;

    /*
    float lastXValue;
    float lastZValue;
    float lastRadiusValue;
    float lastAzimutValue;
    */
    
public:
    PannerAudioProcessorEditor (PannerAudioProcessor&);
    ~PannerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    virtual void textEditorTextChanged(TextEditor& editor) override;
    virtual void textEditorReturnKeyPressed(TextEditor& editor) override;
    virtual void timerCallback() override;
    void mouseDown (const juce::MouseEvent&) override;

    std::unique_ptr<SettingsButton> settingsButton;
    std::unique_ptr<InfoButton> infoButton;
    std::unique_ptr<ViewButton> viewButton;

    void updateTextBox (String s, int axis);
    /*
    void updateTextBoxToDeg(float newValue, int axis);
    void updateTextBoxFromDeg(float radius, float azimut);
    void readTextBoxDeg();
    */
    ChrystisLookAndFeel chrystisLookAndFeel;
    TextEditor xCoordinateTextEditor;
    TextEditor zCoordinateTextEditor;
    
    Rectangle<int> r;
    Rectangle<int> settingsSection;
    Rectangle<int> infoSection;
    Rectangle<int> titleSection;
    Rectangle<int> viewTitleSection;
    Rectangle<int> viewButtonSection;
    Rectangle<int> pannerSection;
    Rectangle<int> coordinatesTitleSection;
    Rectangle<int> xSection;
    Rectangle<int> zSection;
    Rectangle<int> mX;
    Rectangle<int> mZ;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PannerAudioProcessorEditor)
};

