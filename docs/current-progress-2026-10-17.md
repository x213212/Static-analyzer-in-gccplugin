# Current progress — 2026-10-17

`main` now contains only the modular bounded engine, version `1.0.0-rc3`.
The historical headers, adapter, old-only options and benchmark Makefile are
removed. No source compatibility shim is retained. The core SSA, ownership,
CFG/Z3, SDK review, call graph and report modules remain.

See the [module/build map](core-only-refactor-2026-10-17.md) for the removal
scope, supported commands, and options to remove from external build scripts.

## Validation

| Check | Result |
| --- | --- |
| Python suite, including clean preserving reports | 139/139 |
| GCC10 current-engine compiler cases | 150/150 |
| GCC16 current-engine compiler cases | 150/150 |
| GCC16 per-case semantic JSON versus pre-removal binary | 150/150 identical apart from tool version |
| Removed options, each with values 0 and 1 | 18 rejected per compiler |
| Original issue #20 snippets, no workaround/options | 5/5 compile on GCC10 and GCC16 |
| Current `.so` historical symbols | Adapter, old DFS, `PointerConstraint`, `fprintf2` absent |
| Build without warning suppression | `-Werror=return-type` enabled |
| Make smoke tasks: alias/memory caches, path scaling, SSA origin cap, solver/CPU/work budgets | 8/8 |

The new GCC16 binary is 14,001,064 bytes versus the prior report-layout build's
16,640,200 bytes. This is a file-size observation, not an isolated analysis
speed measurement. GCC13.3 is not installed and has not been retested.

## Fresh C LTO replay

All 1,234 groups, each with a bad and good variant, completed. Current plugin
objects and links were rerun; native GCC16 diagnostics came from a validated
cache and were scored again with the current parser.

| Measurement | Current plugin | Cached GCC16 reference |
| --- | ---: | ---: |
| Expected-defect case detections | 1,123 | 974 |
| Missed expected defects | 111 | 260 |
| Target-rule alerts on good variants | 0 | 48 |
| Exact oracle-site detections | 293 | 398 |

The bad variants contain 1,135 actual finding records. Case count and finding
count are different quantities. Additionally, 123 good variants receive a
lifetime alert for another rule; those alerts are retained rather than hidden
behind the target-rule zero. They are not all independently adjudicated false
positives. This suite does not establish general accuracy on real projects.

The only case-detection difference from the 2026-10-15 run is
`416:CWE416_Use_After_Free__malloc_free_int64_t_10`. It previously hit the
100ms function CPU cap, and this replay finishes its proof within the same cap.
No detection algorithm or resource limit was changed to obtain that result.
Keep it identified as budget-sensitive run variation.

The replay took 79.850 seconds with eight workers. Cached native timing is not
a speed comparison. Prior serial/FFmpeg measurements retain their original
2026-10-15 binary fingerprints and are historical evidence.

The public [summary](measurements/2026-10-17-summary.json) records counts,
binary hashes and scope without machine paths. Full local records are under
`artifacts/core-only-20261017/`; they are not committed.

## Open the whole detection list

```sh
python3 tools/render_project_report.py \
  --report artifacts/core-only-20261017/full-c/comparison.json --detections-only
```

This writes an offline bilingual `detection.html` beside the input. It opens
on the paginated list, separates all cases/detected/missed/finding records,
and expands source/trace evidence in the same page. It does not rerun analysis.
Original recorded source hashes must still match to display source code.

Reports and library-model assumptions keep their existing proof status.
Automatic source repair and full binary-derived ownership proofs remain
unimplemented.
