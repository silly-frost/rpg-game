#ifndef MAP_HPP
#define MAP_HPP

#include<ctime>
#include<string>

struct location{
    int npc_count;        // количество жителей
    int enemy_count;      // количество врагов
    std::string loc_name; // название локации
    int loc_id;           // id локации
}

void map_time(); // время на карте (привязано к времени пользователя)
void go_to_location(); // перемещение к выбранной локации

#endif
