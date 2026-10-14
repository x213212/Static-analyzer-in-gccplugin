ROOT := ..

.PHONY: all unit smoke regression clean

all: unit

unit:
	$(MAKE) -C $(ROOT) test

smoke:
	$(MAKE) -C $(ROOT) smoke

# Explicit opt-in only: this is the historical mixed benchmark harness.
regression:
	$(MAKE) -f Makefile.legacy test.o

clean:
	$(MAKE) -C $(ROOT) clean
