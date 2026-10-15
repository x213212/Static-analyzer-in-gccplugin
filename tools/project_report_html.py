"""Offline bilingual presentation of recorded project evidence; no analysis."""
from __future__ import annotations

import hashlib
import json
import math
import os
from collections import deque
from pathlib import Path

TOOLS = ("plugin", "gcc16_fanalyzer")


def speed_evidence(report: dict, speed: dict | None) -> dict | None:
    if speed is None:
        return None
    if speed.get("status") != "OK":
        raise ValueError("speed report did not complete successfully")
    candidate = [row for row in speed.get("runs", []) if row.get("mode") == "candidate"]
    if not candidate:
        raise ValueError("speed report has no candidate compiler runs")
    hashes = speed.get("inputs_sha256", {})
    for row in candidate:
        command = row["process"]["command"]
        plugins = [arg.split("=", 1)[1] for arg in command if arg.startswith("-fplugin=")]
        if len(plugins) != 1 or hashes.get(plugins[0]) != report.get("plugin_sha256"):
            raise ValueError("speed report measured a different plugin build")
        if row["process"].get("status") != "OK":
            raise ValueError("speed report contains an unsuccessful candidate run")
        expected = report.get("recorded_input_hashes", {}).get(command[0])
        if expected is not None and hashes.get(command[0]) != expected:
            raise ValueError("speed report measured a different compiler")
    summary = {}
    for mode in ("plain", "baseline", "candidate", "native"):
        values = speed["summary"][mode]
        summary[mode] = {}
        for key in ("median_wall_ms", "median_cpu_ms"):
            value = values[key]
            if not isinstance(value, (int, float)) or not math.isfinite(value) or value < 0:
                raise ValueError("invalid speed measurement")
            summary[mode][key] = value
    return {"summary": summary, "rounds": speed["rounds"],
            "paired": speed.get("paired", {})}


def location(value: dict) -> dict:
    return {"file": str(value.get("file", "")), "line": value.get("line", 0),
            "column": value.get("column", 0)}


def analysis_evidence(document: dict) -> dict:
    findings = document.get("findings", [])
    unknowns = document.get("inconclusive", [])
    calls = document.get("callee_effects", [])
    return {
        "findings": [{"rule": item["rule_id"], "message": item.get("message", ""),
                      "status": item.get("status", "UNSPECIFIED"),
                      "confidence": item.get("confidence", "unknown"),
                      "remediation": item.get("remediation", {}),
                      "location": location(item.get("location", {})),
                      "trace": [{"role": p.get("role", ""), **location(p)} for p in item.get("trace", [])[:8]],
                      "trace_omitted": max(0, len(item.get("trace", [])) - 8),
                      "model_scope": item.get("model_scope", ""),
                      "path_verdict": item.get("path_verdict", "")} for item in findings[:8]],
        "findings_omitted": max(0, len(findings) - 8),
        "unknowns": [{"rule": item["rule_id"], "reason": item.get("reason", ""),
                      "detail": item.get("detail", ""), "location": location(item.get("location", {})),
                      "related": [{"role": p.get("role", ""), **location(p)} for p in item.get("related", [])[:8]]}
                     for item in unknowns[:8]],
        "unknown_total": document.get("inconclusive_summary", {}).get("total", len(unknowns)),
        "calls": [{"caller": c.get("caller", ""), "callee": c.get("symbol", ""),
                   "argument": c.get("argument_index", 0) + 1, "fates": c.get("fates", "unknown"),
                   "body": bool(c.get("body_available")), "review": bool(c.get("needs_review")),
                   "location": location(c.get("location", {}))} for c in calls[:16]],
        "calls_total": document.get("callee_effect_summary", {}).get("records", len(calls)),
        "profile": document.get("analysis", {}).get("standard_stdio_model_scope", ""),
    }


def relative_link(path: str | Path, output: Path) -> str:
    return os.path.relpath(Path(path), output)


