#include "game.hpp"
#include "event.hpp"
#include "server.hpp"
#include "gameconfig.hpp"
#include <string.h>

int main(int argc, const char* argv[]) {
    try {
        if(argc >= 2) {
            if(strcmp(argv[1], "--server") == 0) {
                return run_server(argc - 2, argv + 2);
            }

            if(strcmp(argv[1], "--test") == 0) {
                test_event_serialization();
                return 0;
            }
        }

        // Else run game
        GameConfig game_config(argc - 1, argv + 1);
        return run_game(game_config);

    } catch(std::exception& e) {
        std::cout << "OH NO: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
