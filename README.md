# A Custom WebAssembly/WASM Wrapper/Library for ZXing Cpp

This is a custom tweak of the [ZXing Cpp wasm wrapper](https://github.com/zxing-cpp/zxing-cpp/tree/master/wrappers/wasm), with cloud build via Github Actions.

## Features

- ES6 module
- Multi-symbol return (allows setting of `maxNumberOfSymbols`)
- Add `eccLevel`, `version`, `orientation`, `isMirrored` and `isInverted` to read result

## Build

You can download the latest build output from the continuous integration system from the [Actions](https://github.com/Sec-ant/zxing-wasm-build/actions/workflows/ci.yml) tab (Look for 'wasm-artifacts'). Or you can download them from the [Release](https://github.com/Sec-ant/zxing-wasm-build/releases) page.

## Acknowledgement

All thanks to [zxing-cpp/zxing-cpp](https://github.com/zxing-cpp/zxing-cpp).

## License

Apache-2.0
