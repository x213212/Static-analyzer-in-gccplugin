#!/usr/bin/env python3
"""Optional post-link inspection of unresolved GIMPLE callees.

Consumes the plugin's report-only callee ledger and a GNU ld --cref map.
Expands selected x86-64 ELF functions with bounded disassembly, not path
enumeration. Findings and source ownership states are never promoted here.
Call observations are not proofs of argument ownership or runtime binding.
"""
from __future__ import annotations

import argparse
from collections import deque
import hashlib
import html
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import time

RELEASE_SYMBOLS = {"free", "__libc_free", "_ZdlPv", "_ZdaPv", "_ZdlPvm", "_ZdaPvm"}
ARGUMENT_REGISTERS = ("rdi", "rsi", "rdx", "rcx", "r8", "r9")
SYMBOL_RE = re.compile(r"^\s*\d+:\s+([0-9a-fA-F]+)\s+(\d+)\s+(FUNC|IFUNC)\s+\S+\s+\S+\s+(\S+)\s+(\S+)")
INSTRUCTION_RE = re.compile(r"^\s*([0-9a-fA-F]+):\s+(\S+)(?:\s+(.*))?$")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def base_symbol(name: str) -> str:
    return name.split("@", 1)[0]


def bounded_json(path: Path) -> dict:
    if path.stat().st_size > 4 * 1024 * 1024:
        raise ValueError(f"report/cache exceeds 4 MiB: {path}")
    value = json.loads(path.read_text())
    if not isinstance(value, dict):
        raise ValueError("expected a JSON object")
    return value


def link_bindings(path: Path, cwd: Path) -> dict[str, Path]:
    if path.stat().st_size > 16 * 1024 * 1024:
        raise ValueError("link map exceeds 16 MiB")
    text = path.read_text(errors="replace")
    if "Cross Reference Table" not in text:
        raise ValueError("link map requires GNU ld --cref")
    bindings = {}
    for line in text.split("Cross Reference Table", 1)[1].splitlines():
        # GNU ld puts the defining input first; subsequent indented lines are
        # references. Archives, undefined and non-file entries remain unknown.
        match = re.match(r"^(\S+)\s+(.+?)\s*$", line)
        if not match or match[1] == "Symbol":
            continue
        file = Path(match[2])
        file = file if file.is_absolute() else cwd / file
        if file.is_file():
            bindings[match[1]] = file.resolve()
    # A default-version definition can satisfy an unversioned reference.
    # Non-default versions alone cannot identify that binding.
    for name, provider in list(bindings.items()):
        if "@@" in name:
            bindings.setdefault(base_symbol(name), provider)
    return bindings


def trace_bindings(path: Path, cwd: Path) -> dict[str, Path]:
    if path.stat().st_size > 4 * 1024 * 1024:
        raise ValueError("link trace exceeds 4 MiB")
    result = {}
    for line in path.read_text(errors="replace").splitlines():
        match = re.match(r"^.*?: (.+): definition of (\S+)\s*$", line)
        if match:
            provider = Path(match[1])
            provider = provider if provider.is_absolute() else cwd / provider
            if provider.is_file():
                result[match[2]] = provider.resolve()
    return result


def symbols(text: str) -> list[dict]:
    result = []
    seen = set()
    for line in text.splitlines():
        match = SYMBOL_RE.match(line)
        if not match or match[4] in ("UND", "ABS") or not match[4].isdigit():
            continue
        address, size = int(match[1], 16), int(match[2])
        if size <= 0:
            continue
        key = (match[5], address, size, match[3])
        if key in seen:
            continue
        seen.add(key)
        result.append(dict(name=match[5], address=address, size=size, kind=match[3]))
    return result


def select_symbol(entries: list[dict], name: str) -> dict | None:
    exact = [entry for entry in entries if entry["name"] == name]
    default = [entry for entry in entries if entry["name"].startswith(name + "@@")]
    matches = exact or default or [entry for entry in entries if base_symbol(entry["name"]) == name]
    if not matches or len({(entry["address"], entry["size"]) for entry in matches}) != 1:
        return None
    return matches[0]


def decode(text: str) -> list[dict]:
    result = []
    for line in text.splitlines():
        match = INSTRUCTION_RE.match(line)
        if match:
            result.append(dict(address=int(match[1], 16), op=match[2], operands=match[3] or ""))
    return result


