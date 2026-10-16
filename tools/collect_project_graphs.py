#!/usr/bin/env python3
"""Replay retained LTO links for structural graphs, without rerunning detectors."""
from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
from pathlib import Path
import shutil
import subprocess


def sha(path: Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def collect(group, plugin, output):
    mode = group['variants']['bad']['project_modes']['plugin']
    original = mode['link']['command']
    folder = output / Path(group['artifact_directory']).name
    folder.mkdir(parents=True, exist_ok=True)
    command = []
    skip = False
    for arg in original:
        if skip:
            skip = False
            continue
        if arg == '-o':
            skip = True
            continue
        if arg.startswith(('-fplugin=', '-fplugin-arg-newplugin-')):
            continue
        command.append(arg)
    objects = [Path(v) for v in command if v.endswith('.o')]
    before = {str(p): sha(p) for p in objects}
    stem = folder / 'analysis'
    command += [f'-fplugin={plugin}', *[
        '-fplugin-arg-newplugin-' + value for value in (
            'analysispolicy=proof-only', 'memoryleakcheck=0', 'doublefreecheck=0', 'useafterfree=0', 'threadmod=0',
            'report=1', 'reporthtml=0', 'reportcallgraph=1', f'reportpath={stem}')],
        '-o', str(folder / 'linked-output')]
    process = subprocess.run(command, capture_output=True, timeout=30, check=False)
    (folder / 'link.log').write_bytes(process.stdout + process.stderr)
    if process.returncode:
        raise ValueError(f'graph link failed: {folder / "link.log"}')
    document = json.loads(stem.with_suffix('.json').read_text())
    if document['summary']['total'] != 0 or not document.get('call_graph', {}).get('enabled'):
        raise ValueError('graph-only replay unexpectedly ran detectors or omitted the graph')
    if before != {str(p): sha(p) for p in objects}:
        raise ValueError('retained LTO object changed during graph replay')
    return {'status': 'OK', 'graph': document['call_graph'], 'command': command,
            'object_hashes': before, 'graph_report': str(stem.with_suffix('.json'))}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--report', type=Path, required=True)
    parser.add_argument('--plugin', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--jobs', type=int, default=8)
    parser.add_argument('--limit', type=int)
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    document = json.loads(args.report.read_text())
    if document.get('status') != 'OK' or document.get('changed_inputs') != []:
        parser.error('requires completed project evidence without input drift')
    for group in document['groups']:
        for source in [*group['sources'], *group.get('provider_sources', [])]:
            if sha(Path(source)) != document['recorded_input_hashes'].get(source):
                parser.error('source differs from the measured benchmark: ' + source)
    compiler = Path(document['compiler']['executable'])
    compiler_hash = sha(compiler)
    if compiler_hash != document['recorded_input_hashes'].get(str(compiler)):
        parser.error('compiler differs from the measured benchmark')
    plugin = output / 'plugin-snapshot' / 'newplugin.so'
    plugin.parent.mkdir(exist_ok=True)
    shutil.copyfile(args.plugin.resolve(), plugin)
    plugin_hash, report_hash = sha(plugin), sha(args.report)
    groups = document['groups'][:args.limit] if args.limit else document['groups']
    results = {}
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        tasks = {pool.submit(collect, g, plugin, output): g['group_id'] for g in groups}
        for task in as_completed(tasks):
            key = tasks[task]
            try:
                results[key] = task.result()
            except Exception as error:
                results[key] = {'status': 'ERROR', 'error': str(error)}
            if len(results) % 100 == 0 or len(results) == len(groups):
                print(f'call graphs {len(results)}/{len(groups)}', flush=True)
    drift = sha(plugin) != plugin_hash or sha(args.report) != report_hash or sha(compiler) != compiler_hash
    failed = sum(v['status'] != 'OK' for v in results.values())
    index = {'status': 'INPUT_DRIFT' if drift else 'INCOMPLETE' if failed else 'OK',
             'graph_only': True, 'detection_counts_recomputed': False,
             'report_sha256': report_hash, 'plugin_sha256': plugin_hash,
             'compiler_sha256': compiler_hash, 'graphs': results}
    (output / 'graphs.json').write_text(json.dumps(index, ensure_ascii=False, separators=(',', ':')))
    print(index['status'], 'failed', failed, output / 'graphs.json')
    return 0 if index['status'] == 'OK' else 1


if __name__ == '__main__':
    raise SystemExit(main())
