#include "NeuralSIMDRuntimePlugin.h"

NeuralSIMDRuntimePlugin::NeuralSIMDRuntimePlugin ()
{
    const auto model_json = chowdsp::JSONUtils::fromBinaryData (BinaryData::fuzz_2_json, BinaryData::fuzz_2_jsonSize);

    for (auto& rnn : rnn_sse_arm)
        rnn.initialise (model_json);
    for (auto& rnn : rnn_avx)
        rnn.initialise (model_json);

    gain_smooth.setParameterHandle (state.params.gain.get());
    gain_smooth.setRampLength (0.05);
}

void NeuralSIMDRuntimePlugin::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto& rnn : rnn_sse_arm)
        rnn.reset();
    for (auto& rnn : rnn_avx)
        rnn.reset();

    const auto num_channels = getMainBusNumInputChannels();
    static constexpr auto os_factor = 2;
    const auto ds_spec = juce::dsp::ProcessSpec{ sampleRate, (uint32_t) samplesPerBlock, (uint32_t) num_channels };
    const auto os_spec = juce::dsp::ProcessSpec{ (double) os_factor * sampleRate, os_factor * (uint32_t) samplesPerBlock, (uint32_t) num_channels };
    upsampler.prepare (ds_spec, os_factor);
    downsampler.prepare (os_spec, os_factor);

    dc_blocker.prepare (ds_spec);
    dc_blocker.calcCoefs (30.0f, (float) ds_spec.sampleRate);

    volume.setRampDurationSeconds (0.05);
    volume.setGainLinear (state.params.level->getCurrentValue());
    volume.prepare (ds_spec);

    gain_smooth.prepare (os_spec.sampleRate, (int) os_spec.maximumBlockSize);
}

void NeuralSIMDRuntimePlugin::processAudioBlock (juce::AudioBuffer<float>& buffer)
{
    auto os_buffer = upsampler.process (buffer);

    gain_smooth.process (os_buffer.getNumSamples());
    const auto gain_span = std::span{ gain_smooth.getSmoothedBuffer(), (size_t) os_buffer.getNumSamples() };

    for (auto [ch, data] : chowdsp::buffer_iters::channels (os_buffer))
    {
        if (state.params.use_avx->get())
            rnn_avx[ch].process_conditioned (data, gain_span, true);
        else
            rnn_sse_arm[ch].process_conditioned (data, gain_span, true);
    }

    downsampler.process (os_buffer, buffer);

    dc_blocker.processBlock (buffer);

    volume.setGainLinear (state.params.level->getCurrentValue());
    volume.process (buffer);
}

juce::AudioProcessorEditor* NeuralSIMDRuntimePlugin::createEditor ()
{
    return new chowdsp::ParametersViewEditor{ *this, state, state.params };
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new NeuralSIMDRuntimePlugin();
}
