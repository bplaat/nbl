if [ "$1" = "format" ]; then
    clang-format -i $(find . -name *.c)
    exit
fi

if [ "$1" = "debug" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 nbl.c -lm -o nbl && lldb -- ./nbl test.nbl
else
    gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 nbl.c -o nbl && ./nbl test.nbl
fi
