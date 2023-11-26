#include "NeuralSIMDRuntimePlugin.h"

NeuralSIMDRuntimePlugin::NeuralSIMDRuntimePlugin() = default;

void NeuralSIMDRuntimePlugin::prepareToPlay ([[maybe_unused]] double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
}

void NeuralSIMDRuntimePlugin::processAudioBlock ([[maybe_unused]] juce::AudioBuffer<float>& buffer)
{
}

juce::AudioProcessorEditor* NeuralSIMDRuntimePlugin::createEditor()
{
    return new chowdsp::ParametersViewEditor { *this, state, state.params };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeuralSIMDRuntimePlugin();
}
