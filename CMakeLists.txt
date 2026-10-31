cmake_minimum_required(VERSION 3.22)
project(GainPlugin VERSION 1.0.0 LANGUAGES C CXX)

if(NOT TARGET juce::juce_core)
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(dbGainPlugin
    COMPANY_NAME "RyanBondAudio"
    PLUGIN_MANUFACTURER_CODE Rbnd
    PLUGIN_CODE Dbgp
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "dB Gain"
)

target_sources(dbGainPlugin PRIVATE
    dBGainPlugin.cpp
    dBGainPlugin.h
)

target_link_libraries(dbGainPlugin PRIVATE
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_gui_basics
)

target_compile_definitions(dbGainPlugin PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_compile_features(dbGainPlugin PRIVATE cxx_std_20)