def focused_graph(graph: dict, evidence: dict, snippet) -> dict:
    """Select real caller chains and CFG predecessors, without inventing edges."""
    functions = {f['symbol']: f for f in graph.get('functions', [])}
    roots = [s for s, f in functions.items() if f['name'] == 'main']
    if not roots:
        called = {s['callee'] for f in functions.values() for s in f['statements']}
        roots = [s for s in functions if s not in called]
    parents = {s: None for s in roots}
    incoming = {}
    queue = deque(roots)
    while queue:
        caller = queue.popleft()
        for call in functions[caller]['statements']:
            callee = call['callee']
            if callee in functions and callee not in parents:
                parents[callee] = caller
                incoming[callee] = call
                queue.append(callee)
    points = [f['location'] for f in evidence['findings']]
    points += [p for f in evidence['findings'] for p in f['trace']]
    points += [u['location'] for u in evidence['unknowns']]
    point_keys = {(p['file'], p['line']) for p in points if p['file'] and p['line']}
    problem_keys = {(f['location']['file'], f['location']['line']) for f in evidence['findings']}
    targets = {symbol for symbol, f in functions.items()
               if any((s['file'], s['line']) in point_keys for s in f['statements'])}
    linked_events = set()
    kinds = {'release'} if any(f['rule'] == 'SA002' for f in evidence['findings']) else {'memory'}
    for symbol, f in functions.items():
        for call in f['statements']:
            if (call['file'], call['line']) not in problem_keys or call['callee'] not in functions:
                continue
            child = call['callee']
            # Retain the exact reported invocation rather than an unrelated
            # call to the same helper elsewhere in the translation unit.
            ancestor, seen = symbol, set()
            while ancestor is not None and ancestor not in seen:
                seen.add(ancestor)
                ancestor = parents.get(ancestor)
            if child not in seen:
                parents[child], incoming[child] = symbol, call
            pending, visited = [child], set()
            while pending:
                current = pending.pop()
                if current in visited:
                    continue
                visited.add(current)
                for item in functions[current]['statements']:
                    if item['kind'] in kinds:
                        linked_events.add((current, item['block'], item['file'], item['line']))
                        targets.add(current)
                    if item['callee'] in functions and item['callee'] not in visited:
                        pending.append(item['callee'])
    routes = []
    for target in sorted(targets):
        route = [target]
        while route[-1] in parents and parents[route[-1]] is not None and parents[route[-1]] not in route:
            route.append(parents[route[-1]])
        routes.append(list(reversed(route)))
    if not routes:
        routes = [[r] for r in roots[:1]]
    # A shorter prefix is already visible in its longer caller chain.
    routes = [p for p in routes if not any(len(q) > len(p) and q[:len(p)] == p for q in routes)]
    retained = {s for route in routes for s in route}
    selected = {}
    for symbol in sorted(retained):
        f = functions[symbol]
        toward = {p[i+1] for p in routes for i, s in enumerate(p[:-1]) if s == symbol}
        focus = [s for s in f['statements'] if (s['file'], s['line']) in point_keys or s['callee'] in toward or
                 (symbol, s['block'], s['file'], s['line']) in linked_events]
        target_blocks = {s['block'] for s in focus}
        predecessors = {}
        for arc in f['arcs']:
            predecessors.setdefault(arc['to'], set()).add(arc['from'])
        ancestors = set(target_blocks)
        pending = list(target_blocks)
        while pending:
            for block in predecessors.get(pending.pop(), ()):
                if block not in ancestors:
                    ancestors.add(block)
                    pending.append(block)
        # Leak obligations extend from allocation to normal return. Retain
        # later branches as structural context, not as a solver witness.
        leak = any(x['rule'] == 'SA001' for x in evidence['findings'])
        if leak:
            ancestors.update(s['block'] for s in f['statements'] if s['kind'] == 'branch')
        events, other = [], []
        for s in f['statements']:
            at_point = (s['file'], s['line']) in point_keys
            linked = (symbol, s['block'], s['file'], s['line']) in linked_events
            kept = at_point or linked or s['callee'] in toward or s['kind'] == 'branch' and s['block'] in ancestors
            item = {**s, 'problem': (s['file'], s['line']) in problem_keys, 'linked': linked, 'snippet': snippet(s)}
            if kept:
                item['branches'] = [dict(a) for a in f['arcs'] if a['from'] == s['block'] and a['branch'] in ('true', 'false')]
                for arc in item['branches']:
                    destinations = [v for v in f['statements'] if v['block'] == arc['to'] and v['line']]
                    arc['location'] = location(destinations[0]) if destinations else {}
                events.append(item)
            elif s['callee']:
                other.append(item)
        selected[symbol] = {'name': f['name'], 'symbol': symbol, 'location': location(f),
                            'from': incoming.get(symbol), 'events': events, 'other': other}
    return {'functions': selected, 'routes': routes, 'truncated': graph.get('truncated', False),
            'entry_recorded': bool(roots), 'available': bool(functions)}


