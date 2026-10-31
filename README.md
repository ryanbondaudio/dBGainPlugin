# dBGainPlugin

A simple gain plugin built using the JUCE framework. It allows users to adjust audio levels in decibels, with parameter smoothing to prevent clicks and other unwanted artifacts when changing the gain.

The plugin supports both 32-bit float and 64-bit double precision processing.

## Features

- **Gain control** — Adjusts volume from -100 dB to +12 dB, converting the decibel value into a linear gain multiplier.
- **Parameter smoothing** — Uses SmoothedValue to smoothly transition between gain values, preventing clicks.
- **Stereo support** — Applies the same gain envelope to each channel to maintain consistent volume changes across mono and stereo audio.
- **Float and double precision** — Uses overloaded functions to support both 32-bit and 64-bit audio processing.

## Implementation Details

The plugin processes audio one channel at a time, iterating through each sample in the channel buffer. This keeps memory access sequential and makes the processing loop straightforward.

A separate copy of the gain smoother is used for each channel so that both channels follow the same gain ramp.

After processing all channels, the original smoother is advanced by the number of samples processed. This ensures the smoother stays in sync between audio blocks.

## Required Software

- CMake VERSION 3.22 or newer
- Ninja 1.10 or newer
- C++ 20 Compiler
- Internet access to download JUCE git repo

## Supported Platforms

- MacOS
- Windows

## Commands to Build

```bash
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

Specific Build Targets
cmake --build build --target dbGainPlugin_Standalone
cmake --build build --target dbGainPlugin_VST3
cmake --build build --target dbGainPlugin_AU

Compiled Plugin Locations
VST3- dBGainPlugin/build/dbGainPlugin_artefacts/Release/VST3
AU- dBGainPlugin/build/dbGainPlugin_artefacts/Release/AU
Standalone- dBGainPlugin/build/dbGainPlugin_artefacts/Release/Standalone

MacOS Copy Command
VST3 folder
mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"

cp -R "build/dbGainPlugin_artefacts/Release/VST3/dB Gain.vst3" \
"$HOME/Library/Audio/Plug-Ins/VST3/"

AU folder
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components"

cp -R "build/dbGainPlugin_artefacts/Release/AU/dB Gain.component" \
"$HOME/Library/Audio/Plug-Ins/Components/"

Windows Copy Command
VST3 Folder
Copy-Item `
"build\dbGainPlugin_artefacts\Release\VST3\dB Gain.vst3" `
"C:\Program Files\Common Files\VST3\" `
-Recurse -Force