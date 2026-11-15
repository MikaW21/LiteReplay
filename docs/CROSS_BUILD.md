# Included binary provenance

The included binaries were produced in this task on Linux with:

- Zig 0.13.0, target x86_64-windows-gnu, C++17 -O2.
- FFmpeg upstream tag n7.1.1 (source tar included).
- nv-codec-headers upstream tag n12.2.72.0 (source tar included).
- FFmpeg reports LGPL 2.1 or later; shared libraries, w32threads, D3D11VA,
  CUDA dynamic-loader headers, NVENC, native AAC, MOV/MP4/Matroska, file protocol.
- No external software video encoder, network protocol, decoder, ffmpeg command-line
  application, CUDA Toolkit, MSVC runtime bundle, or NVIDIA driver is shipped.
- FFmpeg x86 external assembly disabled to avoid a NASM dependency; intrinsic/C
  paths remain. The consequence for audio processing cost has not been measured.
- Only a generated ffbuild/config.mak GNU-ld-only switch
  `-Wl,--disable-auto-image-base` was removed for Zig's linker. Upstream source
  files were not patched. Debug configuration is enabled during FFmpeg configure
  to avoid another GNU executable-link workaround; linked DLLs are stripped by
  FFmpeg's normal build rules.
- The application explicitly links as Windows GUI subsystem and the probe as
  console subsystem. PE metadata/import lists are in PE_IMPORTS.json.

Reproduce on Linux:

```bash
export ZIG=/absolute/path/to/zig-0.13.0/zig
./scripts/build-cross.sh
```

The script includes a small package-info resolver for the sole header-only
ffnvcodec dependency because pkg-config was unavailable in the build environment.
It checks upstream Version and header existence, understands the version bounds
used by FFmpeg configure, and rejects unknown dependencies. It does not replace
or mock any Windows API, NVENC implementation, or codec.

Reproduction requires make, native gcc (for FFmpeg's host table generator), nm,
Python 3, and tar. Build paths should not contain spaces for FFmpeg's makefiles.
Windows/MSVC alternative: scripts/build-windows.ps1 (not executed here).

Compilation/linking and static PE import inspection succeeded. These executables
have NOT been launched on Windows, Wine, or any NVIDIA GPU in this environment.
There is no executable signing or Windows compatibility certification.
