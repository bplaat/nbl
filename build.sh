if [ "$1" = "format" ]; then
    clang-format -i $(find . -name *.h -o -name *.c)
    exit
fi

if [ "$1" = "repl" ] || [ "$2" = "repl" ]; then
    command="./nbl"
else
    command="./nbl test.nbl Bastiaan Leonard Sander Jiska"
fi
if [ "$1" = "debug" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -lm -o nbl && lldb -- $command
elif [ "$1" = "leaks" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl &&
    export MallocStackLogging=1
    leaks -atExit -- $command > dump
    unset MallocStackLogging
else
    gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl && $command
fi
