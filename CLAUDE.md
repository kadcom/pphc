# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PPHC is a portable C library for calculating Indonesian taxes (PPh 21/22/23/26, PPh Final Pasal 4(2), PPN, PPnBM) with complete calculation transparency. The library is designed for extreme portability, running on everything from DOS (OpenWatcom) to modern platforms (Windows/Linux/macOS), mobile (Android/iOS), and WebAssembly.

**Critical**: This is an early alpha (v0.1a) library developed through rapid prototyping. Not all features are thoroughly tested, and it is NOT production-ready.

## Common Build Commands

### Standard Build (Unix/Linux/macOS)
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)  # or make -j$(sysctl -n hw.ncpu) on macOS
```

### Windows Builds

**Visual Studio:**
```batch
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

**MinGW:**
```batch
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
mingw32-make
```

**OpenWatcom:**
```batch
mkdir build && cd build
cmake -G "Watcom WMake" -DCMAKE_BUILD_TYPE=Release ..
wmake
```

### DOS (OpenWatcom)
```batch
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%WATCOM%\binw;%PATH%

mkdir build-dos && cd build-dos
cmake -G "Watcom WMake" -DCMAKE_SYSTEM_NAME=DOS -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF ..
wmake
```

**Output:**
- `libpph/pph.lib` - 28KB static library
- `cli/pphc.exe` - 33KB DOS executable (32-bit protected mode, LE format)

**Note:** DOS builds require:
- CMake 3.18+ (for DOS cross-compile support)
- OpenWatcom 2.0+
- Static library only (no DLL support on DOS)
- Tests and examples are not built for DOS (they use C99 features more extensively)
- The CLI tool (`pphc.exe`) is fully functional on DOS!

### WebAssembly
```bash
./build-wasm.sh Release
# OR manually:
emcmake cmake -B build-wasm -DCMAKE_BUILD_TYPE=Release
emmake make -C build-wasm
```

### Android
```bash
export ANDROID_NDK=/path/to/android-ndk
./build-android.sh Release
```

### CMake Build Options
```bash
cmake \
    -DBUILD_SHARED_LIBS=ON   # Build shared library (default ON)
    -DBUILD_STATIC_LIBS=ON   # Build static library (default ON)
    -DBUILD_CLI=ON           # Build pphc CLI tool (default ON)
    -DBUILD_TESTS=ON         # Build test suite (default ON)
    -DBUILD_EXAMPLES=ON      # Build examples (default ON)
    ..
```

## Running Tests

```bash
# From build directory
ctest

# Or individual tests
./libpph/tests/test_money
./libpph/tests/test_pph21

# Windows
libpph\tests\Release\test_money.exe
libpph\tests\Release\test_pph21.exe
```

## Architecture

### Core Components

