gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -lm -o nbl || exit
./nbl
