from __future__ import annotations

from contextlib import redirect_stderr, redirect_stdout
import io
import json
import os
from pathlib import Path
import signal
import subprocess
import tempfile
import unittest
from unittest.mock import Mock, patch

from tools import compare_lto_project as lto


def plugin_document() -> dict:
    return {
        "schema_version": 2,
        "tool": {"name": "gcc-static-analyzer-plugin", "version": "test"},
        "analysis": {"policy": "proof-only"},
        "source": "real-provider.o",
        "summary": {"total": 1, "SA001": 0, "SA002": 1, "SA003": 0,
                    "SA004": 0, "inconclusive_total": 2},
        "findings": [{"rule_id": "SA002", "status": "PROVEN", "location": {"file": "consumer.c", "line": 7}}],
        "inconclusive_summary": {"total": 2},
        "inconclusive": [],
    }


def sarif_document(results=()) -> dict:
    return {"version": "2.1.0", "runs": [{"results": list(results)}]}


class CommandAssemblyTest(unittest.TestCase):
    def setUp(self) -> None:
        self.compiler = Path("/opt/toolchain/gcc")
        self.sources = [Path("/source space/consumer.c"), Path("/sources/provider.c")]
        self.plugin = Path("/artifacts/plugin-snapshot/newplugin.so")

    def commands(self, mode: str, compile_args=(), link_args=()):
        return lto.build_commands(
            self.compiler, self.sources, self.plugin, Path("/artifacts") / mode,
            mode, compile_args, link_args,
        )

    def test_real_sources_separate_objects_and_link_only_analyzers(self) -> None:
        for mode in lto.MODES:
            with self.subTest(mode=mode):
                compiles, link = self.commands(mode)
                self.assertEqual(len(compiles), 2)
                for index, command in enumerate(compiles):
                    self.assertIn(str(self.sources[index]), command)
                    self.assertNotIn(str(self.sources[1 - index]), command)
                    self.assertIn("-c", command)
                    self.assertFalse(any(flag.startswith("-fplugin") or flag == "-fanalyzer" for flag in command))
                    for flag in lto.COMMON_FLAGS:
                        self.assertIn(flag, command)
                        self.assertIn(flag, link)
                    self.assertIn(command[-1], link)
                self.assertEqual(any(flag.startswith("-fplugin=") for flag in link), mode == "plugin")
                self.assertEqual("-fanalyzer" in link, mode == "gcc_fanalyzer")
                self.assertNotIn("-fwhole-program", link)
                self.assertNotIn("-fno-semantic-interposition", link)

    def test_juliet_macros_includes_and_libraries_preserve_all_modes(self) -> None:
        compile_args = ["-DINCLUDEMAIN", "-DOMITGOOD", "-I/support path", "-std=gnu11", "-pthread"]
        for mode in lto.MODES:
            compiles, link = self.commands(mode, compile_args, ["-L/library path", "-lm"])
            for argument in compile_args:
                self.assertIn(argument, compiles[0])
                self.assertIn(argument, link)
            self.assertGreater(link.index("-lm"), link.index(compiles[-1][-1]))

    def test_split_operands_and_shared_flags_allowed(self) -> None:
        args = ["-I", "include path", "-isystem", "/system", "-D", "MODE=1", "-include", "config.h", "-fPIC"]
        self.assertEqual(lto.validate_extra_args(args, "compile"), args)
        self.assertEqual(lto.validate_extra_args(["-shared", "-L", "/lib", "-l", "m"], "link"),
                         ["-shared", "-L", "/lib", "-l", "m"])

    def test_mode_output_and_pass_overrides_rejected(self) -> None:
        prohibited = (
            "-fplugin=other.so", "-fplugin-arg-newplugin-report=0", "-fanalyzer",
            "-fno-analyzer", "-fno-lto", "-flto-partition=one", "-O2", "-c",
            "-ooutput", "-o", "-S", "-E", "-r", "@response", "other.o",
            "-Wl,-o,/tmp/foreign", "-specs=other", "-B/toolchain", "-fwhole-program",
            "-fno-semantic-interposition", "-Wno-analyzer-double-free", "-w",
        )
        for phase in ("compile", "link"):
            for flag in prohibited:
                with self.subTest(phase=phase, flag=flag), self.assertRaises(lto.ConfigurationError):
                    lto.validate_extra_args([flag], phase)

    def test_missing_or_option_operands_rejected(self) -> None:
        for args in (["-I"], ["-include", "-fanalyzer"], ["-D", "@response"], ["-I", "bad\0path"]):
            with self.subTest(args=args), self.assertRaises(lto.ConfigurationError):
                lto.validate_extra_args(args, "compile")

    def test_report_options_use_snapshot_basename_and_fixed_stem(self) -> None:
        _, link = self.commands("plugin")
        for option in ("analysispolicy=proof-only", "report=1", "reporthtml=1", "useafterfree=1", "threadmod=0"):
            self.assertIn(f"-fplugin-arg-newplugin-{option}", link)
        self.assertIn("-fplugin-arg-newplugin-reportpath=/artifacts/plugin/analysis", link)


class ArtifactParsingTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.path = self.root / "diagnostics.json"

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def test_official_native_options_not_message_matching(self) -> None:
        diagnostics = [
            {"level": "warning", "ruleId": "-Wanalyzer-double-free", "message": {"text": "localized text"}},
            {"level": "warning", "ruleId": "-Wunused-variable", "message": {"text": "double-free"}},
            {"level": "note", "message": {"text": "event trace"}},
        ]
        self.path.write_text(json.dumps(sarif_document(diagnostics)) + "\n" + json.dumps(sarif_document()) + "\n", encoding="utf-8")
        result = lto.parse_native_diagnostics(self.path)
        self.assertEqual(result["status"], "OK")
        self.assertEqual(result["warning_options"], {"-Wanalyzer-double-free": 1})
        self.assertEqual(result["diagnostic_count"], 3)
        self.assertFalse(result["accuracy_scored"])

    def test_native_empty_array_is_zero_but_empty_or_mixed_output_is_unknown(self) -> None:
        empty_sarif = json.dumps(sarif_document())
        for text, status in ((empty_sarif, "OK"), ("", "UNPARSED"), (empty_sarif + "\ncollect2: link failed\n", "UNPARSED"), ("[]", "UNPARSED")):
            with self.subTest(text=text):
                self.path.write_text(text, encoding="utf-8")
                result = lto.parse_native_diagnostics(self.path)
                self.assertEqual(result["status"], status)
                self.assertEqual(result["warning_options"], {} if status == "OK" else None)

    def test_native_size_limit_returns_unparsed(self) -> None:
        self.path.write_text("[]", encoding="utf-8")
        with patch.object(lto, "MAX_REPORT_BYTES", 1):
            self.assertEqual(lto.parse_native_diagnostics(self.path)["status"], "UNPARSED")

    def test_existing_plugin_schema_preserves_findings_and_inconclusive(self) -> None:
        document = plugin_document()
        self.path.write_text(json.dumps(document), encoding="utf-8")
        self.assertEqual(lto.load_plugin_report(self.path), document)

    def test_invalid_plugin_count_or_policy_rejected(self) -> None:
        for mutation in ("count", "policy", "status", "schema", "inconclusive"):
            document = plugin_document()
            if mutation == "count":
                document["summary"]["SA002"] = 0
            elif mutation == "policy":
                document["analysis"]["policy"] = "exploratory"
            elif mutation == "status":
                document["findings"][0]["status"] = "INCONCLUSIVE"
            elif mutation == "schema":
                document["schema_version"] = 1
            else:
                document["inconclusive_summary"]["total"] = 1
            self.path.write_text(json.dumps(document), encoding="utf-8")
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                lto.load_plugin_report(self.path)


class ProcessExecutionTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def test_bounded_process_records_command_and_stream_paths(self) -> None:
        process = Mock(pid=12345)
        process.wait.return_value = 0
        with patch.object(lto.subprocess, "Popen", return_value=process) as popen, \
                patch.object(lto, "wait_process_exit", side_effect=lambda p, t: p.wait(timeout=t)):
            result = lto.run_process(["gcc", "-c", "space file.c"], self.root, self.root / "compile", 5)
        self.assertEqual(result["status"], "OK")
        self.assertEqual(result["command"], ["gcc", "-c", "space file.c"])
        self.assertTrue(popen.call_args.kwargs["start_new_session"])
        self.assertNotIn("shell", popen.call_args.kwargs)
        self.assertTrue(Path(result["stdout_path"]).is_file())
        process.wait.assert_called_once_with(timeout=5)

    def test_timeout_kills_lto_process_group_and_is_not_success(self) -> None:
        process = Mock(pid=12345)
        process.wait.side_effect = [subprocess.TimeoutExpired(["gcc"], 0.5), -signal.SIGKILL]
        with patch.object(lto.subprocess, "Popen", return_value=process), \
                patch.object(lto, "wait_process_exit", side_effect=lambda p, t: p.wait(timeout=t)), \
                patch.object(lto.os, "killpg") as kill:
            result = lto.run_process(["gcc"], self.root, self.root / "link", 0.5)
        kill.assert_called_once_with(12345, signal.SIGKILL)
        self.assertEqual(result["status"], "TIMEOUT")
        self.assertTrue(result["timed_out"])
        self.assertEqual(result["exit_code"], -signal.SIGKILL)

    def test_start_failure_and_crash_are_explicit(self) -> None:
        with patch.object(lto.subprocess, "Popen", side_effect=OSError("not executable")):
            result = lto.run_process(["gcc"], self.root, self.root / "start", 1)
        self.assertEqual(result["status"], "ERROR")
        self.assertIn("not executable", Path(result["stderr_path"]).read_text())
        process = Mock(pid=12345)
        process.wait.return_value = -signal.SIGSEGV
        with patch.object(lto.subprocess, "Popen", return_value=process), \
                patch.object(lto, "wait_process_exit", side_effect=lambda p, t: p.wait(timeout=t)):
            result = lto.run_process(["gcc"], self.root, self.root / "crash", 1)
        self.assertEqual(result["status"], "CRASH")

    def test_exit_notification_uses_blocking_wait_and_closes_descriptor(self) -> None:
        process = Mock(pid=12345)
        process.wait.return_value = 0
        selector = Mock()
        selector.select.return_value = [(object(), lto.selectors.EVENT_READ)]
        with patch.object(lto.os, "pidfd_open", return_value=71, create=True), \
                patch.object(lto.os, "close") as close, \
                patch.object(lto.selectors, "DefaultSelector") as factory:
            factory.return_value.__enter__.return_value = selector
            self.assertEqual(lto.wait_process_exit(process, 5), 0)
        selector.register.assert_called_once_with(71, lto.selectors.EVENT_READ)
        self.assertEqual(selector.select.call_count, 1)
        self.assertGreater(selector.select.call_args.args[0], 0)
        self.assertLessEqual(selector.select.call_args.args[0], 5)
        process.wait.assert_called_once_with()
        close.assert_called_once_with(71)

    def test_exit_notification_timeout_keeps_child_unreaped(self) -> None:
        process = Mock(pid=12345, args=["gcc"])
        selector = Mock()
        selector.select.return_value = []
        with patch.object(lto.os, "pidfd_open", return_value=71, create=True), \
                patch.object(lto.os, "close") as close, \
                patch.object(lto.selectors, "DefaultSelector") as factory:
            factory.return_value.__enter__.return_value = selector
            with self.assertRaises(subprocess.TimeoutExpired):
                lto.wait_process_exit(process, 0.5)
        process.wait.assert_not_called()
        close.assert_called_once_with(71)

    def test_waitid_fallback_observes_without_reaping(self) -> None:
        process = Mock(pid=12345)
        process.wait.return_value = 0
        with patch.object(lto.os, "pidfd_open", side_effect=OSError(), create=True), \
                patch.object(lto.os, "waitid") as waitid:
            self.assertEqual(lto.wait_process_exit(process, 5), 0)
        waitid.assert_called_once_with(lto.os.P_PID, 12345, lto.os.WEXITED | lto.os.WNOWAIT)
        process.wait.assert_called_once_with()

    def test_notification_errors_keep_process_under_bounded_wait(self) -> None:
        for backend in ("pidfd-selector", "waitid"):
            process = Mock(pid=12345)
            process.wait.return_value = 0
            with self.subTest(backend=backend), \
                    patch.object(lto.os, "pidfd_open", create=True) as pidfd, \
                    patch.object(lto.os, "waitid", side_effect=OSError("unsupported")), \
                    patch.object(lto.os, "close"), \
                    patch.object(lto.selectors, "DefaultSelector", side_effect=OSError("descriptor limit")):
                if backend == "waitid":
                    pidfd.side_effect = OSError("unsupported")
                else:
                    pidfd.return_value = 71
                self.assertEqual(lto.wait_process_exit(process, 5), 0)
            self.assertEqual(process.wait.call_count, 1)
            remaining = process.wait.call_args.kwargs["timeout"]
            self.assertGreater(remaining, 0)
            self.assertLessEqual(remaining, 5)


