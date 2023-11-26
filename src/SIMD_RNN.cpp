#include "SIMD_RNN.h"

#if __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#include <RTNeural/RTNeural.h>

#if __clang__
#pragma GCC diagnostic pop
#endif

template <bool use_avx, int input_size, int hidden_size>
struct SIMD_RNN<use_avx, input_size, hidden_size>::Internal
{
    using RecurrentLayerType = RTNEURAL_NAMESPACE::LSTMLayerT<float, input_size, hidden_size>;
    using DenseLayerType = RTNEURAL_NAMESPACE::DenseT<float, hidden_size, 1>;
    RTNEURAL_NAMESPACE::ModelT<float, input_size, 1, RecurrentLayerType, DenseLayerType> model;
};

template <bool use_avx, int input_size, int hidden_size>
SIMD_RNN<use_avx, input_size, hidden_size>::SIMD_RNN()
{
    internal = std::make_unique<Internal>();
}

template <bool use_avx, int input_size, int hidden_size>
SIMD_RNN<use_avx, input_size, hidden_size>::~SIMD_RNN() = default;

template <bool use_avx, int input_size, int hidden_size>
void SIMD_RNN<use_avx, input_size, hidden_size>::initialise (const nlohmann::json& weights_json)
{
    const auto& state_dict = weights_json.at ("state_dict");
    RTNEURAL_NAMESPACE::torch_helpers::loadLSTM<float> (state_dict, "rec.", internal->model.template get<0>());
    RTNEURAL_NAMESPACE::torch_helpers::loadDense<float> (state_dict, "lin.", internal->model.template get<1>());
}

template <bool use_avx, int input_size, int hidden_size>
void SIMD_RNN<use_avx, input_size, hidden_size>::reset()
{
    internal->model.reset();
}

template <bool use_avx, int input_size, int hidden_size>
void SIMD_RNN<use_avx, input_size, hidden_size>::process_conditioned (std::span<float> buffer, std::span<const float> condition, bool useResiduals) noexcept
{
    alignas (xsimd::default_arch::alignment()) float input_vec[xsimd::batch<float>::size] {};
    if (useResiduals)
    {
        for (size_t n = 0; n < buffer.size(); ++n)
        {
            input_vec[0] = buffer[n];
            input_vec[1] = condition[n];
            buffer[n] += internal->model.forward (input_vec);
        }
    }
    else
    {
        for (size_t n = 0; n < buffer.size(); ++n)
        {
            input_vec[0] = buffer[n];
            input_vec[1] = condition[n];
            buffer[n] = internal->model.forward (input_vec);
        }
    }
}

#if RTNEURAL_WITH_AVX
template class SIMD_RNN<true, 2, 24>;
#else
template class SIMD_RNN<false, 2, 24>;
#endif
