from __future__ import annotations

from contextlib import redirect_stderr
import io
import json
import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import textwrap
import unittest
from unittest.mock import patch

from tools.static_analyzer_cc import main


FAKE_COMPILER = """\
#!/usr/bin/env python3
import json
import os
from pathlib import Path
import sys

log = Path(os.environ["FAKE_COMPILER_LOG"])
with log.open("a", encoding="utf-8") as output:
    output.write(json.dumps(sys.argv[1:]) + "\\n")
if os.environ.get("FAKE_COMPILER_SIGNAL"):
    os.kill(os.getpid(), int(os.environ["FAKE_COMPILER_SIGNAL"]))
raise SystemExit(int(os.environ.get("FAKE_COMPILER_EXIT", "0")))
"""


class StaticAnalyzerCompilerLauncherTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name) / "workspace with spaces"
        self.root.mkdir()
        self.compiler = self.root / "fake compiler"
        self.compiler.write_text(textwrap.dedent(FAKE_COMPILER), encoding="utf-8")
        self.compiler.chmod(0o755)
        self.plugin = self.root / "plugin directory" / "newplugin.so"
        self.plugin.parent.mkdir()
        self.plugin.write_bytes(b"fake plugin")
        self.report_directory = self.root / "analysis reports"
        self.log = self.root / "compiler argv.jsonl"

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def invoke(
        self,
        compiler_arguments: list[str],
        *,
        plugin_options: list[str] | None = None,
        exit_code: int = 0,
        report_directory: Path | None = None,
    ) -> int:
        wrapper_arguments = [
            "--compiler",
            str(self.compiler),
            "--plugin",
            str(self.plugin),
            "--report-dir",
            str(report_directory or self.report_directory),
        ]
        for option in plugin_options or []:
            wrapper_arguments.extend(["--plugin-option", option])
        wrapper_arguments.extend(["--", *compiler_arguments])
        environment = {
            "FAKE_COMPILER_LOG": str(self.log),
            "FAKE_COMPILER_EXIT": str(exit_code),
        }
        with patch.dict(os.environ, environment, clear=False):
            return main(wrapper_arguments)

    def calls(self) -> list[list[str]]:
        if not self.log.exists():
            return []
        return [
            json.loads(line)
            for line in self.log.read_text(encoding="utf-8").splitlines()
        ]

    @staticmethod
    def reportpath(arguments: list[str]) -> str:
        matches = [
            item.split("=", 1)[1]
            for item in arguments
            if item.startswith("-fplugin-arg-newplugin-reportpath=")
        ]
        if len(matches) != 1:
            raise AssertionError(f"expected one reportpath, got {matches!r}")
        return matches[0]

    def test_single_tu_injects_production_options_without_shell(self) -> None:
        include_directory = self.root / "include path.c"
        include_directory.mkdir()
        source = self.root / "unit file.c"
        source.write_text("int value;\n", encoding="utf-8")
        output = self.root / "object path" / "unit file.o"
        original = [
            "-I",
            str(include_directory),
            "-O2",
            "-MMD",
            "-MT",
            "-E",
            "-c",
            str(source),
            "-o",
            str(output),
        ]

        status = self.invoke(original)

        self.assertEqual(status, 0)
        call = self.calls()[0]
        self.assertEqual(call[: len(original)], original)
        self.assertIn(f"-fplugin={self.plugin.resolve()}", call)
        for option in (
            "analysispolicy=proof-only",
            "memoryleakcheck=1",
            "doublefreecheck=1",
            "useafterfree=1",
            "threadmod=1",
            "smt=1",
            "report=1",
            "reporthtml=0",
        ):
            self.assertIn(f"-fplugin-arg-newplugin-{option}", call)
        stem = Path(self.reportpath(call))
        self.assertEqual(stem.parent, self.report_directory.resolve())
        self.assertRegex(stem.name, r"^unit_file\.c-[0-9a-f]{20}$")

    def test_non_compile_driver_modes_are_transparent_passthrough(self) -> None:
        source = self.root / "query.c"
        source.write_text("int value;\n", encoding="utf-8")
        object_file = self.root / "query.o"
        passthrough_report_directory = self.root / "must not be created"
        commands = [
            ["-E", str(source)],
            ["-M", str(source)],
            ["-MM", str(source)],
            [str(object_file), "-o", str(self.root / "program")],
            ["--version"],
        ]

        for command in commands:
            with self.subTest(command=command):
                status = self.invoke(
                    command, report_directory=passthrough_report_directory
                )
                self.assertEqual(status, 0)

        self.assertEqual(self.calls(), commands)
        self.assertFalse(passthrough_report_directory.exists())

    def test_report_stem_is_deterministic_and_avoids_path_collisions(self) -> None:
        source_a = self.root / "a" / "same.c"
        source_b = self.root / "b" / "same.c"
        source_a.parent.mkdir()
        source_b.parent.mkdir()
        source_a.write_text("int a;\n", encoding="utf-8")
        source_b.write_text("int b;\n", encoding="utf-8")
        command_a = ["-c", str(source_a), "-o", str(self.root / "a.o")]
        command_b = ["-c", str(source_b), "-o", str(self.root / "b.o")]

        self.assertEqual(self.invoke(command_a), 0)
        self.assertEqual(self.invoke(command_b), 0)
        self.assertEqual(self.invoke(command_a), 0)
        calls = self.calls()
        stem_a_first = self.reportpath(calls[0])
        stem_b = self.reportpath(calls[1])
        stem_a_second = self.reportpath(calls[2])

        self.assertNotEqual(stem_a_first, stem_b)
        self.assertEqual(stem_a_first, stem_a_second)

    def test_output_path_and_analysis_options_participate_in_hash(self) -> None:
        source = self.root / "same.c"
        source.write_text("int value;\n", encoding="utf-8")

        self.assertEqual(
            self.invoke(["-c", str(source), "-o", str(self.root / "one.o")]), 0
        )
        self.assertEqual(
            self.invoke(["-c", str(source), "-o", str(self.root / "two.o")]), 0
        )
        self.assertEqual(
            self.invoke(
                ["-c", str(source), "-o", str(self.root / "one.o")],
                plugin_options=["reportcallgraph=1"],
            ),
            0,
        )
        stems = [self.reportpath(call) for call in self.calls()]

        self.assertEqual(len(set(stems)), 3)

    def test_compiler_failure_status_is_returned(self) -> None:
        source = self.root / "failure.c"
        source.write_text("int value;\n", encoding="utf-8")

        status = self.invoke(["-c", str(source)], exit_code=37)

        self.assertEqual(status, 37)
        self.assertEqual(len(self.calls()), 1)

    def test_compiler_signal_is_propagated_by_wrapper_process(self) -> None:
        source = self.root / "signal.c"
        source.write_text("int value;\n", encoding="utf-8")
        launcher = Path(__file__).parents[1] / "tools" / "static_analyzer_cc.py"
        command = [
            sys.executable,
            str(launcher),
            "--compiler",
            str(self.compiler),
            "--plugin",
            str(self.plugin),
            "--report-dir",
            str(self.report_directory),
            "--",
            "-c",
            str(source),
        ]
        environment = os.environ.copy()
        environment.update(
            {
                "FAKE_COMPILER_LOG": str(self.log),
                "FAKE_COMPILER_SIGNAL": str(signal.SIGTERM),
            }
        )

        completed = subprocess.run(command, env=environment, check=False)

        self.assertEqual(completed.returncode, -signal.SIGTERM)
        self.assertEqual(len(self.calls()), 1)

    def test_ambiguous_and_conflicting_commands_fail_before_compiler(self) -> None:
        first = self.root / "first.c"
        second = self.root / "second.c"
        first.write_text("int first;\n", encoding="utf-8")
        second.write_text("int second;\n", encoding="utf-8")
        cases = [
            (["-c", str(first), str(second)], None, "exactly one source"),
            (["@arguments.rsp"], None, "response files are not supported"),
            (
                ["-c", str(first), "-fplugin=/elsewhere/newplugin.so"],
                None,
                "already load plugin 'newplugin'",
            ),
            (
                ["-c", str(first), "-fplugin-arg-newplugin-reportpath=elsewhere"],
                None,
                "already configure plugin",
            ),
            (["-c", str(first)], ["reportpath=elsewhere"], "cannot be overridden"),
        ]

        for command, plugin_options, expected in cases:
            with self.subTest(command=command, plugin_options=plugin_options):
                if self.log.exists():
                    self.log.unlink()
                stderr = io.StringIO()
                with redirect_stderr(stderr):
                    status = self.invoke(command, plugin_options=plugin_options)
                self.assertEqual(status, 2)
                self.assertIn(expected, stderr.getvalue())
                self.assertEqual(self.calls(), [])

    def test_unrelated_gcc_plugins_are_preserved(self) -> None:
        source = self.root / "with-other-plugin.c"
        source.write_text("int value;\n", encoding="utf-8")
        original = [
            "-c",
            str(source),
            "-fplugin=/opt/compiler-plugins/check-style.so",
            "-fplugin-arg-check-style-reportpath=style.json",
        ]

        self.assertEqual(self.invoke(original), 0)

        call = self.calls()[0]
        self.assertIn("-fplugin=/opt/compiler-plugins/check-style.so", call)
        self.assertIn(
            "-fplugin-arg-check-style-reportpath=style.json", call
        )
        self.assertIn(f"-fplugin={self.plugin.resolve()}", call)

    def test_stdin_requires_explicit_language_then_is_analyzed(self) -> None:
        stderr = io.StringIO()
        with redirect_stderr(stderr):
            status = self.invoke(["-c", "-"])
        self.assertEqual(status, 2)
        self.assertIn("requires an explicit '-x <language>'", stderr.getvalue())
        self.assertEqual(self.calls(), [])

        status = self.invoke(["-x", "c", "-c", "-"])
        self.assertEqual(status, 0)
        call = self.calls()[0]
        self.assertIn("-fplugin-arg-newplugin-analysispolicy=proof-only", call)
        self.assertRegex(Path(self.reportpath(call)).name, r"^stdin-[0-9a-f]{20}$")


if __name__ == "__main__":
    unittest.main()
