if gcc -DDEBUG -Wall -Wextra -Wpedantic -Werror -Wshadow -std=c99 $(find src -name *.c) -o nbl.exe; then
    ./nbl
fi

# if tcc -DDEBUG $(find src -name *.c) -o nbl.exe; then
#     ./nbl
# fi
