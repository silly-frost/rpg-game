#ifndef PLAYER_HPP
#define PLAYER_HPP

#include<string>
#include<vector>

extern const std::string PLAYER_NAME;

struct Player{
    std::string name;
    int health;
    int gold;
    int max_health;
    int armor;
    int damage;
    std::string weapon_name;
    std::vector<std::string> inventory;
    std::vector<std::string> story_flags;

    bool is_alive() const {return health > 0;}

    bool has_flag(const std::string& flag) const {
        for (const auto& f : story_flags) if (f == flag) return true;
        return false;
    }
};

extern Player player;

void show_player_status();

void save_game();
bool load_game();
void respawn_player();

#endif
