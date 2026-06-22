#include"utils.hpp"
#include"map.hpp"
#include"player.hpp"
#include"json.hpp"
#include<iostream>
#include<fstream>

std::unordered_map<locationID, Location> world_map;
using json = nlohmann::json;

locationID string_to_id(const std::string& str) {
    if (str == "home") return locationID::home;
    if (str == "desert") return locationID::desert;
    if (str == "oasis") return locationID::oasis;
    if (str == "desert_village") return locationID::desert_village;
    if (str == "unnamed_kingdom") return locationID::unnamed_kingdom;
    if (str == "tavern") return locationID::tavern;
    if (str == "flower_valley") return locationID::flower_valley;
    if (str == "oak_forest") return locationID::oak_forest;
    if (str == "dark_forest") return locationID::dark_forest;
    if (str == "broken_village") return locationID::broken_village;
    if (str == "snowlands") return locationID::snowlands;
    if (str == "snow_village") return locationID::snow_village;
    if (str == "ice_caves") return locationID::ice_caves;
    if (str == "seaport") return locationID::seaport;
    if (str == "iron_island") return locationID::iron_island;
    if (str == "cursed_castle") return locationID::cursed_castle;
    return locationID::home;
}

bool load_map_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "[Ошибка] Не удалось открыть файл карты: " << filename << "\n";
        return false;
    }

    json map_data;
    try {
        file >> map_data;
    } catch (const json::parse_error& e) {
        std::cout << "[Ошибка JSON] Ошибка синтаксиса файла: " << e.what() << "\n";
        return false;
    }

    for (auto& [key, value] : map_data.items()) {
        locationID loc_id = string_to_id(key);
        
        Location loc;
        loc.name = value["name"].get<std::string>();
        loc.description = value["description"].get<std::string>();
        
        for (auto& conn_json : value["connections"]) {
            Connection conn;
            conn.target = string_to_id(conn_json["target"].get<std::string>());
            conn.travel_time = conn_json["time"].get<int>();
            loc.connected_locations.push_back(conn);
        }
        
        world_map[loc_id] = loc;
    }
    
    return true;
}


void map_time(){
    std::time_t now = std::time(nullptr);
    std::tm local_time;

#ifdef _WIN32
    localtime_s(&local_time, &now); 
#else
    localtime_r(&now, &local_time); 
#endif

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << "\n" << std::endl;
}

locationID current_location = locationID::home;


void change_location(){
    clear_screen();

    if (world_map.find(current_location) == world_map.end()) {
        std::cout << "Критическая ошибка: локация не загружена!\n";
        return;
    }
    const Location& current = world_map.at(current_location);
    
    std::cout << "\n--- Доступные пути из локации \"" << current.name << "\" ---\n";
    for (const Connection& conn : current.connected_locations) {
        std::cout << "- " << world_map.at(conn.target).name << "\n";
    }
    
    std::cout << "Куда вы хотите пойти? ";
    std::string choice;
    std::getline(std::cin, choice);
    
    bool moved = false;
    for (const Connection& conn : current.connected_locations) {
        const Location& next_loc = world_map.at(conn.target);
        
        if (next_loc.name == choice) {
            clear_screen();
            if (conn.target == locationID::desert) {
                if (player.has_flag("jafar_raid_ready") && !player.has_flag("jafar_raid_complete")) {
                    std::cout << "\n[!] Как только вы ступаете на барханы Пустыни, из-за песчаной бури вылетают разбойники!\n";
                    std::cout << "Главарь кочевников обнажает кривой ятаган и бросается на вас!\n";
                    
                    wait_for_player();                    
                    Monster bandit = {"Главарь Кочевников", 110, 22, 70};                     
                    if (start_battle(bandit)) { 
                        player.story_flags.push_back("jafar_raid_complete");
                        std::cout << "\n[Система] Вы одолели главаря. Наводка Джафара успешно отработана!\n";
                    } else {
                        return;
                    }
                }
            }
            else if (conn.target == locationID::iron_island) {
                if (!player.has_flag("world_saved")) {
                    std::cout << "\n[!] Воздух Железного острова отравлен! Железный вирус наносит вам 10 HP.\n";
                    player.health -= 10;
                    if (player.health < 0) player.health = 0;
                } else {
                    std::cout << "\nВокруг цветут молодые деревья. Металлический вирус полностью исчез из этих мест!\n";
                }
            }
            else if (conn.target == locationID::cursed_castle) {
                if (!player.has_flag("world_saved")) {
                    std::cout << "\n[!] Вы вошли в эпицентр заражения! Железный вирус яростно разъедает тело! Вы теряете 50 HP.\n";
                    player.health -= 50; 
                    if (player.health < 0) player.health = 0;
                } else {
                    std::cout << "\nБагровый туман рассеялся. Проклятый замок теперь безопасен, а стальное ядро уничтожено.\n";
                }
            }
            else if (conn.target == locationID::oasis) {
                std::cout << "\n[+] Прохладная вода оазиса восстанавливает вам 15 HP!\n";
                player.health += 15;
                if (player.health > player.max_health) player.health = player.max_health;
            }
            if (!player.is_alive()) {
                std::cout << "\n[!] Заражение оказалось смертельным... Вы упали без сил, не успев сделать и шага.\n";
                wait_for_player();
                respawn_player();
                return;
            }

            std::cout << "Вы отправляетесь в локацию \"" << next_loc.name << "\"\n";
            std::cout << "Путь займет " << conn.travel_time << " сек.\n\nВ пути";
            std::cout << std::flush;
            
            for (int i = 0; i < conn.travel_time; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << " ." << std::flush;
            }
            clear_screen();
            std::cout << "\nВы прибыли!\n";
            map_time();

            current_location = conn.target;
            moved = true;         
            break;
        }
    }

    if(!moved){ 
        clear_screen();
        std::cout << "Название локации указано неверно." << "\n";
        time_stop(2);
        clear_screen();
    }
}
