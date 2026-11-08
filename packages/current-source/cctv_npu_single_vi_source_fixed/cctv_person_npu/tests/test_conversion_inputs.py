"""Host-only tests; do not pretend to run RKNN or ONNX conversion."""
import importlib.util
from pathlib import Path
import tempfile
import unittest

path = Path(__file__).resolve().parents[1] / "tools" / "convert_rv1106.py"
spec = importlib.util.spec_from_file_location("convert_rv1106", path)
convert = importlib.util.module_from_spec(spec)
spec.loader.exec_module(convert)


class CalibrationTests(unittest.TestCase):
    def test_relative_paths_resolve_against_dataset_not_working_directory(self):
        # Use a workspace path without spaces to satisfy Toolkit dataset semantics.
        root = Path(__file__).resolve().parents[1] / "build-host"
        root.mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=root) as directory:
            directory = Path(directory)
            image = directory / "person.jpg"
            image.write_bytes(b"path fixture only; not a decodable image")
            listing = directory / "images.txt"
            listing.write_text("# calibration\n\nperson.jpg\n", encoding="utf-8")
            self.assertEqual(convert.calibration_paths(listing), [str(image.resolve())])

    def test_empty_and_missing_images_fail(self):
        root = Path(__file__).resolve().parents[1] / "build-host"
        root.mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(dir=root) as directory:
            listing = Path(directory) / "images.txt"
            listing.write_text("# no images\n", encoding="utf-8")
            with self.assertRaises(ValueError):
                convert.calibration_paths(listing)
            listing.write_text("absent.jpg\n", encoding="utf-8")
            with self.assertRaises(FileNotFoundError):
                convert.calibration_paths(listing)


if __name__ == "__main__":
    unittest.main()
