#include "game.hpp"
#include "event.hpp"
#include "server.hpp"
#include <string.h>

int main(int argc, const char* argv[]) {
    if(argc < 2) {
        return run_game(argc - 2, argv + 2);
    }

    if(strcmp(argv[1], "--server") == 0) {
        run_server(argc - 2, argv + 2);
    }

    if(strcmp(argv[1], "--test") == 0) {
        test_event_serialization();
    }

    return 0;
}
