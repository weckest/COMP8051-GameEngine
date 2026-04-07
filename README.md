# The Final Corpse
Game Engine Development Project for COMP-8051

## Instructions for Running:
If running normally, just launch the standalone .exe file.
Otherwise, you can run the project through the cmake-build-debug folder.

## Architecture
This C/C++ game was built using SDL within an ECS environment. There are a number of additional managers and behavior files as well.

## Debug Tools
Press `ESC` during gameplay in order to enable debug mode, which displays timers on sections of the game update function. Additional keybinds are available during this mode.

### Debug Keybinds
`Esc`: Toggle Debug Mode \
`G`: Displays the grid lines used for handling collisions, as well as the number of entities that exist within each cell. \
`J`: Displays the distance lines between the player and other spawned entities. \
`C`: Draws boxes around all entities representing their sprite and collider sizes. Colliders change colors as they move between grid cells. \
`N`: Kills all enemy entities on the map. \
`M`: Spawns a Magnet pickup on the player. \
`L`: Prints the player's level to the console. \
`.`: Stops all enemy movement. \
`/`: Prevents all entities from spawning.

## Assets
All visual and most audio assets were sourced from publicly available sources, which required no direct attribution.

Music was made in-house by Zynity.

## Development Team
Weckest \
Flanks \
HeckB \
Zynity