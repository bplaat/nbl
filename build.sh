if [ "$1" = "format" ]; then
    clang-format -i $(find . -name *.h -o -name *.c)
    exit
fi

if [ "$1" = "debug" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -lm -o nbl && lldb -- ./nbl test.nbl
elif [ "$1" = "leaks" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl && leaks -atExit -- ./nbl test.nbl > dump
else
    gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl && ./nbl test.nbl
fi
