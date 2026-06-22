#pragma once

#include<string>
#include<vector>
#include<ctime>
#include<thread>
#include<chrono>
#include<unordered_map>

inline void time_stop(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

enum class locationID {
  // Лесная зона
  home,
  flower_valley,
  dark_forest,
  broken_village,
  oak_forest, 
  
  // Пустынная зона
  desert,
  unnamed_kingdom,
  tavern,
  oasis,
  desert_village,

  // Снежная зона
  snowlands,
  ice_caves,
  snow_village,
  seaport,

  // Проклятые земли
  iron_island,
  cursed_castle,
  forgotten_rocks_island,

  // Степи
  sunny_steppes,
  forgotten_bastion,
  ancient_observatory,
  rotten_swamp
};

struct Connection {
    locationID target; 
    int travel_time;   
};

struct Location {
    std::string id;
    std::string name;
    std::string description;
    std::vector<Connection> connected_locations;
};

extern std::unordered_map<locationID, Location> world_map;
extern locationID current_location;

void map_time();
bool load_map_from_file(const std::string& filename);
void change_location();
