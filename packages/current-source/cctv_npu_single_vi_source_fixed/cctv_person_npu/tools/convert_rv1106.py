"""Convert Rockchip-compatible COCO YOLOv5 ONNX on Ubuntu, not on the board."""
import argparse
import hashlib
import importlib.metadata
import json
from pathlib import Path
import tempfile


def calibration_paths(dataset):
    dataset = Path(dataset).resolve(strict=True)
    paths = []
    for line in dataset.read_text(encoding="utf-8-sig").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        path = Path(line)
        if not path.is_absolute():
            path = dataset.parent / path
        path = path.resolve(strict=True)
        if not path.is_file():
            raise ValueError(f"Not a calibration image: {path}")
        if any(c.isspace() for c in str(path)):
            raise ValueError(f"Toolkit dataset paths must not contain whitespace: {path}")
        paths.append(str(path))
    if not paths:
        raise ValueError("Calibration list is empty. Supply representative camera images.")
    return paths


def check_onnx(path):
    import onnx
    model = onnx.load(str(path))
    onnx.checker.check_model(model)
    model = onnx.shape_inference.infer_shapes(model)
    initializers = {item.name for item in model.graph.initializer}
    inputs = [item for item in model.graph.input if item.name not in initializers]

    def shape(item):
        return [dim.dim_value if dim.HasField("dim_value") else None
                for dim in item.type.tensor_type.shape.dim]

    if len(inputs) != 1 or shape(inputs[0]) != [1, 3, 640, 640]:
        raise ValueError("Expected fixed NCHW ONNX input [1,3,640,640]")
    shapes = [shape(item) for item in model.graph.output]
    expected_nchw = [[1, 255, n, n] for n in (80, 40, 20)]
    expected_nhwc = [[1, n, n, 255] for n in (80, 40, 20)]
    if shapes != expected_nchw and shapes != expected_nhwc:
        raise ValueError(f"Expected 3 COCO YOLOv5 4D detection heads, got {shapes}. "
                         "Use Rockchip's YOLOv5 --rknpu export, not a generic decoded/NMS ONNX.")
    # Layout cannot prove semantic compatibility. Require sigmoid head exports.
    producers = {name: node for node in model.graph.node for name in node.output}
    for item in model.graph.output:
        node = producers.get(item.name)
        while node is not None and node.op_type in ("Identity", "Transpose", "Reshape"):
            node = producers.get(node.input[0])
        if node is None or node.op_type != "Sigmoid":
            raise ValueError(f"Output {item.name} must be a sigmoid YOLOv5 head for this decoder")
    return shapes


def sha256(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--onnx", required=True, type=Path)
    parser.add_argument("--dataset", required=True, type=Path,
                        help="One calibration image per line; paths relative to this list")
    parser.add_argument("--output", required=True, type=Path,
                        help="New .rknn path; existing files are never overwritten")
    parser.add_argument("--check-only", action="store_true",
                        help="Validate ONNX and calibration paths without importing RKNN")
    args = parser.parse_args()
    source = args.onnx.resolve(strict=True)
    destination = args.output.resolve()
    if destination.suffix.lower() != ".rknn":
        parser.error("--output must end in .rknn")
    if destination.exists() or destination.with_suffix(".json").exists():
        parser.error("Output or manifest already exists; choose a new output name")
    paths = calibration_paths(args.dataset)
    shapes = check_onnx(source)
    print(f"Validated ONNX heads: {shapes}; calibration images: {len(paths)}")
    if args.check_only:
        return
    from rknn.api import RKNN
    version = importlib.metadata.version("rknn-toolkit2")
    print(f"RKNN-Toolkit2={version}; target=rv1106; INT8 calibration enabled")
    destination.parent.mkdir(parents=True, exist_ok=True)
    rknn = RKNN(verbose=True)

    def require_ok(code, step):
        if code != 0:
            raise RuntimeError(f"{step} failed: {code}")

    try:
        with tempfile.TemporaryDirectory(prefix="person_rv1106_") as temporary:
            calibration = Path(temporary) / "calibration.txt"
            calibration.write_text("\n".join(paths) + "\n", encoding="utf-8")
            require_ok(rknn.config(target_platform="rv1106", quantized_dtype="asymmetric_quantized-8",
                                   mean_values=[[0, 0, 0]],
                                   std_values=[[255, 255, 255]]), "config")
            require_ok(rknn.load_onnx(model=str(source)), "load_onnx")
            require_ok(rknn.build(do_quantization=True, dataset=str(calibration)), "INT8 build")
            require_ok(rknn.export_rknn(str(destination)), "export_rknn")
    finally:
        rknn.release()
    manifest = {
        "target_platform": "rv1106", "quantization": "INT8", "native_io_rank": 4,
        "toolkit2_version": version, "input_color": "RGB", "mean": [0, 0, 0],
        "std": [255, 255, 255], "classes": 80, "person_class_id": 0,
        "onnx_sha256": sha256(source), "rknn_sha256": sha256(destination),
        "calibration_images": len(paths), "board_verified": False,
        "note": "Conversion success does not establish runtime compatibility or accuracy. Run image test on board."
    }
    destination.with_suffix(".json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"Exported {destination}. Next: ARM build and cctv_person_image on the board.")


if __name__ == "__main__":
    main()
