# rogue-aider

A classic terminal roguelike written in C99 with ncurses.

## Build

```
gcc -o game *.c -lncurses
```

Compiles with zero warnings under `-Wall -Wextra`. Requires a terminal of at
least 80x24.

## How to play

- **Move:** arrow keys or `h` `j` `k` `l`; diagonals `y` `b` `n`
- **Bump into a monster** to attack it
- **`g`** — pick up the item under you (gold is collected automatically by
  walking over it)
- **`i` / `u`** — inventory / use an item (drink potions, wield weapons)
- **`d`** — drop an item
- **`>`** — descend stairs, **`<`** — ascend stairs
- **`q`** — quit (asks for confirmation)

## Objective

The Amulet of Yendor is hidden on **dungeon level 10**. Fight your way down,
take the amulet, then climb all the way back up and **ascend the stairs (`<`)
on level 1 while carrying it** to win. If your HP reaches 0, you die —
permanently. There is no save system.

Deeper levels spawn more and tougher monsters (rat, goblin, orc, troll,
dragon) but also better weapons and bigger gold piles.

## Features

- BSP (binary space partitioning) dungeon generation with corridors
- Recursive shadowcasting field of view (radius 8) with explored-tile memory
- Turn-based grid movement and bump combat
- Five monster types with chase/wander AI and distinct stats
- Inventory with pickup, use, and drop
- Depth progression with scaling difficulty and loot
- Status bar showing HP, gold, dungeon level, and current weapon
- Clean SIGINT handling (flag checked in the main loop; no save/load)
