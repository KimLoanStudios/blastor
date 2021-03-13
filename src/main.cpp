#include "game.hpp"
#include "server.hpp"
#include <string.h>

int main(int argc, const char* argv[]) {
    if(argc < 2) {
        return run_game(argc, argv);
    }

    if(strcmp(argv[1], "--server") == 0) {
        run_server(argc, argv);
    }

    return 0;
}