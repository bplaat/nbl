rm -f -r .vscode
if [ "$1" = "clean" ]; then
    rm -f -r nbl.dSYM dump nbl nbl.exe
    exit
fi

if [ "$1" = "format" ]; then
    clang-format -i $(find . -name *.h -o -name *.c)
    exit
fi

if [ "$1" = "debug" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -lm -o nbl || exit
    lldb -- ./nbl $2
    exit
fi

if [ "$1" = "leaks" ]; then
    gcc -g -DDEBUG -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl || exit
    export MallocStackLogging=1
    leaks -atExit -- ./nbl $2 > dump
    unset MallocStackLogging
    exit
fi

gcc -Wall -Wextra -Wshadow -Wpedantic --std=c11 -Iinclude $(find src -name *.c) -o nbl || exit
if [ "$1" = "test" ]; then
    for file in $(find tests -name *.nbl); do
        echo $file
        ./nbl $file
        if [ $? != 0 ]; then
            echo "FAIL"
            exit
        fi
    done
    echo "OK"
else
    ./nbl $2
fi
