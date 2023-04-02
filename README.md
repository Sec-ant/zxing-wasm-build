# A Custom WebAssembly/WASM Wrapper/Library for ZXing Cpp

## Build

You can download the latest build output from the continuous integration system from the [Actions](https://github.com/Sec-ant/zxing-wasm/actions/workflows/ci.yml) tab (Look for 'wasm-artifacts'). Or you can download them from the [Release](https://github.com/Sec-ant/zxing-wasm/releases) page.

## Performance

It turns out that compiling the library with the `-Os` (`MinSizeRel`) flag causes a noticible performance penalty. Here are some measurements from the demo_cam_reader (performed on Chromium 109 running on a Core i9-9980HK):

|         | `-Os` | `-Os -flto` | `-O3` | `-O3 -flto` | _Build system_      |
| ------- | ----- | ----------- | ----- | ----------- | ------------------- |
| size    | 790kB | 950kb       | 940kb | 1000kB      | _All_               |
| runtime | 320ms | 30ms        | 8ms   | 8ms         | C++17, emsdk 3.1.9  |
| runtime | 13ms  | 30ms        | 8ms   | 8ms         | C++17, emsdk 3.1.31 |
| runtime | 46ms  | 46ms        | 11ms  | 11ms        | C++20, emsdk 3.1.31 |

Conclusions:

- saving 15% of download size for the price of a 2x-4x slowdown seems like a hard sale (let alone the 40x one)...
- building in C++-20 mode brings position independent DataMatrix detection but costs 35% more time
- link time optimization (`-flto`) is not worth it and potentially even counter productive
