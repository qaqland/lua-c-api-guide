MD_FILES := $(wildcard src/*.md)
C_FILES  := $(wildcard src/*.c)

.PHONY: build build-src build-doc fmt fmt-src fmt-doc

build: build-src build-doc

build-src:
	@if [ ! -d build ]; then meson setup build; fi
	meson compile -C build

build-doc:
	mdbook build

fmt: fmt-src fmt-doc

fmt-src:
	clang-format -i $(C_FILES)

fmt-doc:
	@for f in $(MD_FILES); do comrak -i "$$f"; done
