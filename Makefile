.PHONY: build clean run
all: run

run:
	cargo run

build:
	cargo build --release

clean:
	cargo clean