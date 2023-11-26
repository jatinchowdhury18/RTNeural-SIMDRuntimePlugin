target_sources(NeuralSIMDRuntimePlugin PRIVATE
    NeuralSIMDRuntimePlugin.cpp
)

# set up double-compiled SIMD lib
add_library(simd_rnn_sse_or_arm STATIC)
add_library(simd_rnn_avx STATIC)

foreach(target IN ITEMS simd_rnn_sse_or_arm simd_rnn_avx)
    target_sources(${target} PRIVATE SIMD_RNN.cpp)
    target_link_libraries(${target} PRIVATE juce::juce_recommended_config_flags juce::juce_recommended_lto_flags warning_flags)
    target_include_directories(${target}
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../modules/RTNeural
            ${CMAKE_CURRENT_SOURCE_DIR}/../modules/RTNeural/modules/xsimd/include
    )
    target_compile_definitions(${target} PRIVATE RTNEURAL_USE_XSIMD=1)
    set_target_properties(${target} PROPERTIES
            POSITION_INDEPENDENT_CODE TRUE
            VISIBILITY_INLINES_HIDDEN TRUE
            C_VISIBILITY_PRESET hidden
            CXX_VISIBILITY_PRESET hidden
    )
endforeach()
target_compile_definitions(simd_rnn_sse_or_arm PRIVATE RTNEURAL_DEFAULT_ALIGNMENT=16 RTNEURAL_NAMESPACE=RTNeural_sse_arm)
target_compile_definitions(simd_rnn_avx PRIVATE RTNEURAL_DEFAULT_ALIGNMENT=32 RTNEURAL_NAMESPACE=RTNeural_avx RTNEURAL_WITH_AVX=1)

include(CheckCXXCompilerFlag)
if(WIN32)
    CHECK_CXX_COMPILER_FLAG("/arch:AVX" COMPILER_OPT_ARCH_AVX_SUPPORTED)
    if(COMPILER_OPT_ARCH_AVX_SUPPORTED)
        message(STATUS "Compiler supports flags: /arch:AVX")
        target_compile_options(simd_rnn_avx PRIVATE /arch:AVX)
    else()
        message(STATUS "Compiler DOES NOT supports flags: /arch:AVX")
    endif()
else()
    CHECK_CXX_COMPILER_FLAG("-mavx -mfma" COMPILER_OPT_ARCH_AVX_SUPPORTED)
    if(COMPILER_OPT_ARCH_AVX_SUPPORTED)
        message(STATUS "Compiler supports flags: -mavx -mfma")
        target_compile_options(simd_rnn_avx PRIVATE -mavx -mfma -Wno-unused-command-line-argument)
    else()
        message(STATUS "Compiler DOES NOT supports flags: -mavx -mfma")
    endif()
endif()

target_link_libraries(NeuralSIMDRuntimePlugin PRIVATE simd_rnn_sse_or_arm simd_rnn_avx)
    