build/bin/clang-noodle: build/Makefile matcher.h LoopConvert.cpp
	make -C build

build/Makefile: CMakeLists.txt
	mkdir -p build && cd build && cmake ..

matcher.h: matcher.rkt convert-matcher.rkt
	>matcher.h <matcher.rkt racket convert-matcher.rkt
	clang-format -i matcher.h

corpus/code.ast.json: corpus/code.cpp Makefile
	clang -Xclang -ast-dump=json -c -o /dev/null $< >$@

.PHONY: format
format:
	clang-format -i LoopConvert.cpp
