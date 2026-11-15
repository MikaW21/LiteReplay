# UI Preview 1 — 2026-11-13

Software by Yuzi_Lj

25 portable build/run commands passed: 12 C++ programs plus 7 Python unittest cases. Windows x64 cross-build with MinGW GCC 14 POSIX passed (ui1-windows-build.log). No Windows GUI execution, visual screenshot, keyboard/DPI/accessibility runtime acceptance, GPU export or performance measurement. UI changes preserve recorder source and FFmpeg DLLs against HF2; manifests record hashes. Historical media tests below are not new UI/runtime results.

# Phase3a Hotfix 2 — 2026-11-10

Software by Yuzi_Lj

User screenshot confirms HF1 still failed in SetStartStopPosition. HF2 removes that call and uses absolute source seeking and active-timer end detection. Windows runtime and source-video reproduction remain unverified. Existing export integration evidence below is historical; export code unchanged. Current result: 12 C++ test programs and 7 Python cases passed (25 build/run commands, zero failures); Windows x64 cross-build passed. Logs accompany this package.

# Phase3a Hotfix 1 — 2026-11-10

软件由 Yuzi_Lj 制作 / Software by Yuzi_Lj

12 C++ test programs + 7 Python cases passed (25 build/run commands, 0 failures). New editor_preview_tests exercise duration mismatch, relative seek, end clamping, non-finite values and empty ranges. No Windows/MFPlay runtime or supplied failing-video reproduction; the screenshot alone does not prove a unique cause. Existing media integration results below are from Phase3a, not rerun because export code is unchanged. See phase3a-hotfix1-windows-build.log for cross-build result.

# Phase3a validation — 2026-11-09

软件由 Yuzi_Lj 制作 / Software by Yuzi_Lj

## Executed

- 11 portable C++ test programs compiled and passed, plus 7 Python unittest cases: 23 successful build/run commands. See portable-results.json.
- Editor project roundtrip, input rejection, quoting, export arguments, HEVC selection, bounded history and stream-copy constraints passed.
- Linux host FFmpeg integration: exact C++ segment arguments tested on mixed audio/no-audio inputs, trim, scale, frame rate, concatenation, volume attenuation, silence, full decoding and source/existing-output protection. editor-media-integration.log.
- Linux stream-copy integration: exact C++ arguments, unchanged output codec, complete decode, source preservation and overwrite refusal passed. editor-copy-integration.log. Arbitrary frame-accurate cut boundaries are not claimed.
- Windows x64 application and diagnostic cross-compilation passed. phase3a-windows-build.log. PE dependencies and binary SHA-256 recorded.
- Recording engine source and recording DLLs are unchanged from Phase2c. Editor CLI tools are reused byte-for-byte from audited LiteCut 0.1, with source/build/license provenance retained.

## Not executed

Windows editor GUI/layout, MFPlay preview, system dialogs, autosave filesystem recovery, child-process cancel/priority, actual NVENC H.264/HEVC export, AMD/Intel recording regression, clipboard paste, and application performance/endurance remain unverified here. Linux CPU tests do not validate Windows hardware behavior.

User reported the earlier screenshot feature works; device and coverage were unspecified. No new Windows lab result is inferred from that feedback. Phase 3a is a functional-code development preview, not completion of all Phase 3 requirements. BGM, titles, speed changes, proxy, pause/resume and full multi-track editing are not implemented.

No CPU/RAM/GPU/disk target is claimed achieved. Count all processes including ffmpeg/ffprobe and Windows codec processes during real testing. Follow docs/PHASE3.md for manual acceptance.