def observations(instructions: list[dict]) -> list[dict]:
    """CFG may-reach observations; predicates are deliberately not guessed."""
    if not instructions:
        return []
    by_address = {item["address"]: index for index, item in enumerate(instructions)}
    pending = [0]
    visited = set()
    found = []
    while pending:
        index = pending.pop()
        if index in visited or not 0 <= index < len(instructions):
            continue
        visited.add(index)
        item = instructions[index]
        op, operands = item["op"], item["operands"]
        is_jump = op.startswith("j") or op.startswith("loop")
        if op.startswith("call") or is_jump:
            target = re.match(r"^([0-9a-fA-F]+)\s+<([^>]+)>", operands)
            internal = target is not None and int(target[1], 16) in by_address
            if is_jump and internal:
                pending.append(by_address[int(target[1], 16)])
            if op.startswith("call") or (op.startswith("jmp") and not internal):
                found.append(dict(address=item["address"], instruction=f"{op} {operands}",
                                  target=target[2] if target else None,
                                  target_address=int(target[1], 16) if target else None,
                                  indirect=target is None, tail_call=op.startswith("jmp")))
            if op.startswith("jmp"):
                continue
        if op.startswith("ret") or op in ("ud2", "hlt"):
            continue
        pending.append(index + 1)
    return sorted(found, key=lambda item: item["address"])


def linear_argument_origins(instructions: list[dict]) -> dict[int, int | None]:
    """Small SysV x86-64 prefix recognizer; branches/loads stay unverified."""
    origins = {register: index for index, register in enumerate(ARGUMENT_REGISTERS)}
    result = {}
    for item in instructions:
        op, operands = item["op"], item["operands"].split("#", 1)[0].strip()
        if op.startswith("j") and not op.startswith("jmp"):
            break
        if op.startswith("call") or op.startswith("jmp"):
            result[item["address"]] = origins.get("rdi")
            for register in (*ARGUMENT_REGISTERS, "rax", "r10", "r11"):
                origins.pop(register, None)
            if op.startswith("jmp"):
                break
        elif op == "mov":
            fields = [field.strip() for field in operands.split(",")]
            if len(fields) == 2 and re.fullmatch(r"r(?:[abcd]x|[sb]p|[sd]i|[89]|1[0-5])", fields[0]):
                origins.pop(fields[0], None) if fields[1] not in origins else None
                if fields[1] in origins:
                    origins[fields[0]] = origins[fields[1]]
            elif fields and re.fullmatch(r"(?:e[a-z]{2}|r\d+[dwb]?|[abcd][lh]|[sd]il)", fields[0]):
                # Partial writes destroy pointer identity.
                partial = {"dil": "rdi", "sil": "rsi", "cl": "rcx", "ch": "rcx", "dl": "rdx", "dh": "rdx", "al": "rax", "ah": "rax", "bl": "rbx", "bh": "rbx"}
                full = partial.get(fields[0], "r" + fields[0][1:] if fields[0].startswith("e") else re.sub(r"[dwb]$", "", fields[0]))
                origins.pop(full, None)
        elif op in ("endbr64", "nop", "push", "test", "cmp"):
            pass
        elif op in ("add", "sub") and re.fullmatch(r"rsp,0x[0-9a-f]+", operands):
            pass
        elif op.startswith("ret"):
            break
        else:
            # Unsupported arithmetic, pops and memory operations can change
            # tracked registers. Do not carry stale provenance across them.
            origins.clear()
    return result