**libpph/** - Core tax calculation library
- **include/pph/** - Public API headers
  - `pph_calculator.h` - Main API, all tax calculation functions
  - `pph_types.h` - Portable type definitions for 64-bit integers across compilers
  - `pph_export.h` - DLL export macros for Windows
- **src/** - Implementation files
  - `pph_money.c` - Fixed-point decimal arithmetic (4 decimal places, 10000 scale factor)
  - `pph_constants.c` - Tax brackets, PTKP values, TER tables
  - `pph_breakdown.c` - Result structures and breakdown management
  - `pph21.c` - PPh 21/26 (employee income tax)
  - `pph22.c` - PPh 22 (import/excise tax)
  - `pph23.c` - PPh 23 (service/dividend tax)
  - `pph4_2.c` - PPh Final Pasal 4(2) (construction/rental tax)
  - `ppn.c` - PPN (VAT)
  - `ppnbm.c` - PPnBM (luxury tax)
  - `pph_internal.h` - **CRITICAL**: Private internal API, NOT for public use
- **tests/** - Unit tests

**cli/** - Command-line tool (pphc executable)

**examples/** - Usage examples including custom_allocator.c

**android/** - Android JNI bindings

**wasm/** - WebAssembly JavaScript/TypeScript wrappers

### Key Design Patterns

**Fixed-Point Arithmetic**: All monetary values use `pph_money_t` with 4 decimal places (scale factor 10,000). Use `PPH_RUPIAH(amount)` macro to create money values, never manipulate `.value` directly.

**Memory Management**:
- Caller-free pattern: All `*_calculate()` functions return allocated `pph_result_t*` that must be freed with `pph_result_free()`
- Custom allocator support via `pph_set_custom_allocator()` for embedded systems
- Internal allocation uses `pph_malloc/pph_realloc/pph_free` wrappers (see pph_internal.h)

**Portability Layer**:
- `pph_types.h` provides portable 64-bit integers across OpenWatcom (__int64), MSVC (__int64), and GCC/Clang (int64_t)
- `PPH_INT64_C()` macro for portable 64-bit literals
- No external dependencies, pure ANSI C with compiler-specific extensions only for integer types

**Tax Breakdown System**: Results include detailed calculation breakdowns with typed rows (section headers, currency values, percentages, subtotals, totals). Use `pph_result_add_*()` functions from pph_internal.h when building breakdowns.

### PPh21 Calculation Details

**Two Schemes**:
1. **Scheme Lama (Old)**: Annual progressive tax (Pasal 17) applied uniformly
2. **Scheme TER (New)**: Month-by-month withholding using TER tables (months 1-11), with final adjustment in month 12

**TER Categories**: A, B, C - different rate tables based on taxpayer status

**Multi-bonus Support**: Flexible bonus system via `pph21_bonus_t` array. Each bonus has month, amount, and name. Bonuses are added to monthly income and affect TER calculation for that month.

**PTKP**: 8 status levels (TK/0 through K/3) for tax-free allowances

## Compiler-Specific Considerations

**OpenWatcom**: Uses `__int64` not `int64_t`, requires `PPH_INT64_C()` macro for literals

**MSVC**: Uses `__int64`, requires `/W3` or higher for warnings, generates `pph.dll` with version resources

**GCC/Clang**: Uses C99 `int64_t`, enable `-Wall -Wextra -pedantic` for full warnings

**Emscripten**: Special CMake configuration in `cmake/EmscriptenConfig.cmake`, exports C functions to JavaScript

## Platform-Specific Build Outputs

- **Unix/Linux**: `libpph.so`, `libpph.a`, `pphc`
- **macOS**: `pph.framework`, `libpph.dylib`, `libpph.a`, `pphc`
- **iOS**: `pph.framework` (device/simulator), `pph.xcframework`
- **Android**: `libpph.so` (arm64-v8a, armeabi-v7a, x86, x86_64)
- **Windows MSVC**: `pph.dll`, `pph.lib`, `pph_static.lib`, `pphc.exe`
- **Windows OpenWatcom**: `pph.dll`, `pph.lib`, `pph_s.lib`, `pphc.exe`
- **Windows MinGW**: `libpph.dll`, `libpph.dll.a`, `libpph.a`, `pphc.exe`
- **WebAssembly**: `pph.js`, `pph.wasm`

## DOS Compatibility Notes

The codebase has been made compatible with DOS builds using OpenWatcom. The following changes were made to avoid C99 compound literals:

1. **Static Array Initializers**: All `PPH_RUPIAH()` and `PPH_MONEY()` macros in static arrays have been replaced with explicit `{ PPH_INT64_C(value) }` initializers.

2. **Runtime Code**: Compound literals in runtime code (function arguments, assignments, return statements) have been replaced with local variables:
   ```c
   // Instead of: pph_money_min(x, PPH_RUPIAH(6000000))
   pph_money_t limit;
   limit.value = PPH_INT64_C(60000000000);
   pph_money_min(x, limit);
   ```

3. **DOS Build Limitations**:
   - Only static library builds are supported (no DLL)
   - CLI tool is fully functional
   - Tests and examples require additional porting work
   - Use `-DBUILD_SHARED_LIBS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF`

## Important Notes for Development

1. **API Boundary**: NEVER include `pph_internal.h` in public headers. It's strictly for internal implementation files only.

2. **Memory Allocation**: When adding features that allocate memory, use `pph_malloc/pph_realloc/pph_free` wrappers to respect custom allocators, NOT direct malloc/free.

3. **Fixed-Point Math**: Always use `pph_money_*` functions for arithmetic. Direct manipulation of `.value` field leads to scale factor errors.

4. **Portability**: Test changes across compilers if modifying core code. The library supports ancient compilers (OpenWatcom 1.9 for DOS) to modern ones.

5. **Error Handling**: Use `pph_set_last_error()` for error messages, return NULL from calculation functions on failure.

6. **Breakdown Building**: For tax calculations, build breakdowns progressively using `pph_result_add_section/currency/percent/subtotal/total` functions.

7. **Integer Literals**: Use `PPH_INT64_C()` macro for 64-bit literals to ensure portability (e.g., `PPH_INT64_C(10000)` not `10000LL`).

8. **Tax Regulations**: This library implements 2024 Indonesian tax regulations. Changes to tax law require updates to `pph_constants.c`.

## Testing Considerations

- Core PPh21 calculations have validation tests
- Edge cases for other tax types (PPh22, PPh23, etc.) have limited coverage
- Always run full test suite (`ctest`) after changes
- Test memory allocations with custom allocator example when modifying allocation patterns

## Documentation References

- `README.md` - Feature overview, quick start, platform support
- `BUILDING.md` - Detailed platform-specific build instructions
- `examples/` - Usage examples including custom allocator
- `wasm/README.md` - WebAssembly usage
- `android/README.md` - Android/JNI usage
