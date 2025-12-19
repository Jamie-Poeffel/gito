.PHONY: build clean run
all: run
VERSION=v2.0.2

run:
	cargo run

build:
	cargo build --release
	copy target\release\gito.exe release\installer\gito.exe

clean:
	cargo clean

msi:
	wix build release\installer\product.wxs -o release\gito-cli-$(VERSION).msi

	del /F /Q release\gito-cli-$(VERSION).wixpdb

