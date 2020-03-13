#!/bin/bash
if gcc -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 $(find src -name *.c) -o nbl.exe; then
    ./nbl test.nbl
fi
