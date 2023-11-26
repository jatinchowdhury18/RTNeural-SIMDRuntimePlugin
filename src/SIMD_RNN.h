#pragma once

#include "../modules/RTNeural/modules/json/json.hpp"
#include <memory>
#include <span>

template <bool use_avx, int input_size, int hidden_size>
class SIMD_RNN
{
public:
    SIMD_RNN();
    ~SIMD_RNN();

    SIMD_RNN (const SIMD_RNN&) = delete;
    SIMD_RNN& operator= (const SIMD_RNN&) = delete;
    SIMD_RNN (SIMD_RNN&&) noexcept = delete;
    SIMD_RNN& operator= (SIMD_RNN&&) noexcept = delete;

    void initialise (const nlohmann::json& weights_json);

    void reset();

    void process_conditioned (std::span<float> buffer, std::span<const float> condition, bool useResiduals = false) noexcept;

private:
    struct Internal;
    std::unique_ptr<Internal> internal {};
};
