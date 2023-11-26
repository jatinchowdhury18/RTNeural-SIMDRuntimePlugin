target_sources(NeuralSIMDRuntimePlugin PRIVATE
    NeuralSIMDRuntimePlugin.cpp
)

target_precompile_headers(NeuralSIMDRuntimePlugin PRIVATE pch.h)
    