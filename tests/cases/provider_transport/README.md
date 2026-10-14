# Cross-partition provider-summary fixtures

These are independent multi-translation-unit link tests, not single-file
manifest cases. Compile each source separately, then link its group with LTO.
Never execute the positive UAF programs or the signature-mismatch program.
The ordinary C and C++ examples contain no analyzer contracts. The GCC-only
`noinline,noclone` attributes and volatile loads preserve the provider call and
original pointer formal; they do not promise anything about ownership.

| Group | Sources (in this directory) | Required proof-only result |
| --- | --- | --- |
| C positive | `positive_c_caller.c`, `positive_c_provider.c` | SA001: 1; SA003: 1 |
| C++ positive | `positive_cpp_caller.cpp`, `positive_cpp_provider.cpp` | SA001: 1; SA003: 1; genuinely mangled provider symbols |
| Prevailing body | `weak_caller.c`, `weak_provider.c`, `weak_strong_provider.c` | No findings; the strong freeing implementation wins |
| Callback | `callback_caller.c`, `callback_provider.c` | No findings; forwarding may free, never assume borrow |
| Interposition | `interposable_caller.c`, `interposable_provider.c` | Reject a proof-only SA001 based on the unbound default body |
| Signature rejection | `mismatch_caller.c`, `mismatch_provider.c` | Reject the mismatched call/signature; expect `-Wlto-type-mismatch` |

SA002 must be zero throughout. The interposition group is a theorem-rejection
test, **not** a universally leak-free program. The intentionally incompatible C
declarations in the signature group produce undefined behavior; **exclude this
group from accuracy, TP, FP, and TN totals**. It tests fail-closed binding only.
The weak and ELF interposition cases intentionally require GNU/ELF linking;
the positive and callback source logic also compiles as ordinary C/C++.

## Minimal harness contract

Run every group with `-flto-partition=none` and `-flto-partition=1to1`, using fresh
output directories. A representative C-positive compiler-only control is:

```sh
fixture_dir=/absolute/path/to/tests/cases/provider_transport
probe_dir=$(mktemp -d /tmp/provider-transport.XXXXXX)
gcc_path=/root/toolchains/gcc-16.2/bin/gcc
"$gcc_path" -O1 -flto -fno-ipa-sra -fno-ipa-cp -c \
  "$fixture_dir/positive_c_caller.c" -o "$probe_dir/caller.o"
"$gcc_path" -O1 -flto -fno-ipa-sra -fno-ipa-cp -c \
  "$fixture_dir/positive_c_provider.c" -o "$probe_dir/provider.o"
"$gcc_path" -O1 -flto -flto-partition=1to1 -fno-ipa-sra -fno-ipa-cp \
  -save-temps -fdump-ipa-all-details "$probe_dir/caller.o" \
  "$probe_dir/provider.o" -o "$probe_dir/positive"
```

For the analyzer run, load the same immutable, compiler-ABI-matched plugin
with `-fplugin=/absolute/path/to/plugin.so` on **both producer compilations and
the final link**. A link-only plugin cannot recover metadata that was never
generated at compile time. Use the project's established proof-only options,
`-fipa-pta`, and report handling; keep options identical between the two LTO
partition modes. The `-fno-ipa-sra -fno-ipa-cp` controls isolate serialization
from changed-formal and clone handling. Follow with ordinary optimization
stress runs, recording conservative rejection of changed providers rather
than demanding unsafe reuse.

Use `g++ -std=c++11` for the C++ pair. For interposition, add `-fPIC
-fsemantic-interposition` to every compile and link, and `-shared` to the link;
do not add `-Bsymbolic`, hidden visibility, or `-fno-semantic-interposition`.
For the weak group, link **all three** objects so the strong provider prevails.
Do not suppress the signature group's LTO type-mismatch warning.

Do not count matching reports alone as a transport pass:

1. In producer dumps, require recorded facts for the positive providers.
2. In the actual caller LTRANS partition, require the provider call to survive
   and its target to be `in_other_partition`, without a loaded body. If GCC
   places both bodies together, that run tests local analysis, not transport.
3. Require imported-fact hits and proof provenance tied to the bound provider
   in the caller's findings. The positive totals must match `partition=none`.
4. Collect **all** LTRANS process reports using distinct output paths. A shared
   report stem can be overwritten, hiding findings or failures.
5. Recompile a control set without the producer plugin, then link with the
   plugin. Remote-body-dependent proofs must not appear merely from a caller
   declaration. A `partition=none` local-body proof remains legitimate.
6. Check gate rejection/provenance for interposition and signature mismatch;
   zero findings alone does not show that a stale fact was safely rejected.

Do not reuse a GCC 10 plugin binary in GCC 16 or vice versa. Do not modify the
in-use `Compiler/newplugin.so`; snapshot the tested binary and record its hash.

Compiler-only verification: all 13 translation units compiled and all six
groups linked in both partition modes with GCC 10 and GCC 16.2. The positive
C and C++ caller dumps explicitly contained remote `in_other_partition`
providers in `1to1`, with two LTRANS jobs per link. Both compilers emitted the
expected mismatch warning. This checks fixture construction, **not** plugin
transport correctness; the analyzer acceptance checks above remain separate.
