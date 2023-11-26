# Neural SIMD Runtime Plugin

This repository contains an example audio plugin,
using [RTNeural](https://github.com/jatinchowdhury18/RTNeural)
with a SIMD architecture detected at run-time.

## How It Works

The basic idea is as follows:
- Set up a static library. The static library will be compiled twice, once with
  SSE/ARM SIMD instructions, and once with AVX instructions (if the platform supports them).
- Make sure that any SIMD-dependent definitions defined in that static library
  are contained entirely within it (i.e. no "leaking" definitions in the header file).
- Inside the library there will be some struct that contains our neural network.
  We need to have some way of choosing between some version of the struct that uses
  SSE/ARM instructions, and a version that uses AVX instructions. In this example,
  we use a template struct, but you could just use two separately-defined structs.
- In the plugin code, create an instance for each version of the neural network struct.
  In this example, we create two separate instances, but insome cases it might make sense
  to compress them using a `std::variant` or similar.
- Now we can check which instructions the host CPU supports and select which version of
  the struct to use at run-time!

## Building

To build the plugin, you must have CMake installed.

```bash
# Clone the repository
$ git clone https://github.com/jatinchowdhury18/NeuralSIMDRuntimePlugin.git
$ cd NeuralSIMDRuntimePlugin

# build with CMake
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

## License

The code in this repository is open source, and is licensed under the
BSD 3-clause license. Enjoy!
