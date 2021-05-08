#pragma once

#include <JuceHeader.h>
#include <optional>

//==============================================================================
/**
*/
class PannerAudioProcessor  : public juce::AudioProcessor,
                              public OSCReceiver::ListenerWithOSCAddress<OSCReceiver::MessageLoopCallback>,
                              public OSCReceiver
                              
                    
                              


{
public:
    //==============================================================================
    PannerAudioProcessor();
    ~PannerAudioProcessor() override;
    

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================

    void setSettings();
        
    float getRoomSize(int axis);
    String getIP();
    void setIP(String ip);
    int getUDP();
    void setUDP(int udp);
    
    void sendPanningInfo(float value, int axis);
    

    bool coordinatesChanged = false;  
    //==============================================================================
    
    void showConnectionErrorMessage(const juce::String& messageText);
    void oscMessageReceived(const OSCMessage& message) override;

    float translateRadiusAndAzimutToValues(int axisToReturn, float radius, float azimut);
    //==============================================================================
   
    AudioProcessorValueTreeState apvts;
    AudioProcessorValueTreeState::ParameterLayout createParameters();

    //==============================================================================
    
    std::unique_ptr<ParameterAttachment> xAttachment;
    std::unique_ptr<ParameterAttachment> zAttachment;

    std::unique_ptr<ParameterAttachment> radiusAttachment;
    std::unique_ptr<ParameterAttachment> azimutAttachment;
    
    std::unique_ptr<ParameterAttachment> oscChannelAttachment;
    std::unique_ptr<ParameterAttachment> circleModeAttachment;



private:
    //==============================================================================
    
    OSCSender sender;
    Value roomSizeX;
    Value roomSizeZ;
    Value hostURL;
    Value udpPort;
    
    ValueTree settings;

public:
    int oscChannelIndex;
    int oscChannel;    // The OSC Channel Number   
    
    float lastXValue;
    float lastZValue;
    float lastRadiusValue;
    float lastAzimutValue;
    bool isCircleModeOn;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PannerAudioProcessor)
};

