# Run './build.sh' to build and run the interpreter
# Run './build.sh debug' when you want to compile and debug the interpreter
# Run './build.sh release' when you want to compile to a smaller executable
if [[ $1 = "release" ]]; then
    tcc $(find src -name *.c) -o nbl.exe
elif [[ $1 = "debug" ]]; then
    if gcc -g -DDEBUG -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 $(find src -name *.c) -o nbl.exe; then
        drmemory -- ./nbl test.nbl
    fi
else
    if gcc -DDEBUG -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 $(find src -name *.c) -o nbl.exe; then
        ./nbl
    fi
fi
