#!/usr/bin/env python3
"""Validate cache accounting against actual GCC alias-query dump records."""
from pathlib import Path
import re
import sys


text = Path(sys.argv[1]).read_text()
queries = re.findall(r"^alias-query: .* cache=(hit|miss)$", text, re.M)
summary = re.search(r"^alias-oracle summary: (.*)$", text, re.M)
if summary is None:
    raise SystemExit("missing alias cache summary")
counts = {key: int(value) for key, value in re.findall(r"([\w-]+)=(\d+)", summary[1])}
assert len(queries) == counts["cache-requests"]
assert queries.count("hit") == counts["cache-hits"] > 0
assert queries.count("miss") == counts["cache-evaluations"] == counts["cache-entries"] == 3
assert counts["cache-requests"] == counts["cache-hits"] + counts["cache-evaluations"]
assert counts["cache-cap-skips"] == 0
assert "relation=no-alias" in text and "relation=same-value" in text
print("alias cache: recorded queries match requests, hits and three unique evaluations")
