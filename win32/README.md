# Win32 PPh 21 Calculator

Native Windows GUI application for calculating Indonesian Income Tax (PPh Pasal 21/26).

## Features

- **Full PPh 21/26 Calculation**: Supports permanent employees, contract workers, pensioners, and foreign taxpayers
- **TER and Old Scheme**: Choose between new TER (Tarif Efektif) or old progressive scheme
- **User-Friendly Interface**: Clean, modern UI matching openpajak.hamardikan.com color scheme
- **Detailed Breakdown**: Complete calculation breakdown with explanations
- **Export Capabilities**: Copy to clipboard, export to CSV
- **Windows NT 4.0+ Compatible**: Runs on all Windows versions from NT 4.0 to Windows 11
- **Static Linking**: Single executable with no DLL dependencies (except system DLLs)

## System Requirements

- **Operating System**: Windows NT 4.0 or later (NT 4.0, 2000, XP, Vista, 7, 8, 10, 11)
- **Memory**: 4 MB RAM minimum
- **Disk Space**: 500 KB
- **Display**: 800x600 minimum resolution

## Building from Source

### Prerequisites

- CMake 3.10 or later
- One of the following compilers:
  - **Visual Studio 2010 or later** (recommended for modern Windows)
  - **Visual C++ 6.0** (for maximum NT 4.0 compatibility)
  - **MinGW** (gcc for Windows)
  - **OpenWatcom 2.0+** (for DOS/Windows cross-compilation)

### Build Instructions

#### Visual Studio (MSVC)

```batch
# From the pphc root directory

# Create build directory
mkdir build-msvc
cd build-msvc

# Configure with CMake
cmake -G "Visual Studio 16 2019" -A x64 ^
      -DBUILD_WIN32_GUI=ON ^
      -DBUILD_SHARED_LIBS=OFF ^
      -DBUILD_STATIC_LIBS=ON ^
      ..

# Build
cmake --build . --config Release

# Executable will be at:
# build-msvc\win32\bin\Release\pphcalc.exe
```

#### MinGW

```batch
# From the pphc root directory

mkdir build-mingw
cd build-mingw

cmake -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DBUILD_WIN32_GUI=ON ^
      -DBUILD_SHARED_LIBS=OFF ^
      -DBUILD_STATIC_LIBS=ON ^
      ..

mingw32-make

# Executable will be at:
# build-mingw\win32\bin\pphcalc.exe
```

#### OpenWatcom

```batch
# From the pphc root directory

set WATCOM=C:\WATCOM
set PATH=%WATCOM%\binnt;%WATCOM%\binw;%PATH%

mkdir build-watcom
cd build-watcom

cmake -G "Watcom WMake" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DBUILD_WIN32_GUI=ON ^
      -DBUILD_SHARED_LIBS=OFF ^
      -DBUILD_STATIC_LIBS=ON ^
      ..

wmake

# Executable will be at:
# build-watcom\win32\bin\pphcalc.exe
```

## Usage

### Running the Application

1. Double-click `pphcalc.exe` to launch
2. The application will open with example data pre-filled

### Input Fields

- **Jenis Subjek**: Type of taxpayer (Pegawai tetap, Pensiunan, etc.)
- **Penghasilan Bruto**: Monthly gross income
- **Jumlah Masa**: Number of months (1-12)
- **Iuran Pensiun**: Monthly pension contribution
- **Bonus Tahunan**: Annual bonus amount
- **Status PTKP**: Tax-free allowance status (TK/0 through K/3)
  - TK = Single (Tidak Kawin)
  - K = Married (Kawin)
  - Number = dependents (0-3)
- **Zakat/Sumbangan**: Annual charitable donations
- **Skema**: Calculation scheme
  - **Skema Lama**: Old progressive tax (Pasal 17)
  - **TER**: New monthly withholding scheme
- **Kategori TER**: TER category (A, B, C) - only for TER scheme

### Buttons

- **Reset**: Clear all inputs and results
- **Gunakan Contoh**: Load example data (15M/month salary, K/0 status, TER scheme)
- **Salin**: Copy results to clipboard
- **Ekspor CSV**: Export results to CSV file
- **Cetak Bukti**: Print results (not yet implemented)

### Results Display

The right panel shows:

1. **Summary Box**:
   - Total PPh 21/26 for the year
   - Monthly TER withholding
   - December balance (final adjustment)

2. **Take-Home Pay**:
   - Annual take-home pay (after tax)
   - Monthly take-home pay

3. **Detailed Breakdown Table**:
   - Section A: Gross Income
   - Section B: Deductions (pension, job allowance)
   - Section C: Tax Base (netto, PTKP, PKP)
   - Section D: Progressive Tax Tiers (Pasal 17)
   - Section E: Payment Status (TER withholding)
   - Section F: Take-Home Pay

## Technical Details

### Architecture

- **Pure Win32 API**: No .NET, no MFC, no ATL
- **ANSI/MBCS**: Compatible with Windows NT 4.0 (no Unicode requirement)
- **Common Controls**: Uses COMCTL32 v4.70 (ListView, UpDown control)
- **Static Linking**: Links to PPHC static library (`pph_static.lib`)
- **Runtime**: Static CRT linkage (`/MT`) for standalone executable

### File Structure

```
win32/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── include/
│   ├── pphcalc.h              # Main application header
│   ├── resource.h             # Control IDs
│   └── colors.h               # Color scheme
└── src/
    ├── main.c                 # Entry point and initialization
    ├── mainwnd.c              # Main window procedure
    ├── controls.c             # Control creation
    ├── calculation.c          # PPHC integration
    ├── display.c              # Results display
    ├── formatting.c           # Money formatting
    ├── colors.c               # Color resources
    └── clipboard.c            # Clipboard and export
```

### Color Scheme

Matches openpajak.hamardikan.com:

- **Header**: Dark navy (#1E293B)
- **Active Tab**: Gold/Yellow (#EAB308)
- **Background**: Light cream (#FAF8F3)
- **Section Headers**: Light yellow (#FEF3C7)
- **Text**: Dark gray (#111827)

## Known Limitations

1. **Single Bonus**: Currently supports only one annual bonus field. For multiple bonuses in different months, use the API directly.
2. **Print Function**: Print functionality is a placeholder (not implemented).
3. **NT 4.0 Testing**: While designed for NT 4.0 compatibility, full testing on NT 4.0 not yet performed.
4. **TER Tables**: Uses simplified TER calculations. For exact DJP compliance, verify against official tables.

## License

Same as PPHC library - see LICENSE file in root directory.

## Contributing

This Win32 application is part of the PPHC project. Contributions welcome!

- Report bugs or request features via GitHub Issues
- Follow the existing coding style (ANSI C, Win32 API conventions)
- Test changes on multiple Windows versions if possible

## Credits

- UI Design inspired by: https://openpajak.hamardikan.com/pph21
- Tax Calculation Engine: PPHC Library (libpph)
- Built with: CMake, Win32 API, Common Controls

## Version History

- **1.0** (2025-01-XX): Initial release
  - PPh 21/26 calculation with TER and old scheme support
  - Clean UI matching openpajak.hamardikan.com
  - Export to clipboard and CSV
  - Windows NT 4.0+ compatibility