class WorkflowTest(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.compiler = self.root / "gcc"
        self.compiler.write_text("fake GCC driver\n", encoding="utf-8")
        self.compiler.chmod(0o755)
        self.plugin = self.root / "newplugin.so"
        self.plugin.write_bytes(b"verified-plugin-content")
        self.sources = [self.root / "consumer.c", self.root / "provider.c"]
        for source in self.sources:
            source.write_text("int unit;\n", encoding="utf-8")
        self.output = self.root / "output"
        self.calls = []

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def fake_process(self, command, cwd, stem, timeout):
        self.calls.append(list(command))
        stdout, stderr = stem.with_suffix(".stdout.txt"), stem.with_suffix(".stderr.txt")
        stdout.write_text("gcc (GCC) 16.2.0\n" if "--version" in command else "", encoding="utf-8")
        stderr.write_text(json.dumps(sarif_document()) + "\n" if "-fanalyzer" in command else "", encoding="utf-8")
        report_args = [item for item in command if item.startswith("-fplugin-arg-newplugin-reportpath=")]
        if report_args:
            report_stem = Path(report_args[0].split("=", 1)[1])
            report_stem.with_suffix(".json").write_text(json.dumps(plugin_document()), encoding="utf-8")
            report_stem.with_suffix(".html").write_text("<!doctype html><title>Plugin findings</title>", encoding="utf-8")
        return {"command": list(command), "cwd": str(cwd), "status": "OK", "exit_code": 0,
                "timed_out": False, "duration_ms": 10.0, "timeout_seconds": timeout,
                "stdout_path": str(stdout), "stderr_path": str(stderr)}

    def invoke(self, extra=(), process=None):
        argv = ["--cc", str(self.compiler), "--plugin", str(self.plugin), "--output", str(self.output)]
        for source in self.sources:
            argv += ["--source", str(source)]
        with patch.object(lto, "run_process", side_effect=process or self.fake_process), redirect_stdout(io.StringIO()), redirect_stderr(io.StringIO()):
            return lto.main([*argv, *extra])

    def report(self):
        return json.loads((self.output / "comparison.json").read_text(encoding="utf-8"))

    def test_schema_real_source_hashes_and_total_phase_times(self) -> None:
        self.assertEqual(self.invoke(["--compile-arg=-DINCLUDEMAIN", "--compile-arg=-DOMITGOOD"]), 0)
        report = self.report()
        self.assertEqual(report["status"], "OK")
        self.assertEqual(report["benchmark_mode"]["name"], "lto-project")
        self.assertFalse(report["benchmark_mode"]["raw_headline_eligible"])
        self.assertFalse(report["accuracy"]["scored"])
        self.assertTrue(report["input_integrity"]["verified_recorded_files"])
        self.assertEqual(report["sources"][0]["sha256"], lto.sha256_file(self.sources[0]))
        self.assertEqual(report["compiler"]["sha256"], lto.sha256_file(self.compiler))
        self.assertEqual(report["plugin"]["sha256"], lto.sha256_file(self.plugin))
        self.assertNotEqual(report["plugin"]["original_path"], report["plugin"]["snapshot_path"])
        for result in report["modes"].values():
            self.assertEqual(result["compile_duration_ms"], 20)
            self.assertEqual(result["link_duration_ms"], 10)
            self.assertEqual(result["total_compile_link_ms"], 30)
            self.assertTrue(result["timing_complete"])
        self.assertEqual(len(self.calls), 10)
        self.assertFalse(any(command[0].endswith("project-output") for command in self.calls))
        rendered = (self.output / "comparison.html").read_text(encoding="utf-8")
        self.assertIn("No accuracy oracle", rendered)
        self.assertIn("plugin/analysis.html", rendered)
        self.assertIn("comparison.json", rendered)

    def test_compile_failure_skips_that_modes_link_and_records_partial_timing(self) -> None:
        def fail_first(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if stem.parent.name == "baseline" and "-c" in command:
                result.update(status="ERROR", exit_code=1)
            return result
        self.assertEqual(self.invoke(process=fail_first), 1)
        report = self.report()
        self.assertEqual(report["status"], "INCOMPLETE")
        baseline = report["modes"]["baseline"]
        self.assertEqual(baseline["sources_completed"], 0)
        self.assertIsNone(baseline["link"])
        self.assertFalse(baseline["timing_complete"])
        self.assertEqual(baseline["total_compile_link_ms"], 10)
        self.assertEqual(report["modes"]["plugin"]["status"], "OK")

    def test_link_timeout_remains_explicit_with_other_modes_preserved(self) -> None:
        def timeout_native(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if "-fanalyzer" in command:
                result.update(status="TIMEOUT", exit_code=-9, timed_out=True)
            return result
        self.assertEqual(self.invoke(process=timeout_native), 1)
        result = self.report()["modes"]["gcc_fanalyzer"]
        self.assertEqual(result["status"], "TIMEOUT")
        self.assertFalse(result["timing_complete"])
        self.assertTrue(result["link"]["timed_out"])

    def test_missing_plugin_report_does_not_become_zero_findings(self) -> None:
        def missing_report(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if stem.parent.name == "plugin" and stem.name == "link":
                (stem.parent / "analysis.json").unlink()
            return result
        self.assertEqual(self.invoke(process=missing_report), 1)
        result = self.report()["modes"]["plugin"]
        self.assertEqual(result["status"], "REPORT_ERROR")
        self.assertNotIn("plugin_report", result)

    def test_native_unparsed_diagnostics_fail_comparison_not_zero_findings(self) -> None:
        def invalid_sarif(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if "-fanalyzer" in command:
                Path(result["stderr_path"]).write_text("unexpected linker warning\n", encoding="utf-8")
            return result
        self.assertEqual(self.invoke(process=invalid_sarif), 1)
        report = self.report()
        result = report["modes"]["gcc_fanalyzer"]
        self.assertEqual(report["status"], "INCOMPLETE")
        self.assertEqual(result["status"], "REPORT_ERROR")
        self.assertIsNone(result["native_diagnostics"]["warning_options"])

    def test_source_drift_invalidates_comparison(self) -> None:
        def drift(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if "-fanalyzer" in command:
                self.sources[0].write_text("int changed;\n", encoding="utf-8")
            return result
        self.assertEqual(self.invoke(process=drift), 1)
        self.assertEqual(self.report()["status"], "INPUT_DRIFT")
        self.assertIn(str(self.sources[0]), self.report()["input_integrity"]["changed_paths"])

    def test_concurrent_original_plugin_rebuild_does_not_replace_snapshot(self) -> None:
        original_hash = lto.sha256_file(self.plugin)
        def rebuild(command, cwd, stem, timeout):
            result = self.fake_process(command, cwd, stem, timeout)
            if "--version" in command:
                self.plugin.write_bytes(b"new-ABI-plugin-from-concurrent-build")
            return result
        self.assertEqual(self.invoke(process=rebuild), 0)
        report = self.report()
        self.assertEqual(report["plugin"]["sha256"], original_hash)
        self.assertEqual(lto.sha256_file(Path(report["plugin"]["snapshot_path"])), original_hash)

    def test_duplicate_source_invalid_timeout_and_existing_output_rejected(self) -> None:
        self.assertEqual(self.invoke(["--source", str(self.sources[0])]), 2)
        self.assertFalse(self.output.exists())
        self.assertEqual(self.invoke(["--timeout", "nan"]), 2)
        self.assertFalse(self.output.exists())
        self.output.mkdir()
        existing = self.output / "user-data.txt"
        existing.write_text("preserve", encoding="utf-8")
        self.assertEqual(self.invoke(), 2)
        self.assertEqual(existing.read_text(), "preserve")

    def test_html_escapes_sources_and_commands(self) -> None:
        self.assertEqual(self.invoke(["--compile-arg=-DNAME=<script>bad</script>"]), 0)
        rendered = (self.output / "comparison.html").read_text(encoding="utf-8")
        self.assertNotIn("<script>", rendered)
        self.assertIn("&lt;script&gt;", rendered)


if __name__ == "__main__":
    unittest.main()
