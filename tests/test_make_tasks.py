from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


class MakeTaskTest(unittest.TestCase):
    def test_clean_preserves_reports_and_requires_no_compiler(self):
        repository = Path(__file__).resolve().parents[1]
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            shutil.copy2(repository / "Makefile", root / "Makefile")
            shutil.copytree(repository / "mk", root / "mk")
            (root / "Compiler/build").mkdir(parents=True)
            shutil.copy2(repository / "Compiler/Makefile", root / "Compiler/Makefile")
            (root / "Compiler/newplugin.so").write_bytes(b"build output")
            (root / "Compiler/build/old.o").write_bytes(b"build output")
            report = root / "artifacts/measurements/report.json"
            report.parent.mkdir(parents=True)
            report.write_text('{"retained": true}\n')
            result = subprocess.run(
                ["make", "clean", "PLUGIN_CXX=/does-not-exist/compiler"],
                cwd=root, capture_output=True, text=True, timeout=10,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stderr, "")
            self.assertFalse((root / "Compiler/newplugin.so").exists())
            self.assertFalse((root / "Compiler/build").exists())
            self.assertEqual(report.read_text(), '{"retained": true}\n')
            result = subprocess.run(
                ["make", "help", "PLUGIN_CXX=/does-not-exist/compiler"],
                cwd=root, capture_output=True, text=True, timeout=10,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stderr, "")


if __name__ == "__main__":
    unittest.main()
