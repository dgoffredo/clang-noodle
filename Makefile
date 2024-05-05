matcher.h: matcher.rkt convert.rkt
	>matcher.h <matcher.rkt racket convert.rkt
	clang-format -i matcher.h
