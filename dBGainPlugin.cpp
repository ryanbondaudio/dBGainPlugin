/*
  Thin wrapper that pulls in dBGainPlugin.h and provides
  the JUCE plugin entry-point macro.
*/

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>

#include "dBGainPlugin.h"

using namespace juce;

// JUCE plugin entry point — connects GainProcessor to the host.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainProcessor();
}
