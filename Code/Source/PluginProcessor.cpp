#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PannerAudioProcessor::PannerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    
        apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    apvts.state = ValueTree ("Parameters");
    settings = ValueTree ("Settings");
    settings.setProperty("IP", var("10.50.24.225"), nullptr);
    settings.setProperty("UDP", var(9001), nullptr);
    settings.setProperty("RoomSizeX", var(25.0f), nullptr);
    settings.setProperty("RoomSizeZ", var(25.0f), nullptr);
    apvts.state.addChild(settings, 0, nullptr);
    
    roomSizeX.referTo(settings.getPropertyAsValue("RoomSizeX", nullptr));
    roomSizeZ.referTo(settings.getPropertyAsValue("RoomSizeZ", nullptr));
    hostURL.referTo(settings.getPropertyAsValue("IP", nullptr));
    udpPort.referTo(settings.getPropertyAsValue("UDP", nullptr));

    // OSC
    if (!sender.connect(getIP(), getUDP()))   //
    showConnectionErrorMessage ("Error: could not connect to UDP port.");
    
    connect(9002);  // connect the OSC-Receiver to a UDP-socket
    OSCReceiver::addListener(this, "/roomSizeX");    // make the OSC-Receiver listen to these AdressPatterns
    OSCReceiver::addListener(this, "/roomSizeZ");

    String s = apvts.state.createXml()->toString();
    DBG(s);

    /*
    lastXValue = 0.0f;
    lastZValue =  -roomSizeZ.getValue().toString().getFloatValue() / 2.0f;
    lastRadiusValue = roomSizeZ.getValue().toString().getFloatValue() / 2.0f;
    lastAzimutValue = 0.0f;
    
    
    lastXValue = apvts.getParameterAsValue("xValue").toString().getFloatValue();
    lastZValue = apvts.getParameterAsValue("zValue").toString().getFloatValue();
    lastRadiusValue = apvts.getParameterAsValue("radius").toString().getFloatValue();
    lastAzimutValue = apvts.getParameterAsValue("azimut").toString().getFloatValue();
    isCircleModeOn = apvts.getParameterAsValue("circleMode").toString().getIntValue();
    */
    circleModeAttachment = std::make_unique<ParameterAttachment>(*apvts.getParameter("circleMode"),
        [this](bool newValue)
        {
            isCircleModeOn = newValue;
        });
    
    xAttachment = std::make_unique<juce::ParameterAttachment>(*apvts.getParameter("xValue"),
        [this](float newXValue)
        {
            if (isCircleModeOn == false)
            {
                lastXValue = newXValue;
                sendPanningInfo(newXValue, 0);
            }
        });
        
    zAttachment = std::make_unique<juce::ParameterAttachment>(*apvts.getParameter("zValue"),
        [this](float newZValue)
        {
            if (isCircleModeOn == false)
            {
                lastZValue = newZValue;
                sendPanningInfo(newZValue, 1);
            }
        });

    radiusAttachment = std::make_unique<ParameterAttachment>(*apvts.getParameter("radius"),
        [this](float newRadius)
        {
            if (isCircleModeOn == true)
            {
                lastRadiusValue = newRadius;
                float newXValue = translateRadiusAndAzimutToValues(0, newRadius, lastAzimutValue);
                float newZValue = translateRadiusAndAzimutToValues(1, newRadius, lastAzimutValue);
                sendPanningInfo(newXValue, 0);
                sendPanningInfo(newZValue, 1);
            }
        });
    azimutAttachment = std::make_unique<ParameterAttachment>(*apvts.getParameter("azimut"),
        [this](float newAzimut)
        {
            if (isCircleModeOn == true)
            {
                lastAzimutValue = newAzimut;
                float newXValue = translateRadiusAndAzimutToValues(0, lastRadiusValue, newAzimut);
                float newZValue = translateRadiusAndAzimutToValues(1, lastRadiusValue, newAzimut);
                sendPanningInfo(newXValue, 0);
                sendPanningInfo(newZValue, 1);
            }
        });

    oscChannelAttachment = std::make_unique<ParameterAttachment>(*apvts.getParameter("oscChannel"),
        [this](int newChannel)
        {
            oscChannel = newChannel;
            oscChannelIndex = newChannel - 1; // minus 1 because the value to get back from the host ist the channel number, not the index.
        });

    circleModeAttachment->sendInitialUpdate();
    xAttachment->sendInitialUpdate();
    zAttachment->sendInitialUpdate();
    radiusAttachment->sendInitialUpdate();
    azimutAttachment->sendInitialUpdate();
    oscChannelAttachment->sendInitialUpdate();
}

PannerAudioProcessor::~PannerAudioProcessor()
{
    
}

//==============================================================================
const juce::String PannerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PannerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PannerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PannerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PannerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PannerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PannerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PannerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PannerAudioProcessor::getProgramName (int index)
{
    return {};
}

void PannerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PannerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void PannerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PannerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PannerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int sample =0; sample < buffer.getNumSamples(); sample++)
        {
            channelData[sample] = buffer.getSample(channel, sample);
        }
    }
}

//==============================================================================
bool PannerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PannerAudioProcessor::createEditor()
{
    return new PannerAudioProcessorEditor (*this);
}

//==============================================================================
void PannerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
    DBG(xml->toString());
}

void PannerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    DBG(apvts.state.createXml()->toString());
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    DBG(xmlState->toString());
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            if (xmlState->getFirstChildElement()->hasAttribute("IP"))   // check if there exists a preciously saved version of the plugin
            {
                    apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
                    
                    roomSizeX.referTo(apvts.state.getChildWithName("Settings").getPropertyAsValue("RoomSizeX", nullptr));
                    roomSizeZ.referTo(apvts.state.getChildWithName("Settings").getPropertyAsValue("RoomSizeZ", nullptr));
                    hostURL.referTo(apvts.state.getChildWithName("Settings").getPropertyAsValue("IP", nullptr));
                    udpPort.referTo(apvts.state.getChildWithName("Settings").getPropertyAsValue("UDP", nullptr));
                    
                    // OSC
                    if (!sender.connect(getIP(), getUDP()))   //
                    showConnectionErrorMessage ("Error: could not connect to UDP port.");

                    circleModeAttachment->sendInitialUpdate();
                    xAttachment->sendInitialUpdate();
                    zAttachment->sendInitialUpdate();
                    radiusAttachment->sendInitialUpdate();
                    azimutAttachment->sendInitialUpdate();
                    oscChannelAttachment->sendInitialUpdate();

                    if (apvts.getParameterAsValue("circleMode").toString().getIntValue() == true)
                    {
                        circleModeAttachment->setValueAsCompleteGesture(false);
                        circleModeAttachment->setValueAsCompleteGesture(true);
                    }
                    else
                    {
                        circleModeAttachment->setValueAsCompleteGesture(true);
                        circleModeAttachment->setValueAsCompleteGesture(false);
                    }

                    DBG(xmlState->toString());
            }
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PannerAudioProcessor();
}

void PannerAudioProcessor::setIP(String ip)
{
    hostURL.setValue(ip);
}

void PannerAudioProcessor::setUDP(int udp)
{
    udpPort.setValue(udp);
}


float PannerAudioProcessor::getRoomSize(int axis)
{
    if (axis == 0)
        return roomSizeX.toString().getFloatValue();
    else if (axis == 1)
        return roomSizeZ.toString().getFloatValue();
}

String PannerAudioProcessor::getIP()
{
    return hostURL.getValue().toString();
}

int PannerAudioProcessor::getUDP()
{
    return udpPort.getValue();
}

void PannerAudioProcessor::sendPanningInfo(float value, int axis)
{
    if (axis == 0)
    {
    OSCMessage xAxisMessage("/xPanner");    // create an OSC message that is to be sent with a certain adressPattern
    xAxisMessage.addInt32(oscChannelIndex); // add the first attribute to the message. In this case the ChannelIndex of the channel inside the WFS-Software which is supposed to be controlled with that plugin-instance
    xAxisMessage.addFloat32(value);    // add the second attribute to the message. In this case it is the actual value of the Panoroama-Slider
    if (!sender.send(xAxisMessage)) // send the message
        showConnectionErrorMessage("Error: could not send OSC message.");
    }

    else if (axis == 1)
    {
    OSCMessage zAxisMessage("/zPanner");    // create an OSC message that is to be sent with a certain adressPattern
    zAxisMessage.addInt32(oscChannelIndex); // add the first attribute to the message. In this case the ChannelIndex of the channel inside the WFS-Software which is supposed to be controlled with that plugin-instance
    zAxisMessage.addFloat32(value); // add the second attribute to the message. In this case it is the actual value of the Panoroama-Slider
    if (!sender.send(zAxisMessage)) // send the message
            showConnectionErrorMessage("Error: could not send OSC message.");
    }
    
    coordinatesChanged = true;
    
    
}

void PannerAudioProcessor::showConnectionErrorMessage(const juce::String& messageText)
{
    DBG(messageText);
}

void PannerAudioProcessor::oscMessageReceived(const OSCMessage& message)
{
    if (message.getAddressPattern().toString() == "/roomSizeX")
    {
        if (message.size() == 1 && message[0].isFloat32())
        {
            roomSizeX.setValue(message[0].getFloat32());

        }
    }
    
    else if (message.getAddressPattern().toString() == "/roomSizeZ")
    {
        if (message.size() == 1 && message[0].isFloat32())
        {
            roomSizeZ.setValue(message[0].getFloat32());
        }
    }
}

void PannerAudioProcessor::setSettings()
{
    sender.disconnect();
    if (! sender.connect (getIP(), getUDP()))   //
       showConnectionErrorMessage ("Error: could not connect to UDP port.");
    
}

AudioProcessorValueTreeState::ParameterLayout PannerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    params.push_back(std::make_unique<AudioParameterFloat>("xValue", "X", -100 / 2.0f, 100 / 2.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>("zValue", "Z", -100 / 2.0f, 100 / 2.0f, -12.5f));
    params.push_back(std::make_unique<AudioParameterInt>("oscChannel", "OSC Channel", 1, 128, 0));
    params.push_back(std::make_unique<AudioParameterBool>("circleMode", "Circle Mode", false));
    params.push_back(std::make_unique<AudioParameterFloat>("radius", "Radius", 0.0f, 100 / 2.0f, 12.5f));
    params.push_back(std::make_unique<AudioParameterFloat>("azimut", "Azimut", -180.0f, 180.0f, 0.0f));


    return {params.begin(), params.end()};
}

float PannerAudioProcessor::translateRadiusAndAzimutToValues(int axisToReturn, float radius, float azimut)
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
        rad = ((azimut - 90) * M_PI) / 180;
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

