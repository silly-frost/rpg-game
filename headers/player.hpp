#pragma once

#include<string>
#include<vector>

extern const std::string PLAYER_NAME;

struct Player {
    std::string name;
    int health;
    int gold;
    int max_health;
    
    // Базовые статы игрока (голышом, без оружия и брони)
    int base_damage; 
    int base_armor;

    // Слоты экипировки (Хранят точное название надетого предмета)
    std::string equipped_weapon; // Например: "Ржавый_меч"
    std::string equipped_armor;  // Например: "Тканевая_одежда"

    std::vector<std::string> inventory;
    std::vector<std::string> story_flags;

    bool is_alive() const { return health > 0; }

    bool has_flag(const std::string& flag) const {
        for (const auto& f : story_flags) if (f == flag) return true;
        return false;
    }

    // Динамический расчёт характеристик (С учетом экипировки)
    int get_damage() const; 
    int get_armor() const; 
};

extern Player player;

void show_player_status();

void save_game();
bool load_game();
void respawn_player();