class Inspector:
    def __init__(self, cache: Path, max_instructions: int, deadline: float):
        self.cache = cache
        self.max_instructions = max_instructions
        self.deadline = deadline
        self.images = {}
        self.hits = self.misses = 0
        self.tool_ms = 0.0
        self.tools = {name: Path(shutil.which(name) or "") for name in ("readelf", "objdump")}
        if any(not path.is_file() for path in self.tools.values()):
            raise ValueError("GNU readelf and objdump are required")
        self.tool_hashes = {name: sha256(path) for name, path in self.tools.items()}
        self.code_hash = sha256(Path(__file__))

    def run(self, name: str, args: list[str]) -> str:
        remaining = self.deadline - time.monotonic()
        if remaining <= 0:
            raise TimeoutError("binary tracing time budget exhausted")
        start = time.monotonic()
        with tempfile.TemporaryFile() as out, tempfile.TemporaryFile() as err:
            process = subprocess.run([str(self.tools[name]), *args], stdout=out, stderr=err,
                                     timeout=min(5, remaining), env={**os.environ, "LC_ALL": "C"})
            self.tool_ms += (time.monotonic() - start) * 1000
            if out.tell() > 8 * 1024 * 1024 or err.tell() > 1024 * 1024:
                raise ValueError("binary tool output limit exceeded")
            if process.returncode:
                raise ValueError(f"{name} failed with exit code {process.returncode}")
            out.seek(0)
            return out.read().decode("utf-8", errors="replace")

    def cached(self, descriptor: dict, build):
        key = hashlib.sha256(json.dumps({"descriptor": descriptor, "tools": self.tool_hashes,
                                        "inspector": self.code_hash}, sort_keys=True).encode()).hexdigest()
        path = self.cache / (key + ".json")
        if path.is_file():
            try:
                document = bounded_json(path)
                if document["key"] == key:
                    payload = document["payload"]
                    digest = hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest()
                    if digest == document["payload_sha256"]:
                        self.hits += 1
                        return payload
            except (ValueError, KeyError, OSError):
                pass
        self.misses += 1
        payload = build()
        self.cache.mkdir(parents=True, exist_ok=True)
        temporary = path.with_suffix(f".tmp.{os.getpid()}")
        temporary.write_text(json.dumps({"key": key, "payload": payload,
            "payload_sha256": hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest()}))
        temporary.replace(path)
        return payload

    def image(self, path: Path) -> dict:
        if path in self.images:
            return self.images[path]
        if path.stat().st_size > 256 * 1024 * 1024:
            raise ValueError("ELF exceeds 256 MiB")
        with path.open("rb") as stream:
            if stream.read(4) != b"\x7fELF":
                raise ValueError("defining input is not ELF (archive/linker script/object unresolved)")
        digest = sha256(path)
        def build():
            text = self.run("readelf", ["--wide", "--file-header", "--symbols", "--notes", str(path)])
            if "Advanced Micro Devices X86-64" not in text:
                raise ValueError("this prototype only disassembles x86-64 ELF")
            if not re.search(r"Type:\s+(?:DYN|EXEC)\b", text):
                raise ValueError("relocatable objects require relocation-aware analysis")
            build_id = re.search(r"Build ID:\s+([0-9a-f]+)", text)
            return dict(sha256=digest, build_id=build_id[1] if build_id else None,
                        symbols=symbols(text), machine="x86-64", abi_assumption="SysV AMD64")
        image = self.cached(dict(kind="elf-index", sha256=digest), build)
        if sha256(path) != digest:
            raise ValueError("ELF changed while indexing")
        self.images[path] = image
        return image

    def function(self, path: Path, name: str) -> dict:
        image = self.image(path)
        symbol = select_symbol(image["symbols"], name)
        result = dict(provider=str(path), provider_sha256=image["sha256"],
                      build_id=image["build_id"], symbol=name, ownership_effect="UNKNOWN",
                      proof_eligible=False)
        if symbol is None or symbol["kind"] == "IFUNC" or symbol["size"] > 65536:
            return {**result, "reason": "missing/ambiguous symbol, IFUNC resolver, or function-size cap", "calls": []}
        def build():
            text = self.run("objdump", ["-d", "-w", "-M", "intel", "--no-show-raw-insn",
                f"--start-address={symbol['address']}",
                f"--stop-address={symbol['address'] + symbol['size']}", str(path)])
            instructions = decode(text)
            truncated = len(instructions) > self.max_instructions
            instructions = instructions[:self.max_instructions]
            calls = observations(instructions)
            origins = linear_argument_origins(instructions)
            for call in calls:
                call["release_symbol"] = base_symbol(call["target"] or "") in RELEASE_SYMBOLS
                call["candidate_argument_index"] = origins.get(call["address"]) if call["release_symbol"] else None
                call["argument_proof"] = "linear-register-prefix-only; path/binding not certified"
            return dict(address=symbol["address"], size=symbol["size"],
                        instruction_count=len(instructions), truncated=truncated,
                        calls=calls, disassembly=text[:1024 * 1024])
        evidence = self.cached(dict(kind="function", sha256=image["sha256"], symbol=symbol,
                                    max_instructions=self.max_instructions), build)
        return {**result, **evidence}


