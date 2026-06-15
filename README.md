--------------------------------------------------------------------------------------------------
Copyright (c) 2026 William Ashley d/b/a William Ashley Music ( http://WilliamAshley.music )
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License  (v3) 

This program is distributed in the hope that it will be useful to other audio programmers and music makers in their own plugin designs.
There is no WARRANTY expressed or implied including for MERCHANTABILITY or FITNESS FOR ANY PURPOSE. 
See the GNU General Public License for more details.

Attribution is requested where possible if you use or modify any of the source.
Notice of use is requested so I can familiarize myself with how the code has been adapted for personal interest.
contact@WilliamAshley.music   
-----------------------------------------------------------------------------------------------------

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![JUCE](https://img.shields.io/badge/Built%20with-JUCE%208.0.12-blue)](https://juce.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20-lightgrey)]()
[![Format](https://img.shields.io/badge/Format-VST3%20%7C%20-orange)]()

# Balancer Plus (GainMono Pro) VST3 Plugin
(I've prepared a Medium Article related to this plugin that may provided additional useful information on some design principles background and use: https://medium.com/@12264447666.williamashley/mid-side-processing-mono-compatibility-stereo-math-and-free-vst3-plugin-balancer2-plus-dc3065c63e57 ) 

**Version:** 2.01 (Alpha)  
**License:** GPL-3.0  

**Balancer Plus** is an advanced utility VST3 plugin designed for mixing, mastering, and spatial diagnostics. Built on the foundation of the original "Balancer", this upgrade transforms a simple gain/mono tool into a professional-grade Mid/Side matrix and Elliptical EQ utility. 

Featuring a custom "AlphaAudio" dark-themed UI, it provides precise gain staging, switchable mono summing laws, and a phase-coherent "Bass Focus" crossover to tighten up low-end for club systems and vinyl.

## Features

###  Advanced DSP & Audio Engine
- **Precision Gain Staging**: Adjustable from -24dB to +24dB with smooth, zipper-noise-free automation.
- **Mid/Side Monitoring Matrix**: Instantly switch between **Stereo**, **Mono (Mid)**, and **Solo Side** to analyze your stereo field, check mono compatibility, or isolate reverb tails and spatial effects.
- **Switchable Summing Laws**: Choose how stereo signals collapse to mono:
  - **-6dB (Voltage)**: Strict peak protection (0.5x scaling) mimicking an analog passive summer.
  - **-3dB (Power)**: Power-preserving RMS scaling (0.7071x) to maintain perceived loudness for wide, uncorrelated stereo signals.
- **Bass Focus (Elliptical EQ)**: A professional mastering technique that forces low frequencies into absolute mono while preserving the stereo width of the mid/high frequencies. 
- **Phase-Coherent Crossover**: Utilizes a unique Mid/Side subtraction method for the Bass Focus filter, ensuring zero phase cancellation or frequency "holes" at the crossover point.
- **CPU Optimized**: Features an "early exit" architecture. If bypassed or set to unity gain with no processing, it consumes virtually zero CPU.

### Custom "AlphaAudio" UI
- **Dark Charcoal & Amber-Gold Palette**: A sleek, modern aesthetic designed for low-eye-strain in dark studio environments.
- **Custom LookAndFeel (`AlphaLAF`)**: Features amber-arc rotary knobs, LED-style toggle buttons (Bypass glows red), and custom vertical linear sliders.
- **Sectioned Layout**: Clearly divided into **GAIN**, **MONITOR**, and **BASS FOCUS** panels for an intuitive workflow.

## Screenshots

![Balancer Plus VST3 Plugin UI](https://raw.githubusercontent.com/WilliamAshley2019/Balancer2plus/main/balancer2plus.png)

## Usage Guide for Mixing & Mastering

1. **Gain Staging**: Use the main vertical slider to match levels when A/B testing mono vs. stereo, or to drive the next plugin in your chain.
2. **Checking Mono Compatibility**: Switch the Monitor Mode to **Mono (Mid)**. If your vocals or bass disappear, you have phase cancellation issues that need fixing.
3. **Isolating Spatial Effects**: Switch to **Solo Side** to hear *only* the stereo differences. This is incredibly useful for checking the level of your room reverb, delay throws, or wide synths without the center-panned dry signal getting in the way.
4. **Tightening the Low-End (Bass Focus)**: Enable the **Mono Bass** toggle. This applies an Elliptical EQ, summing everything below the crossover frequency to mono. This is essential for:
   - **Club/Dance Music**: Ensuring the sub-bass hits perfectly on mono club sound systems.
   - **Vinyl Mastering**: Preventing out-of-phase low frequencies from causing the cutting stylus to jump.
   - **General Mix Cleanup**: Removing muddy, wide low-end buildup while keeping the high-end stereo image intact.

## Build Instructions

### Requirements
- **Operating System:** Windows 10/11 (Tested on Windows 11)
- **IDE:** Visual Studio 2026 (Windows) or newer. *(Note: Compiled successfully with VS 2026 as of June 2026)*
- **JUCE Framework:** Version 8.0.12
- **VST3 SDK:** Steinberg VST3 SDK 3.7 or newer

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/WilliamAshley2019/Balancer2Plus.git
   cd Balancer2plus

   Open the .jucer file in Projucer (v8.0.12).
Ensure VST3 format is enabled and the VST3 SDK path is configured in your Projucer global paths.
Save and export the project for Visual Studio.
Build the project in Visual Studio (Release x64).
Copy the generated .vst3 file to your DAW’s plugin folder:
Windows: C:\Program Files\Common Files\VST3\
Note: This plugin has been primarily tested in FL Studio 2025. If you encounter issues in another DAW (Reaper, Cubase, Ableton, etc.), please feel free to email me at contact@williamashley.music.
License and Open Source Compliance
Balancer Plus is released under the GNU General Public License v3 (GPL-3.0).
GPL Requirements
Source Code: Must remain open and accessible.
Derivative Works: Must also be licensed under GPL-3.0.
Attribution: Include this README and license file when redistributing.
No Tivoization: You may not impose restrictions preventing modification of the plugin.
Third-Party Dependencies:
JUCE Framework (GPLv3 license)
Steinberg VST3 SDK (Developers may use it to create and distribute VST3 plugins but may not redistribute the SDK itself)
Contributing
Contributions, bug reports, and pull requests are welcome! If you adapt this code for your own personal interest or plugin designs, please drop me a line. I love seeing how the community utilizes and modifies this framework.
Contact & Support
Website: http://WilliamAshley.music
GitHub: https://github.com/WilliamAshley2019
Email: contact@WilliamAshley.music
