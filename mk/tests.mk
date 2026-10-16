.PHONY: test-python test test-gcc16 check-options

check-options: plugin
	$(PYTHON) tests/check_plugin_options.py --compiler "$(PLUGIN_CC)" \
		--plugin "$(PLUGIN_BINARY)" --output "$(ARTIFACT_DIR)/options"

test-python:
	$(PYTHON) -m unittest discover -s tests -p 'test_*.py' -v
	$(PYTHON) -m py_compile tests/*.py tools/*.py

test: plugin test-python
	$(PYTHON) tests/run.py \
		--compiler $(PLUGIN_CC) \
		--plugin "$(PLUGIN_BINARY)" \
		--manifest tests/manifest.json \
		--output $(TEST_DIR)

test-gcc16: gcc16-plugin
	$(PYTHON) tests/run.py \
		--compiler $(GCC16_CC) \
		--plugin "$(GCC16_PLUGIN_BINARY)" \
		--manifest tests/manifest.json \
		--output $(TEST_DIR)-gcc16
