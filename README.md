# Blastor

### Get blasted:
* On Linux:
    * Install `sfml`:
    ```bash
    # Ubuntu:
    apt install libsfml-dev

    # Arch:
    pacman -S sfml
    ```
    * Build the game:
    ```bash
    # To make a debug build:
    make debug -j16

    # To make a release build:
    make release -j16

    # To run the game:
    ./game

    # To run the server:
    ./game --server

    # To make a clean build
    make clean
    make debug/release
    ```
* On Windows
    * Install linux
    * Go to linux installation guide
