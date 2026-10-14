from __future__ import annotations

import importlib.util
from pathlib import Path
import tempfile
import time
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("binary_trace", ROOT / "tools/trace_binary_callees.py")
TRACE = importlib.util.module_from_spec(spec)
spec.loader.exec_module(TRACE)


class BinaryTraceTest(unittest.TestCase):
    def test_link_definition_not_referencing_object(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            provider, reference = root / "provider.so", root / "caller.o"
            provider.touch(); reference.touch()
            link = root / "link.map"
            link.write_text("Cross Reference Table\nSymbol File\nconsume provider.so\n        caller.o\n")
            self.assertEqual(TRACE.link_bindings(link, root)["consume"], provider.resolve())

    def test_default_symbol_version_only(self):
        entries = [dict(name="consume@V1", address=16, size=5),
                   dict(name="consume@@V2", address=32, size=5)]
        self.assertEqual(TRACE.select_symbol(entries, "consume")["address"], 32)
        self.assertIsNone(TRACE.select_symbol([*entries[:1], dict(name="consume@V3", address=48, size=5)], "consume"))

    def test_unreachable_free_is_not_a_call_observation(self):
        code = TRACE.decode("100: jmp 110 <body+0x10>\n105: call 200 <free@plt>\n110: ret\n")
        self.assertEqual(TRACE.observations(code), [])

    def test_free_of_other_parameter_is_not_free_of_first(self):
        code = TRACE.decode("100: sub rsp,0x8\n104: mov rdi,rsi\n108: call 200 <free@plt>\n")
        self.assertEqual(TRACE.linear_argument_origins(code)[0x108], 1)

    def test_partial_register_write_and_branch_do_not_supply_identity(self):
        code = TRACE.decode("100: mov dil,0x0\n104: call 200 <free@plt>\n")
        self.assertIsNone(TRACE.linear_argument_origins(code)[0x104])
        code = TRACE.decode("100: test esi,esi\n104: je 110 <body+0x10>\n108: call 200 <free@plt>\n110: ret\n")
        self.assertNotIn(0x108, TRACE.linear_argument_origins(code))

    def test_indirect_call_stays_visible(self):
        code = TRACE.decode("100: call QWORD PTR [r14+0x38]\n104: ret\n")
        self.assertTrue(TRACE.observations(code)[0]["indirect"])

    def test_cache_reuses_identity_and_invalidates_changed_binary(self):
        with tempfile.TemporaryDirectory() as tmp:
            reader = TRACE.Inspector(Path(tmp), 4096, time.monotonic() + 5)
            calls = []
            def build():
                calls.append(True)
                return {"ownership_effect": "UNKNOWN", "proof_eligible": False}
            reader.cached({"provider_sha256": "first"}, build)
            reader.cached({"provider_sha256": "first"}, build)
            reader.cached({"provider_sha256": "changed"}, build)
            self.assertEqual(len(calls), 2)
            self.assertEqual((reader.hits, reader.misses), (1, 2))

    def test_unresolved_provider_does_not_become_a_proof(self):
        with tempfile.TemporaryDirectory() as tmp:
            reader = TRACE.Inspector(Path(tmp), 4096, time.monotonic() + 5)
            report = {"summary": {"total": 0}, "callee_effects": [dict(
                symbol="unresolved", needs_review=True, body_available=False)]}
            result = TRACE.trace(report, {}, reader)
            self.assertEqual(result["roots"][0]["status"], "INCONCLUSIVE")
            self.assertEqual(result["functions"], [])
            self.assertEqual(result["original_finding_summary"]["total"], 0)


if __name__ == "__main__":
    unittest.main()
