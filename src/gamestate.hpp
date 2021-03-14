#pragma once

#include <unordered_map>
#include <span>
#include <iostream>

#include "types.hpp"
#include "event.hpp"

struct Player {
    vec2f pos;
    vec2f look_dir;

    bool dead = false;
    i32 score = 0;
    std::string name;
};

struct Bullet {
    u64 owner_id;
    vec2f pos;
    vec2f direction;
};

struct Box {
    vec2f pos;
    vec2f size;

    bool is_inside(vec2f point) {
        return point.x >= pos.x && point.x <= pos.x + size.x
            && point.y >= pos.y && point.y <= pos.y + size.y;
    }
};

struct GameState {
    std::unordered_map<u64, Player> players;
    std::unordered_map<u64, Bullet> bullets;
    std::unordered_map<u64, Box> boxes;

    void apply_events(const std::span<Event>& events) {
        for (auto&& event: events) {
            if (std::holds_alternative<PlayerPos>(event.content)) {
                PlayerPos player_pos = std::get<PlayerPos>(event.content);
                if (players.find(player_pos.player_id) == players.end())
                    return;

                players[player_pos.player_id].pos = player_pos.pos;
                players[player_pos.player_id].look_dir = player_pos.look_dir;
            }
            if (std::holds_alternative<PlayerRemove>(event.content)) {
                auto player_remove = std::get<PlayerRemove>(event.content);
                players.erase(player_remove.player_id);
            }
            if (std::holds_alternative<BulletShot>(event.content)) {
                auto shot = std::get<BulletShot>(event.content);
                bullets[shot.bullet_id] = Bullet {
                    .owner_id = shot.owner_id,
                    .pos = shot.pos,
                    .direction = shot.direction,
                };
            }
            if (std::holds_alternative<BulletPos>(event.content)) {
                auto bp = std::get<BulletPos>(event.content);
                if (bullets.find(bp.bullet_id) != bullets.end())
                    bullets[bp.bullet_id].pos = bp.pos;
            }
            if (std::holds_alternative<BulletRemove>(event.content)) {
                auto bp = std::get<BulletRemove>(event.content);
                bullets.erase(bp.bullet_id);
            }
            if (std::holds_alternative<PlayerStatsChange>(event.content)) {
                auto s = std::get<PlayerStatsChange>(event.content);

                Player& player = players[s.player_id];
                player.dead = s.dead;
                player.score = s.score;
                player.name = s.name;

                std::cout << "player state change\n"
                    << "    id = " << s.player_id << "\n"
                    << "    score = " << s.score << "\n"
                    << "    dead = " << s.dead << "\n"
                    << "    name = " << s.name << std::endl;
            }
            if(std::holds_alternative<BoxAdded>(event.content)) {
                BoxAdded box_added = std::get<BoxAdded>(event.content);

                boxes[box_added.box_id] = Box {
                    .pos = box_added.pos,
                    .size = box_added.size,
                };
            }
        }
    }
};
