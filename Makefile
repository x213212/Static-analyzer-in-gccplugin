# Current analyzer only. Shared configuration and task rules live in mk/.
.DEFAULT_GOAL := all
SHELL := /bin/sh
.SHELLFLAGS := -eu -c

include mk/config.mk
include mk/build.mk
include mk/reports.mk
include mk/tests.mk
include mk/benchmarks.mk
include mk/smoke.mk
