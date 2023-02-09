gcc src/inspection_console.c -lncurses -lm -o bin/inspection &
gcc src/command_console.c -lncurses -o bin/command &
gcc src/master.c -o bin/master &
gcc src/motorX.c -o bin/motorX &
gcc src/motorZ.c -o bin/motorZ &
gcc src/world.c -o bin/world