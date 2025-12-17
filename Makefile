.PHONY: build clean run
all: run

run:
	cargo run

build:
	cargo build

clean:
	cargo clean