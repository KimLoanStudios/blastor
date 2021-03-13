#pragma once
#include <unordered_map>
#include "types.hpp"
#include "event.hpp"

struct Player {
    vec2f pos;
};

struct GameState {
    u64 my_player_id;
    std::unordered_map<u64, Player> players;

    void apply_events(const std::vector<Event>& events) {
        for(auto&& event: events) {
            if(std::holds_alternative<PlayerPos>(event.content)) {
                PlayerPos player_pos = std::get<PlayerPos>(event.content);

                players[player_pos.player_id].pos = player_pos.pos;
            }
        }
    }
};