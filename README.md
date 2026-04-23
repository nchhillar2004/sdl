# Games using SDL3

This repository contains recreation of few classic video games like snake game, pong, flappy bird, etc... Implemented in
C Programming Language using SDL3 library for graphics.

Before compiling from source, you need to have [SDL3](https://github.com/libsdl-org/SDL) and 
[SDL3_ttf](https://github.com/libsdl-org/SDL_ttf) on your system.

How to compile and run a game:

```bash
gcc game.c snake.c -o snake.out -lSDL3 -lSDL3_ttf
./snake.out
```

hot-reload using [entr](https://github.com/eradman/entr):
```bash
ls game.c flappy.c | entr -r sh -c 'clear && gcc game.c flappy.c -lSDL3 -lSDL3_ttf && ./a.out'
```

## Screenshots

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/70429c47-41a7-423b-9330-8a4ded55b5e8" />