def trace(report: dict, bindings: dict[str, Path], inspector: Inspector,
          max_functions: int = 32, max_depth: int = 2) -> dict:
    effects = report.get("callee_effects")
    if not isinstance(effects, list):
        raise ValueError("report has no callee-effect ledger; rebuild the plugin")
    if len(effects) > 128:
        raise ValueError("callee ledger exceeds its record limit")
    roots = []
    pending = deque()
    for effect in effects:
        if not effect.get("needs_review"):
            continue
        name = effect.get("symbol", "")
        provider = bindings.get(name)
        root = {**effect, "selected_provider": str(provider) if provider else None,
                "status": "PENDING" if provider and not effect.get("body_available") else "INCONCLUSIVE",
                "reason": "source-summary refinement required" if effect.get("body_available") else
                          "link-map provider selected; runtime binding unverified" if provider else
                          "indirect/unresolved link symbol"}
        roots.append(root)
        if root["status"] == "PENDING":
            pending.append((provider, name, 0))
    functions = []
    seen = set()
    stopped = []
    while pending and len(seen) < max_functions:
        path, name, depth = pending.popleft()
        if (path, name) in seen:
            continue
        seen.add((path, name))
        try:
            evidence = inspector.function(path, name)
        except (ValueError, OSError, subprocess.TimeoutExpired, TimeoutError) as error:
            stopped.append(dict(provider=str(path), symbol=name, reason=str(error)))
            continue
        evidence["depth"] = depth
        functions.append(evidence)
        if depth >= max_depth or evidence.get("truncated"):
            continue
        image = inspector.images[path]
        for call in evidence["calls"]:
            target = call["target"]
            if not target or "+" in target or "-" in target:
                continue
            name = base_symbol(target)
            local = select_symbol(image["symbols"], name)
            # PLT references remain subject to link resolution/interposition.
            provider = bindings.get(name) if "@plt" in target else path if local else bindings.get(name)
            if provider:
                pending.append((provider, name, depth + 1))
    if pending:
        stopped.append(dict(reason="function/depth/work limits leave expansion incomplete"))
    for root in roots:
        if root["status"] == "PENDING":
            root["status"] = "EVIDENCE_ONLY"
    return dict(schema_version=1, mode="post-link-binary-trace", roots=roots,
                functions=functions, stopped=stopped,
                limitations=["No target program or shared library is executed.",
                    "Link-map providers are build-time selections, not guaranteed runtime bindings.",
                    "Register-prefix origins assume SysV AMD64 and are not ownership certificates.",
                    "CFG reachability is not path feasibility; indirect calls and incomplete expansions stay unknown.",
                    "No observations are imported into PROVEN findings or benchmark true positives."],
                original_finding_summary=report.get("summary"),
                ledger_omitted_events=report.get("callee_effect_summary", {}).get("omitted_events", 0),
                cache=dict(hits=inspector.hits, misses=inspector.misses),
                tool_ms=round(inspector.tool_ms, 3))


