#pragma once

#include <pch.h>
#include "SIMD_RNN.h"

struct Params : chowdsp::ParamHolder
{
    Params()
    {
        add (gain, level);

        if (juce::SystemStats::hasAVX() && juce::SystemStats::hasFMA3())
        {
            juce::Logger::writeToLog ("Host CPU supports AVX instructions!");
            add (use_avx);
        }
    }

    chowdsp::PercentParameter::Ptr gain {
        juce::ParameterID { "gain", 100 },
        "Gain",
        0.5f
    };

    chowdsp::PercentParameter::Ptr level {
        juce::ParameterID { "level", 100 },
        "Level",
        0.5f
    };

    chowdsp::BoolParameter::Ptr use_avx {
        juce::ParameterID { "use_avx", 100 },
        "Use AVX",
        false
    };
};

using State = chowdsp::PluginStateImpl<Params>;

class NeuralSIMDRuntimePlugin : public chowdsp::PluginBase<State>
{
public:
    NeuralSIMDRuntimePlugin();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processAudioBlock (juce::AudioBuffer<float>& buffer) override;

    juce::AudioProcessorEditor* createEditor() override;

private:
    chowdsp::SmoothedBufferValue<float> gain_smooth;

    SIMD_RNN<false, 2, 24> rnn_sse_arm[2];
    SIMD_RNN<true, 2, 24> rnn_avx[2];

    using AAFilter = chowdsp::EllipticFilter<6>;
    chowdsp::Upsampler<float, AAFilter> upsampler;
    chowdsp::Downsampler<float, AAFilter, false> downsampler;

    chowdsp::FirstOrderHPF<float> dc_blocker;
    chowdsp::Gain<float> volume;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralSIMDRuntimePlugin)
};
