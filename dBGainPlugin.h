/*==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================


*/
/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             GainPlugin
 version:          1.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Gain audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra,
                   juce_audio_processors_headless
 exporters:        xcode_mac, vs2022, vs2026

 moduleFlags:      JUCE_STRICT_REFCOUNTEDPOINTER=1

 type:             AudioProcessor
 mainClass:        GainProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

using namespace juce;

//==============================================================================
    class GainProcessor final : public AudioProcessor
    {
    public:
        //==============================================================================
        
        // CHANGED:
        // Converted the linear gain to a logarithmic dB scale
        GainProcessor()
            : AudioProcessor(BusesProperties().withInput("Input", AudioChannelSet::stereo()).withOutput("Output", AudioChannelSet::stereo()))
        {
            addParameter(gain = new AudioParameterFloat({"gain", 1}, "Gain (dB)", -100.0f, 12.0f, 0.0f));
        }

        //==============================================================================

        // CHANGED:
        // Set the initial value of the smoothed gain to the linear gain equivalent of the dB value.
        void prepareToPlay(double, int) override 
        {
            smoothGain.reset(getSampleRate(), 0.05);

            float initialGain = juce::Decibels::decibelsToGain(gain->get());
            smoothGain.setCurrentAndTargetValue(initialGain);
        }

        void releaseResources() override {}

        // CHANGED:
        // Process the audio buffer by applying the gain in dB and smoothing it over time.
        void processBlock(AudioBuffer<float> &buffer, MidiBuffer &) override
        {
            // Store the gain in dB and apply gain in dB instead of linear
            float currentLinearTarget = static_cast<float>(juce::Decibels::decibelsToGain(gain->get()));

            smoothGain.setTargetValue(currentLinearTarget);

            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto *channelData = buffer.getWritePointer(channel);

                // Create a copy of the smoothGain to avoid modifying the original when calculating smoothing values for each sample
                auto smoothGainCopy = smoothGain;

                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    float gainValue = smoothGainCopy.getNextValue();
                    channelData[sample] *= gainValue;

                    // Ensure gain never clips (limiter for each sample)
                    channelData[sample] = juce::jlimit(static_cast<float>(-1.0f), static_cast<float>(1.0f), channelData[sample]);
                }
            }
            smoothGain.skip(buffer.getNumSamples());
        }

        // Changed:
        // Overloaded processBlock for double precision audio buffers
        void processBlock(AudioBuffer<double> &buffer, MidiBuffer &) override
        {
            // Store the gain in dB and apply gain in dB instead of linear
            double currentLinearTarget = static_cast<double>(juce::Decibels::decibelsToGain(gain->get()));

            smoothGain.setTargetValue(currentLinearTarget);

            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto *channelData = buffer.getWritePointer(channel);

                // Create a copy of the smoothGain to avoid modifying the original when calculating smoothing values for each sample
                auto smoothGainCopy = smoothGain;

                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    double gainValue = smoothGainCopy.getNextValue();
                    channelData[sample] *= gainValue;

                    // Ensure gain never clips (limiter for each sample)
                    channelData[sample] = juce::jlimit(static_cast<double>(-1.0f), static_cast<double>(1.0f), channelData[sample]);
                }
            }
            smoothGain.skip(buffer.getNumSamples());
        }


        //==============================================================================
        AudioProcessorEditor *createEditor() override
        {
            return new GenericAudioProcessorEditor(*this);
        }
        bool hasEditor() const override { return true; }

        //==============================================================================
        const String getName() const override { return "Gain PlugIn"; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        double getTailLengthSeconds() const override { return 0; }

        //==============================================================================
        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const String getProgramName(int) override { return "None"; }
        void changeProgramName(int, const String &) override {}

        //==============================================================================
        void getStateInformation(MemoryBlock &destData) override
        {
            MemoryOutputStream(destData, true).writeFloat(*gain);
        }

        void setStateInformation(const void *data, int sizeInBytes) override
        {
            gain->setValueNotifyingHost(MemoryInputStream(data, static_cast<size_t>(sizeInBytes), false).readFloat());
        }

        //==============================================================================
        bool isBusesLayoutSupported(const BusesLayout &layouts) const override
        {
            const auto &mainInLayout = layouts.getChannelSet(true, 0);
            const auto &mainOutLayout = layouts.getChannelSet(false, 0);

            return (mainInLayout == mainOutLayout && (!mainInLayout.isDisabled()));
        }

    private:
        //==============================================================================
        AudioParameterFloat *gain;

        // CHANGED:
        // Added a smoothed value to avoid clicks when changing gain in dB
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothGain;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessor)
    };