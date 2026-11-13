"""Create a temporary IMX415 IQ copy for the BAY3D comparison."""

import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="IQ JSON pulled from the board")
    parser.add_argument("output_dir", type=Path, help="directory for the trial copy")
    args = parser.parse_args()

    iq = json.loads(args.source.read_text(encoding="utf-8-sig"))
    try:
        main_scene, = iq["main_scene"]
        sub_scene, = main_scene["sub_scene"]
        tnr = sub_scene["scene_isp32"]["bayertnr_v23"]
        tuning = tnr["TuningPara"]
        if tnr["Version"] != "V23_LITE" or tuning["enable"] != 1:
            raise ValueError
    except (KeyError, TypeError, ValueError):
        parser.error("expected one ISP32 scene with Bayer TNR V23 Lite enabled")

    tuning["enable"] = 0
    args.output_dir.mkdir(parents=True, exist_ok=True)
    destination = args.output_dir / args.source.name
    with destination.open("x", encoding="utf-8", newline="\n") as output:
        json.dump(iq, output, ensure_ascii=False, indent=2)
        output.write("\n")
    print(f"Created {destination} (bayertnr_v23.TuningPara.enable: 1 -> 0)")


if __name__ == "__main__":
    main()
