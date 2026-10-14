#!/usr/bin/env python3
"""Conservative compiler launcher for the GCC static-analyzer plugin.

The launcher injects the production proof-only configuration only when the
driver arguments identify exactly one translation unit.  Driver queries,
preprocessing/dependency-only commands, and object-only links are passed
through unchanged.  Ambiguous commands fail before the compiler is started.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
from typing import Sequence


HASH_HEX_LENGTH = 20
REQUIRED_PLUGIN_OPTIONS: tuple[tuple[str, str], ...] = (
    ("analysispolicy", "proof-only"),
    ("debugoutput", "0"),
    ("legacyengine", "0"),
    ("memoryleakcheck", "1"),
    ("doublefreecheck", "1"),
    ("useafterfree", "1"),
    ("threadmod", "1"),
    ("smt", "1"),
    ("report", "1"),
    ("reporthtml", "0"),
)
RESERVED_PLUGIN_OPTIONS = {
    key for key, _ in REQUIRED_PLUGIN_OPTIONS
} | {"reportpath"}
PLUGIN_OPTION_KEY = re.compile(r"^[A-Za-z][A-Za-z0-9_-]*$")
SAFE_STEM_CHARACTER = re.compile(r"[^A-Za-z0-9._-]+")

# GCC options in this set consume the following argv item.  Joined forms such
# as -Iinclude or -MFdeps.d begin with '-' and need no special treatment.
OPTIONS_WITH_VALUE = {
    "-A",
    "-B",
    "-D",
    "-I",
    "-L",
    "-MF",
    "-MQ",
    "-MT",
    "-T",
    "-U",
    "-Xassembler",
    "-Xlinker",
    "-Xpreprocessor",
    "--include",
    "--include-directory",
    "--param",
    "--sysroot",
    "-aux-info",
    "-dumpbase",
    "-dumpbase-ext",
    "-dumpdir",
    "-idirafter",
    "-imacros",
    "-include",
    "-include-pch",
    "-iprefix",
    "-iquote",
    "-isystem",
    "-isysroot",
    "-iwithprefix",
    "-iwithprefixbefore",
    "-l",
    "-specs",
    "-u",
    "-wrapper",
}

SOURCE_SUFFIXES = {
    ".adb",
    ".ads",
    ".asm",
    ".c",
    ".cc",
    ".cp",
    ".cpp",
    ".cxx",
    ".d",
    ".f",
    ".f03",
    ".f08",
    ".f77",
    ".f90",
    ".f95",
    ".for",
    ".go",
    ".h",
    ".hh",
    ".hpp",
    ".hxx",
    ".i",
    ".ii",
    ".m",
    ".mm",
    ".r",
    ".rs",
    ".s",
    ".sx",
    ".tcc",
}

NO_COMPILE_OPTIONS = {"-E", "-M", "-MM"}
ALWAYS_QUERY_OPTIONS = {"--version", "-###", "-dumpfullversion", "-dumpmachine", "-dumpversion"}
QUERY_PREFIXES = (
    "--help",
    "-print-file-name=",
    "-print-multi-directory",
    "-print-multi-lib",
    "-print-multi-os-directory",
    "-print-prog-name=",
    "-print-search-dirs",
    "-print-sysroot",
)


class LauncherError(RuntimeError):
    """A safe, user-facing launcher error."""


@dataclass(frozen=True)
class Invocation:
    analyze: bool
    source_token: str | None = None
    output_token: str | None = None


def _driver_flags(arguments: Sequence[str]) -> list[str]:
    """Return actual driver flags, excluding option operands and filenames."""

    result: list[str] = []
    index = 0
    while index < len(arguments):
        item = arguments[index]
        if item == "--":
            break
        if item in OPTIONS_WITH_VALUE or item in {"-o", "--output", "-x"}:
            index += 2
            continue
        if item.startswith("-") and item != "-":
            result.append(item)
        index += 1
    return result


def _is_query(arguments: Sequence[str]) -> bool:
    options = _driver_flags(arguments)
    if any(item in ALWAYS_QUERY_OPTIONS for item in options):
        return True
    return any(item.startswith(prefix) for item in options for prefix in QUERY_PREFIXES)


def _is_source_filename(value: str) -> bool:
    suffix = Path(value).suffix
    if suffix == ".C":
        return True
    return suffix.lower() in SOURCE_SUFFIXES


def _consume_value(arguments: Sequence[str], index: int, option: str) -> int:
    if index + 1 >= len(arguments):
        raise LauncherError(f"compiler option '{option}' is missing its value")
    return index + 2


def classify_invocation(arguments: Sequence[str]) -> Invocation:
    """Classify a GCC-driver argv without reading response files."""

    if any(item.startswith("@") for item in arguments):
        raise LauncherError(
            "response files are not supported because their source count and "
            "plugin options cannot be verified safely"
        )

    driver_options = _driver_flags(arguments)
    if any(item in NO_COMPILE_OPTIONS for item in driver_options) or _is_query(
        arguments
    ):
        return Invocation(analyze=False)

    sources: list[str] = []
    output_token: str | None = None
    language: str | None = None
    options_finished = False
    index = 0
    while index < len(arguments):
        item = arguments[index]
        if not options_finished and item == "--":
            options_finished = True
            index += 1
            continue

        if not options_finished and item == "-x":
            if index + 1 >= len(arguments):
                raise LauncherError("compiler option '-x' is missing its language")
            language_value = arguments[index + 1]
            language = None if language_value == "none" else language_value
            index += 2
            continue
        if not options_finished and item.startswith("-x") and len(item) > 2:
            language_value = item[2:]
            language = None if language_value == "none" else language_value
            index += 1
            continue

        if not options_finished and item in {"-o", "--output"}:
            if index + 1 >= len(arguments):
                raise LauncherError(f"compiler option '{item}' is missing its value")
            if output_token is not None:
                raise LauncherError("multiple compiler output paths are ambiguous")
            output_token = arguments[index + 1]
            index += 2
            continue
        if not options_finished and item.startswith("--output="):
            if output_token is not None:
                raise LauncherError("multiple compiler output paths are ambiguous")
            output_token = item.split("=", 1)[1]
            if not output_token:
                raise LauncherError("compiler option '--output' has an empty value")
            index += 1
            continue
        if (
            not options_finished
            and item.startswith("-o")
            and item != "-o"
        ):
            if output_token is not None:
                raise LauncherError("multiple compiler output paths are ambiguous")
            output_token = item[2:]
            index += 1
            continue

        if not options_finished and item in OPTIONS_WITH_VALUE:
            index = _consume_value(arguments, index, item)
            continue

        is_positional = options_finished or item == "-" or not item.startswith("-")
        if is_positional:
            if item == "-":
                if language is None:
                    raise LauncherError(
                        "stdin compilation requires an explicit '-x <language>' so "
                        "the translation unit can be identified safely"
                    )
                sources.append(item)
            elif language is not None or _is_source_filename(item):
                sources.append(item)
            index += 1
            continue

        index += 1

    if len(sources) > 1:
        quoted = ", ".join(repr(item) for item in sources[:4])
        if len(sources) > 4:
            quoted += ", ..."
        raise LauncherError(
            f"exactly one source is required; found {len(sources)}: {quoted}"
        )
    if not sources:
        return Invocation(analyze=False)
    return Invocation(analyze=True, source_token=sources[0], output_token=output_token)


def _parse_plugin_options(items: Sequence[str]) -> list[tuple[str, str]]:
    result: list[tuple[str, str]] = []
    seen: set[str] = set()
    for item in items:
        if "=" not in item:
            raise LauncherError(
                f"plugin option {item!r} must use the form key=value"
            )
        key, value = item.split("=", 1)
        if not PLUGIN_OPTION_KEY.fullmatch(key):
            raise LauncherError(f"invalid plugin option key: {key!r}")
        if not value:
            raise LauncherError(f"plugin option '{key}' has an empty value")
        if len(value) > 4_096:
            raise LauncherError(f"plugin option '{key}' exceeds 4096 characters")
        if key in RESERVED_PLUGIN_OPTIONS:
            raise LauncherError(
                f"plugin option '{key}' is managed by the launcher and cannot "
                "be overridden"
            )
        if key in seen:
            raise LauncherError(f"plugin option '{key}' was specified more than once")
        seen.add(key)
        result.append((key, value))
    return result


def _validate_no_plugin_conflicts(arguments: Sequence[str], plugin_name: str) -> None:
    target_prefix = f"-fplugin-arg-{plugin_name}-"
    for item in _driver_flags(arguments):
        if item == "-fplugin":
            raise LauncherError(
                "compiler argument '-fplugin' is missing an explicit '=PATH' value"
            )
        if item.startswith("-fplugin="):
            configured_path = item.split("=", 1)[1]
            if not configured_path or Path(configured_path).stem == plugin_name:
                raise LauncherError(
                    f"compiler arguments already load plugin '{plugin_name}'; "
                    "refusing to configure it twice"
                )
            # GCC supports loading multiple different plugins.  Preserve an
            # unrelated project plugin instead of making the launcher
            # needlessly incompatible with that build.
            continue
        if item.startswith(target_prefix):
            raise LauncherError(
                f"compiler arguments already configure plugin '{plugin_name}'; "
                "use wrapper --plugin-option entries instead"
            )


def _resolve_source(token: str) -> str:
    if token == "-":
        return f"<stdin:{Path.cwd().resolve().as_posix()}>"
    return Path(token).expanduser().resolve(strict=False).as_posix()


def _resolve_output(token: str | None) -> str:
    if token is None:
        return "<compiler-default>"
    if token == "-":
        return "<stdout>"
    return Path(token).expanduser().resolve(strict=False).as_posix()


def _safe_source_label(source_token: str) -> str:
    if source_token == "-":
        return "stdin"
    label = Path(source_token).name or "translation-unit"
    label = SAFE_STEM_CHARACTER.sub("_", label).strip("._-")
    return (label or "translation-unit")[:64]


def report_stem(
    report_directory: Path,
    source_token: str,
    output_token: str | None,
    compiler_arguments: Sequence[str],
    plugin_path: Path,
    plugin_options: Sequence[tuple[str, str]],
) -> Path:
    """Return a deterministic, cross-directory collision-resistant stem."""

    payload = {
        "source": _resolve_source(source_token),
        "output": _resolve_output(output_token),
        "compiler_arguments": list(compiler_arguments),
        "plugin": plugin_path.as_posix(),
        "plugin_options": list(plugin_options),
    }
    encoded = json.dumps(
        payload, ensure_ascii=False, sort_keys=True, separators=(",", ":")
    ).encode("utf-8")
    digest = hashlib.sha256(encoded).hexdigest()[:HASH_HEX_LENGTH]
    return report_directory / f"{_safe_source_label(source_token)}-{digest}"


def _validated_plugin(path: Path) -> Path:
    try:
        resolved = path.expanduser().resolve(strict=True)
    except OSError as error:
        raise LauncherError(f"plugin does not exist: {path}") from error
    if not resolved.is_file():
        raise LauncherError(f"plugin is not a regular file: {resolved}")
    plugin_name = resolved.stem
    if not plugin_name or not re.fullmatch(r"[A-Za-z0-9_.-]+", plugin_name):
        raise LauncherError(
            f"plugin filename does not provide a safe GCC plugin name: {resolved.name!r}"
        )
    return resolved


def _prepare_report_directory(path: Path) -> Path:
    resolved = path.expanduser().resolve(strict=False)
    try:
        resolved.mkdir(parents=True, exist_ok=True)
    except OSError as error:
        raise LauncherError(
            f"cannot create report directory '{resolved}': {error}"
        ) from error
    if not resolved.is_dir():
        raise LauncherError(f"report path is not a directory: {resolved}")
    return resolved


def build_compiler_command(
    compiler: str,
    plugin: Path,
    report_directory: Path,
    compiler_arguments: Sequence[str],
    extra_plugin_options: Sequence[tuple[str, str]],
    invocation: Invocation,
) -> list[str]:
    if not invocation.analyze or invocation.source_token is None:
        return [compiler, *compiler_arguments]

    plugin_name = plugin.stem
    _validate_no_plugin_conflicts(compiler_arguments, plugin_name)
    configured_options = [*REQUIRED_PLUGIN_OPTIONS, *extra_plugin_options]
    stem = report_stem(
        report_directory,
        invocation.source_token,
        invocation.output_token,
        compiler_arguments,
        plugin,
        configured_options,
    )
    injected = [f"-fplugin={plugin}"]
    injected.extend(
        f"-fplugin-arg-{plugin_name}-{key}={value}"
        for key, value in configured_options
    )
    injected.append(f"-fplugin-arg-{plugin_name}-reportpath={stem}")

    # Keep injected options before a compiler-level '--'; options after it are
    # filenames, not driver switches.
    try:
        separator = compiler_arguments.index("--")
    except ValueError:
        separator = len(compiler_arguments)
    return [
        compiler,
        *compiler_arguments[:separator],
        *injected,
        *compiler_arguments[separator:],
    ]


def _propagate_child_signal(returncode: int) -> int:
    if returncode >= 0:
        return returncode
    signum = -returncode
    try:
        if signum not in {signal.SIGKILL, signal.SIGSTOP}:
            signal.signal(signum, signal.SIG_DFL)
        os.kill(os.getpid(), signum)
    except (OSError, RuntimeError, ValueError):
        # This fallback is mainly for non-POSIX hosts.  POSIX callers observe
        # the same signal because os.kill above does not return.
        return 128 + signum
    return 128 + signum


def run_compiler(command: Sequence[str]) -> int:
    try:
        completed = subprocess.run(list(command), check=False, shell=False)
    except FileNotFoundError:
        print(f"error: compiler not found: {command[0]}", file=sys.stderr)
        return 127
    except PermissionError:
        print(f"error: compiler is not executable: {command[0]}", file=sys.stderr)
        return 126
    except OSError as error:
        print(f"error: could not start compiler '{command[0]}': {error}", file=sys.stderr)
        return 126
    return _propagate_child_signal(completed.returncode)


def _parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "Run one GCC translation-unit compilation with the static analyzer, "
            "or transparently pass through non-compilation driver commands."
        )
    )
    parser.add_argument("--compiler", required=True, help="GCC driver executable")
    parser.add_argument(
        "--plugin", required=True, type=Path, help="static-analyzer plugin shared object"
    )
    parser.add_argument(
        "--report-dir", required=True, type=Path, help="directory for per-TU reports"
    )
    parser.add_argument(
        "--plugin-option",
        action="append",
        default=[],
        metavar="KEY=VALUE",
        help="additional non-reserved plugin option; repeat as needed",
    )
    parser.add_argument(
        "compiler_args",
        nargs=argparse.REMAINDER,
        help="original compiler argv, preceded by --",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = _parser()
    arguments = parser.parse_args(argv)
    if not arguments.compiler_args or arguments.compiler_args[0] != "--":
        parser.error("original compiler arguments must be preceded by '--'")
    compiler_arguments = arguments.compiler_args[1:]

    try:
        extra_options = _parse_plugin_options(arguments.plugin_option)
        invocation = classify_invocation(compiler_arguments)
        if not invocation.analyze:
            return run_compiler([arguments.compiler, *compiler_arguments])

        plugin = _validated_plugin(arguments.plugin)
        _validate_no_plugin_conflicts(compiler_arguments, plugin.stem)
        report_directory = _prepare_report_directory(arguments.report_dir)
        command = build_compiler_command(
            arguments.compiler,
            plugin,
            report_directory,
            compiler_arguments,
            extra_options,
            invocation,
        )
    except LauncherError as error:
        print(f"error: {error}", file=sys.stderr)
        return 2
    return run_compiler(command)


if __name__ == "__main__":
    raise SystemExit(main())
