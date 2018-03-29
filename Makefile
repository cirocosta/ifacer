# Builds main binary responsible for running
# the HSTATIC server (`./hstatic.out`).
build: ./main.c
	gcc -O2 -static -Wall $^ -o ./main.out


# Formats any C-related file using the clang-format
# definition at the root of the project.
#
# Make sure you have clang-format installed before
# executing.
fmt:
	find . -name "*.c" -o -name "*.h" | \
		xargs clang-format -style=file -i


# Removes any binary generated.
clean:
	find . -name "*.out" -type f -delete


.PHONY: build fmt clean test functional