def presentation_data(report: dict, output: Path, speed: dict | None = None,
                      expand_problems: bool = False, graphs: dict | None = None) -> dict:
    groups = []
    source_cache = {}
    expected_hashes = report.get('recorded_input_hashes', {})
    def snippet(point):
        name, line = point.get('file', ''), point.get('line', 0)
        if not name or not line or name not in expected_hashes:
            return []
        if name not in source_cache:
            p = Path(name)
            raw = p.read_bytes() if p.is_file() and p.stat().st_size <= 1024 * 1024 else b''
            source_cache[name] = raw.decode('utf-8', errors='replace').splitlines() if hashlib.sha256(raw).hexdigest() == expected_hashes[name] else []
        lines = source_cache[name]
        return [{'line': i+1, 'text': lines[i], 'focus': i+1 == line}
                for i in range(max(0, line-2), min(len(lines), line+1))]
    overlap = {"both": 0, "plugin_only": 0, "gcc_only": 0, "neither": 0}
    for group in report["groups"]:
        variants = group["variants"]
        p = variants["bad"]["plugin"]["case_detection_classification"]
        g = variants["bad"]["gcc16_fanalyzer"]["case_detection_classification"]
        if p in ("TP", "FN") and g in ("TP", "FN"):
            overlap["both" if p == g == "TP" else "plugin_only" if p == "TP" else
                    "gcc_only" if g == "TP" else "neither"] += 1
        modes = variants["bad"].get("project_modes", {})
        mode = modes.get("plugin", {})
        folder = Path(group["artifact_directory"])
        links = {v: relative_link(folder / v / "project.json", output) for v in ("bad", "good")}
        if mode.get("plugin_html_path"):
            links["analysis"] = relative_link(mode["plugin_html_path"], output)
        evidence = analysis_evidence(mode.get("plugin_report", {}))
        for finding in evidence['findings']:
            finding['location']['snippet'] = snippet(finding['location'])
            for point in finding['trace']:
                point['snippet'] = snippet(point)
        graph_record = (graphs or {}).get('graphs', {}).get(group['group_id'], {})
        graph = graph_record.get('graph', mode.get('plugin_report', {}).get('call_graph', {}))
        evidence['graph'] = focused_graph(graph, evidence, snippet)
        groups.append({"id": group["group_id"], "cwe": group["cwe"],
                       "bad": {t: variants["bad"][t]["case_detection_classification"] for t in TOOLS},
                       "good": {t: variants["good"][t]["case_detection_classification"] for t in TOOLS},
                       "evidence": evidence, "links": links})
    return {"status": report["status"], "selected": report["selected_groups"],
            "paired": report["common_paired_groups"], "sha256": report.get("plugin_sha256", ""),
            "accuracy": report["common_paired_accuracy"], "by_cwe": report.get("common_paired_by_cwe", {}),
            "timing": report.get("timing", {}), "cached": bool(report.get("reference_cache")),
            "speed": speed_evidence(report, speed), "overlap": overlap, "groups": groups,
            "sources": source_cache,
            "expand": expand_problems,
            "triage": relative_link(output.parent / "triage" / "triage.html", output)
                      if (output.parent / "triage" / "triage.html").exists() else None}


def render_html(report: dict, output: Path, speed: dict | None = None,
                expand_problems: bool = False, graphs: dict | None = None,
                detection_only: bool = False) -> str:
    data = presentation_data(report, output, speed, expand_problems, graphs)
    data["detection_only"] = detection_only
    if detection_only:
        data["detection_summary"] = {
            "total": len(data["groups"]),
            "detected": sum(g["bad"]["plugin"] == "TP" for g in data["groups"]),
            "missed": sum(g["bad"]["plugin"] == "FN" for g in data["groups"]),
            "findings": sum(
                g["variants"]["bad"].get("project_modes", {}).get("plugin", {})
                 .get("plugin_report", {}).get("summary", {}).get("total", 0)
                for g in report["groups"]
            ),
        }
    # Escaping '<' also prevents a source-controlled '</script>' from ending
    # the inert data block. Browser code renders evidence using textContent.
    payload = json.dumps(data, ensure_ascii=False, separators=(",", ":"))
    payload = payload.replace("<", "\\u003c").replace(">", "\\u003e").replace("&", "\\u0026")
    payload = payload.replace("\u2028", "\\u2028").replace("\u2029", "\\u2029")
    template = Path(__file__).with_name("project_report_template.html").read_text(encoding="utf-8")
    return template.replace("__REPORT_DATA__", payload)
