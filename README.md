# PPHC - Indonesian Tax Calculator Library

[![Version](https://img.shields.io/badge/version-0.1a-blue.svg)](https://github.com/openpajak/pphc)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A portable C library for calculating Indonesian taxes (PPh 21/22/23/26, PPh Final Pasal 4(2), PPN, PPnBM) with complete calculation transparency.

## ⚠️ Warning: Early Development

**This library is currently in early alpha stage (v0.1a) and was developed through rapid prototyping ("vibe coding").**

**Known Limitations:**
- ⚠️ **Not all features have been thoroughly tested** - While the core PPh21 calculations have been validated, many edge cases and other tax types (PPh22, PPh23, PPh 4(2), PPN, PPNBM) have limited testing coverage
- ⚠️ **Not production-ready** - This library should NOT be used for actual tax filing or financial reporting without thorough validation
- ⚠️ **API may change** - Breaking changes may occur in future versions as the library matures
- ⚠️ **Regulatory accuracy** - Tax calculations are based on 2024 regulations but may not cover all scenarios or interpretations

**Use at your own risk.** This library is provided for educational and prototyping purposes. Always verify calculations with official tax regulations and consult with tax professionals for production use.

**Contributions welcome!** If you find bugs or have test cases, please open an issue or PR at https://github.com/kadcom/pphc

## Features

- **Multi-tax support**: PPh 21/26, PPh 22, PPh 23, PPh Final Pasal 4(2), PPN, PPnBM
- **Fixed-point arithmetic**: 4 decimal places (10,000 scale factor) for accurate financial calculations
- **Calculation transparency**: Full breakdown of every calculation step with month-by-month TER withholding
- **Multi-bonus support**: Flexible bonus system (THR, performance bonuses, etc.) with accurate TER calculation per month
- **Extreme portability**: Runs on DOS (OpenWatcom), Windows (MSVC/MinGW), Linux, macOS, iOS, Android, and WebAssembly
- **Zero dependencies**: Pure ANSI C with no external libraries
- **Configurable allocator**: Custom memory allocator support for bare-metal/embedded systems without malloc
- **Thread-safe**: No global state
- **Framework support**: macOS/iOS frameworks with XCFramework support
- **WebAssembly**: Run in web browsers with JavaScript/TypeScript bindings
- **Android NDK**: Full JNI bindings with Java/Kotlin support

## Quick Start

### Building (Unix/Linux/macOS)

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```

### Using the Library

```c
#include <pph/pph_calculator.h>

int main(void) {
    pph21_input_t input = {0};
    pph_result_t *result;

    pph_init();

    /* Configure calculation */
    input.subject_type = PPH21_PEGAWAI_TETAP;
    input.bruto_monthly = PPH_RUPIAH(10000000);    /* 10 million IDR/month */
    input.months_paid = 12;
    input.ptkp_status = PPH_PTKP_TK0;
    input.scheme = PPH21_SCHEME_TER;
    input.ter_category = PPH21_TER_CATEGORY_A;

    /* Calculate */
    result = pph21_calculate(&input);

    /* Use result->total_tax and result->breakdown */
    printf("Total tax: %lld\n", result->total_tax.value);

    /* Clean up */
    pph_result_free(result);
    return 0;
}
```

### Using the CLI

```bash
pphc pph21
```

### WebAssembly / Browser

```bash
# Build for WebAssembly
./build-wasm.sh Release

# Or manually with emscripten
emcmake cmake -B build-wasm -DCMAKE_BUILD_TYPE=Release
emmake make -C build-wasm
```

JavaScript usage:
```javascript
import { createPPH } from './wasm/pph-wrapper.js';

const pph = await createPPH('./build-wasm/libpph/pph.js');
console.log('Version:', pph.getVersion());

// Parse Indonesian format
const salary = pph.parseMoneyID('250.000.000');
console.log('Formatted:', pph.formatMoney(salary));

// Calculate
const bonus = pph.createMoney(100000000);
const total = pph.moneyAdd(salary, bonus);
```

See [wasm/README.md](wasm/README.md) for detailed WASM documentation.

### Android / NDK

```bash
# Set Android NDK path
export ANDROID_NDK=/path/to/android-ndk

# Build for all ABIs
./build-android.sh Release
```

Java usage:
```java
import com.openpajak.pph.*;

// Initialize
PPHCalculator.initialize();

// Create input
PPHMoney salary = PPHMoney.fromRupiah(10_000_000);
PPH21Calculator.PPH21Input input = new PPH21Calculator.PPH21Input(salary)
    .withMonthsPaid(12)
    .withPTKPStatus(PPHCalculator.PTKPStatus.TK0)
    .withScheme(PPH21Calculator.Scheme.TER)
    .withTERCategory(PPHCalculator.TERCategory.B);

// Calculate
try (PPH21Calculator.PPH21Result result = PPH21Calculator.calculate(input)) {
    PPHMoney totalTax = result.getTotalTax();
    Log.i("Tax", "Total: " + totalTax);
}
```

Kotlin DSL:
```kotlin
import com.openpajak.pph.*

val result = pph21Input(10_000_000.idr) {
    monthsPaid(12)
    ptkpStatus(PPHCalculator.PTKPStatus.TK0)
    scheme(PPH21Calculator.Scheme.TER)
    terCategory(PPHCalculator.TERCategory.B)
}.calculate()

result.use {
    println("Total tax: ${it.totalTax}")
}
```

See [android/README.md](android/README.md) for detailed Android documentation.

## Custom Memory Allocator

For extreme portability on embedded systems, bare-metal platforms, or custom memory management:

### Basic Usage

```c
#include <pph/pph_calculator.h>

// Define your custom allocator functions
void* my_malloc(size_t size);
void* my_realloc(void* ptr, size_t size);
void my_free(void* ptr);

int main(void) {
    // Set custom allocator before any calculations
    pph_init();
    pph_set_custom_allocator(my_malloc, my_realloc, my_free);

    // Calculations now use your custom allocator
    pph21_input_t input = {0};
    input.bruto_monthly = PPH_RUPIAH(10000000);
    input.scheme = PPH21_SCHEME_TER;

    pph_result_t *result = pph21_calculate(&input);
    pph_result_free(result);

    // Reset to default allocator
    pph_set_custom_allocator(NULL, NULL, NULL);

    return 0;
}
```

### Use Cases

**Embedded Systems:**
```c
// Fixed memory pool for resource-constrained devices
unsigned char memory_pool[64 * 1024];
size_t pool_offset = 0;

void* pool_malloc(size_t size) {
    if (pool_offset + size > sizeof(memory_pool)) return NULL;
    void* ptr = &memory_pool[pool_offset];
    pool_offset += (size + 7) & ~7;  // 8-byte align
    return ptr;
}
```

**Debugging/Tracking:**
```c
// Track all allocations for memory leak detection
void* tracking_malloc(size_t size) {
    void* ptr = malloc(size);
    printf("ALLOC: %p (%zu bytes)\n", ptr, size);
    return ptr;
}
```

**WebAssembly:**
```c
// Use Emscripten's memory management
pph_set_custom_allocator(
    emscripten_builtin_malloc,
    emscripten_builtin_realloc,
    emscripten_builtin_free
);
```

See `examples/custom_allocator.c` for complete examples including:
- Tracking allocator with statistics
- Fixed pool allocator
- Multiple allocator switching

### Memory Requirements

Typical memory usage per calculation:
- Result struct: ~200 bytes
- Breakdown array: ~64 rows × 400 bytes = 25 KB (initial)
- Total: ~25-30 KB per calculation

Arrays grow dynamically (doubles when full), but most calculations use <20 breakdown rows.

## Platform Support

| Platform | Compiler | Status |
|----------|----------|--------|
| Linux | GCC, Clang | ✅ Tested |
| macOS | Clang | ✅ Tested |
| iOS | Clang | ✅ Framework |
| Android | NDK (Clang) | ✅ JNI/Java/Kotlin |
| Windows | MSVC 2019+ | ✅ Tested |
| Windows | OpenWatcom 1.9+ | ✅ Tested |
| Windows | MinGW | ✅ Tested |
| DOS | OpenWatcom 1.9 | ✅ 16/32-bit |
| WebAssembly | Emscripten | ✅ Browser/Node.js |

## Build Outputs

- **Unix/Linux**: `libpph.so`, `libpph.a`, `pphc`
- **macOS**: `pph.framework`, `libpph.dylib`, `libpph.a`, `pphc`
- **iOS**: `pph.framework` (device/simulator), `pph.xcframework`
- **Android**: `libpph.so` (arm64-v8a, armeabi-v7a, x86, x86_64), `libpph.a`
- **Windows (MSVC)**: `pph.dll` (v0.1a), `pph.lib`, `pph_static.lib`, `pphc.exe`
- **Windows (OpenWatcom)**: `pph.dll` (v0.1a), `pph.lib`, `pph_s.lib`, `pphc.exe`
- **Windows (MinGW)**: `libpph.dll`, `libpph.dll.a`, `libpph.a`, `pphc.exe`
- **WebAssembly**: `pph.js`, `pph.wasm`

## Documentation

- [BUILDING.md](BUILDING.md) - Detailed platform-specific build instructions
- [API Reference](docs/API.md) - Complete API documentation (TODO)
- [Examples](examples/) - Usage examples

## Technical Details

- **Precision**: 4 decimal places (scale factor 10,000)
- **Integer type**: `__int64` (OpenWatcom/MSVC) or `int64_t` (GCC/Clang)
- **C Standard**: C99 with compiler-specific extensions for 64-bit integers
- **Memory management**: Heap allocation, caller-free pattern

## License

MIT License - see [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! This is an early release (v0.1a) and the library is still being developed.

## Support

For issues and questions: https://github.com/openpajak/pphc/issues

## Acknowledgments

Part of the OpenPajak project - open-source Indonesian tax calculators.
