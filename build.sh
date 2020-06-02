if [ "$1" = "release" ]; then
    gcc -s -O2 -Wno-incompatible-pointer-types $(find src -name *.c) -lm -o nbl
else
    gcc -g -DDEBUG -Wall -Wextra -Werror -Wshadow -Wpedantic --std=c99 -Wno-incompatible-pointer-types $(find src -name *.c) -lm -o nbl
fi