def render(document: dict) -> str:
    esc = lambda value: html.escape(str(value), quote=True)
    parts = ["<!doctype html><html lang=zh-Hant><meta charset=utf-8>",
             "<meta name=viewport content='width=device-width,initial-scale=1'>",
             "<title>二次函式追蹤</title><style>body{font:16px system-ui;background:#f3f5f9;color:#172033;margin:0}main{max-width:1100px;margin:32px auto;padding:20px}.card,details{background:white;border:1px solid #dce2eb;border-radius:12px;padding:18px;margin:14px 0}table{width:100%;border-collapse:collapse}th,td{text-align:left;padding:12px;border-bottom:1px solid #e0e5ed;vertical-align:top}pre,code{white-space:pre-wrap;overflow-wrap:anywhere}.muted{color:#536078}.unknown{color:#9a5a00}</style><main>",
             "<h1>二次函式追蹤</h1><p class=unknown>這是二進位呼叫證據；尚未認證所有權效果，不增加漏洞檢出數。</p>",
             f"<div class=card>函式 {len(document['functions'])} · 快取命中 {document['cache']['hits']} · 新分析 {document['cache']['misses']} · 工具耗時 {document['tool_ms']} ms</div>",
             "<h2>待查呼叫</h2><table><tr><th>Caller → callee</th><th>函式庫</th><th>進度</th></tr>"]
    for root in document["roots"]:
        parts.append(f"<tr><td>{esc(root.get('caller'))} → {esc(root.get('symbol'))}<br>參數 {int(root.get('argument_index', 0)) + 1}</td><td><code>{esc(root.get('selected_provider'))}</code></td><td>{esc(root['status'])}<br>{esc(root['reason'])}</td></tr>")
    parts.append("</table><h2>二進位展開</h2>")
    for function in document["functions"]:
        calls = function.get("calls", [])
        releases = sum(call["release_symbol"] for call in calls)
        indirect = sum(call["indirect"] for call in calls)
        parts.append(f"<details><summary>{esc(function['symbol'])} · 釋放符號 {releases} · 間接呼叫 {indirect} · 效果 UNKNOWN</summary><p><code>{esc(function['provider'])}</code><br>Build ID: {esc(function['build_id'])}</p><ul>")
        for call in calls:
            origin = call['candidate_argument_index']
            label = f"參數 {origin + 1} 的直線寄存器來源（待驗證）" if origin is not None else "參數來源未確認"
            parts.append(f"<li><code>0x{call['address']:x}: {esc(call['instruction'])}</code> · {esc(label)}</li>")
        parts.append(f"</ul><pre>{esc(function.get('disassembly', function.get('reason', '')))}</pre></details>")
    parts.append("<h2>未完成與限制</h2><ul>")
    for item in document["stopped"]:
        parts.append(f"<li>{esc(item)}</li>")
    for item in document["limitations"]:
        parts.append(f"<li class=muted>{esc(item)}</li>")
    return "\n".join([*parts, "</ul></main></html>"])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--link-map", type=Path, required=True)
    parser.add_argument("--link-log", type=Path, help="optional GNU ld --trace-symbol output for imported dependencies")
    parser.add_argument("--link-cwd", type=Path, default=Path.cwd())
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--cache", type=Path, required=True)
    parser.add_argument("--max-functions", type=int, default=32)
    parser.add_argument("--max-depth", type=int, default=2)
    parser.add_argument("--max-instructions", type=int, default=4096)
    parser.add_argument("--timeout", type=float, default=10)
    args = parser.parse_args()
    if not (1 <= args.max_functions <= 256 and 0 <= args.max_depth <= 8 and
            1 <= args.max_instructions <= 65536 and 0 < args.timeout <= 60):
        parser.error("invalid expansion/time bounds")
    start = time.monotonic()
    try:
        input_paths = [args.report, args.link_map, *([args.link_log] if args.link_log else [])]
        inputs = {str(path.resolve()): sha256(path) for path in input_paths}
        inspector = Inspector(args.cache.resolve(), args.max_instructions, start + args.timeout)
        bindings = link_bindings(args.link_map, args.link_cwd)
        if args.link_log:
            bindings.update(trace_bindings(args.link_log, args.link_cwd))
        document = trace(bounded_json(args.report), bindings,
                         inspector, args.max_functions, args.max_depth)
        for path, image in inspector.images.items():
            if sha256(path) != image["sha256"]:
                raise ValueError("provider changed during tracing")
        if any(sha256(Path(path)) != digest for path, digest in inputs.items()):
            raise ValueError("report/link map changed during tracing")
        document.update(inputs=inputs, tools=inspector.tool_hashes,
                        inspector_sha256=inspector.code_hash,
                        elapsed_ms=round((time.monotonic() - start) * 1000, 3))
        args.output.mkdir(parents=True, exist_ok=False)
        (args.output / "trace.json").write_text(json.dumps(document, indent=2) + "\n")
        (args.output / "trace.html").write_text(render(document))
        print(f"EVIDENCE_ONLY: {len(document['functions'])} functions, {document['elapsed_ms']} ms, "
              f"cache {inspector.hits} hits/{inspector.misses} misses")
        return 0
    except (OSError, ValueError, KeyError, TypeError) as error:
        print(f"binary trace failed: {error}")
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
