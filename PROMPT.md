# Roguelike Text Game in C

## Build Target
- Language: C (C99)
- Single executable: `game`
- Build command: `gcc -o game *.c -lncurses`
- Platform: Linux/WSL

## File Structure
- main.c       — entry point, game loop
- map.c / map.h        — dungeon generation
- player.c / player.h  — player state, movement, combat
- monster.c / monster.h — monster types, AI
- render.c / render.h  — ncurses display layer
- items.c / items.h    — inventory, items

## Required Features
1. Procedural dungeon via BSP (binary space partitioning) room generation
2. Field of view (shadowcasting algorithm)
3. Turn-based movement on a grid
4. At least 5 monster types with simple chase AI
5. Basic inventory (pickup, use, drop)
6. Permadeath — no save system
7. Depth progression — stairs down, difficulty scales per level
8. Status bar showing HP, gold, dungeon level, current weapon

## Win / Lose Conditions
- Win: Retrieve the Amulet of Yendor on level 10 and escape
- Lose: HP reaches 0

## Coding Conventions
- No global mutable state except the main game_state struct
- All allocations checked, no silent malloc failures
- ncurses init/teardown must be signal-safe (handle SIGINT cleanly)

## Definition of Done
`gcc -o game *.c -lncurses` compiles with zero warnings (-Wall -Wextra)
Running `./game` launches without crashing and renders a dungeon on terminal.
