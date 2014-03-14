.PHONY: all src clean run echo http

all: src run

src:
	$(MAKE) -C src

run:
	$(MAKE) -C example

http: src
	$(MAKE) -C example http

echo: src
	$(MAKE) -C example echo

clean:
	$(MAKE) -C src clean
