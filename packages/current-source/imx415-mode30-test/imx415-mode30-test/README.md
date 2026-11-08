# IMX415 30fps mode selection experiment

2026-10-01. Test source only; no ARM build or board validation yet.

The VM source SHA256 matches the reviewed original (verification.json).
The first two complete entries in the four-lane supported_modes table are swapped.
Both are 3864x2192, 10-bit, NO_HDR. The existing 30fps entry with the 891M
register table now precedes the 20fps entry with the 594M register table.
No register values are invented or modified. Equal-resolution best-fit selection,
initial NO_HDR selection, and enumeration order are affected. Two-lane modes,
HDR entries, IQ, DTS, ISP clocks and NPU clocks are not changed by this patch.
This does not prove the board is running the same kernel or can sustain 30fps.

## Ubuntu VM

Extract into /home/student/imx415-mode30-test, then:

```sh
cd /home/student/imx415-mode30-test
sh apply.sh check
sh apply.sh apply
```

The script checks source hashes and creates imx415.c.before-mode30 without
overwriting a different backup. imx415-mode30.patch shows the small reviewable diff.
To undo this source change:

```sh
sh apply.sh restore
```

Source restoration does not restore an already flashed image.

## Before building / board deployment

Previous work changed NPU DTS to 420MHz and examined ISP unite preservation.
Do not build and flash blindly: this script leaves those existing edits untouched.
Compare the candidate DTB and kernel with the actual board backup. Keep NPU/ISP
settings consistent for a camera-mode experiment. Board backup is in the Windows
workspace backups/pre-npu-clock-20260930; it is not a full disk backup, and physical
recovery procedure is not yet verified. No flashing command is included here.

The active driver table has existing timing metadata differences between the
two modes; selecting the existing 30fps entry is an experiment, not a complete
sensor timing audit. The observed AE FPS vs measured FPS mismatch remains open.
Validate raw mode, sensor controls, AE_RESULT, measured frame counts, image quality,
ISP/CIF errors and thermal behavior before considering this a production change.
