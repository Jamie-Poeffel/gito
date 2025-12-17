.PHONY: build clean run
all: run

run:
	cargo run

build:
	cargo build --release
	copy target\release\gito.exe release\gito.exe

clean:
	cargo clean

msi:
	wix build release\installer\product.wxs -o release\gito.msi

