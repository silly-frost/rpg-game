#include"map.hpp"
#include"player.hpp"
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>

const std::string PLAYER_NAME = []() {
#ifdef _WIN32
    const char* env_p = std::getenv("USERNAME");
#else
    const char* env_p = std::getenv("USER");
#endif
    return (env_p != nullptr) ? env_p : "Странник";
}();

Player player = {
    PLAYER_NAME,
    100,
    0,
    100,
    0,
    15,
    "Ржавый_меч",
    {"Ржавый_меч"},
    {}
}; 

void show_player_status() {
  while (true) {
        extern void clear_screen();
        clear_screen();

        std::cout << "\n========================================" << "\n";
        std::cout << " ИГРОК: " << player.name << "\n";
        std::cout << " Здоровье: " << player.health << " / " << player.max_health << "\n";
        std::cout << " Защита (Броня): " << player.armor << "\n";
        
        std::string weapon_display = player.weapon_name;
        for (char& c : weapon_display) if (c == '_') c = ' ';
        std::cout << " Оружие: \"" << weapon_display << "\" | Урон: " << player.damage << "\n";
        std::cout << " Золото: " << player.gold << " монет\n";
        std::cout << " Текущее место: " << world_map.at(current_location).name << "\n\n";
        
        std::cout << " Инвентарь: ";
        if (player.inventory.empty()) {
            std::cout << "Пусто\n";
        } else {
            std::cout << "\n";
            for (size_t i = 0; i < player.inventory.size(); ++i) {
                std::string item_display = player.inventory[i];
                for (char& c : item_display) if (c == '_') c = ' ';
                std::cout << "  " << i + 1 << ". [" << item_display << "]\n";
            }
        }
        std::cout << "========================================" << "\n";

        if (player.inventory.empty()) {
            extern void wait_for_player();
            wait_for_player();
            break;
        }

        std::cout << "Выберите номер предмета, чтобы использовать его (или 0, чтобы выйти): ";
        int item_choice;
        if (!(std::cin >> item_choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(1, '\n');

        if (item_choice == 0) break;
        if (item_choice < 1 || item_choice > (int)player.inventory.size()) {
            std::cout << "\n[Система] Неверный номер предмета!\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        std::string chosen_item = player.inventory[item_choice - 1];
        bool item_used = false;

        // Логика лечения
        if (chosen_item == "Настойка_трав") {
            if (player.health == player.max_health) {
                std::cout << "\n[Система] У вас и так полное здоровье!\n";
            } 
            else {
                player.health += 15;
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы выпили Настойку трав и восстановили 15 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Таинственное_Зелье") { 
            if (player.health == player.max_health) {
                std::cout << "\n[Система] У вас и так полное здоровье! Использовать зелье ради урона в мирной зоне не имеет смысла.\n";
            } 
            else {
                player.health += 60;
                player.damage += 10;
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы выпиваете Таинственное зелье, восстанавливаете 60 HP и навсегда увеличиваете урон на 10!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Пиво") {
            if (player.health == player.max_health) {
                std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n";
            } 
            else {
                player.health += 20; 
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Торопясь, вы выпиваете Пиво и восстанавливаете 20 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Медовуха") {
            if (player.health == player.max_health) {
                std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n";
            } 
            else {
                player.health += 40; 
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы с удовольствием выпиваете Медовуху и восстанавливаете 40 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Крепкий_Эль") {
            if (player.health == player.max_health) {
                std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n";
            } 
            else {
                player.health += 50;
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы залпом выпиваете Крепкий Эль, прожигая горло, и восстанавливаете 50 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Вино") {
            if (player.health == player.max_health) {
                std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n";
            } 
            else {
                player.health += 60; 
                if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы изящно выпиваете Вино и восстанавливаете себе 60 HP!\n";
                item_used = true;
            }
        }
        else {
            // Защита от использования оружия, квестов или брони через сумку
            std::cout << "\n[Система] Предмет [" << chosen_item << "] нельзя использовать таким образом!\n";
        }

        // Если предмет успешно применился — удаляем его из инвентаря
        if (item_used) {
            player.inventory.erase(player.inventory.begin() + (item_choice - 1));
            save_game(); 
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(2));  
    }
}



void save_game() {
    std::ofstream file("save.txt");
    if (!file.is_open()) {
        std::cout << "[Ошибка] Не удалось создать файл сохранения!\n";
        return;
    }

    std::string loc_str = "home";
    if (current_location == locationID::desert) loc_str = "desert";
    else if (current_location == locationID::oasis) loc_str = "oasis";
    else if (current_location == locationID::desert_village) loc_str = "desert_village";
    else if (current_location == locationID::unnamed_kingdom) loc_str = "unnamed_kingdom";
    else if (current_location == locationID::tavern) loc_str = "tavern";
    else if (current_location == locationID::flower_valley) loc_str = "flower_valley";
    else if (current_location == locationID::oak_forest) loc_str = "oak_forest";
    else if (current_location == locationID::dark_forest) loc_str = "dark_forest";
    else if (current_location == locationID::broken_village) loc_str = "broken_village";
    else if (current_location == locationID::snowlands) loc_str = "snowlands";
    else if (current_location == locationID::snow_village) loc_str = "snow_village";
    else if (current_location == locationID::ice_caves) loc_str = "ice_caves";
    else if (current_location == locationID::seaport) loc_str = "seaport";
    else if (current_location == locationID::iron_island) loc_str = "iron_island";
    else if (current_location == locationID::cursed_castle) loc_str = "cursed_castle";

    file << loc_str << "\n";
    file << player.health << "\n";
    file << player.max_health << "\n";
    file << player.gold << "\n";
    file << player.armor << "\n";
    file << player.damage << "\n";
    file << player.weapon_name << "\n"; 

    if (player.inventory.empty()) {
        file << "NONE_ITEMS";
    } else {
        for (size_t i = 0; i < player.inventory.size(); ++i) {
            file << player.inventory[i] << (i + 1 == player.inventory.size() ? "" : " ");
        }
    }
    file << "\n"; 

    if (player.story_flags.empty()) {
        file << "NONE_ITEMS";
    } else {
        for (size_t i = 0; i < player.story_flags.size(); ++i) {
            file << player.story_flags[i] << (i + 1 == player.story_flags.size() ? "" : " ");
        }
    }
    file << "\n";

    std::cout << "[Система] Игра успешно сохранена!\n";
}




bool load_game() {
    std::ifstream file("save.txt");
    if (!file.is_open()) return false;

    std::string loc_str;
    file >> loc_str;
    
    extern locationID string_to_id(const std::string& str);
    current_location = string_to_id(loc_str);

    file >> player.health >> player.max_health >> player.gold >> player.armor >> player.damage >> player.weapon_name;
    
    // Очищаем поток от оставшихся пробелов и переносов строк перед getline
    file >> std::ws;
 
    if (player.max_health > 200) {
        player.max_health = 100;
        std::cout << "[Система] Подозрительно высокий уровень максимального здоровья сброшен до 100!\n";
    }
    if (player.health > player.max_health) {
        player.health = player.max_health;
    }
 
    // Читаем инвентарь
    std::string inv_line;
    std::getline(file, inv_line);
    player.inventory.clear();
    if (!inv_line.empty() && inv_line != "NONE_ITEMS") {
        std::stringstream ss(inv_line); 
        std::string item;
        while (ss >> item) player.inventory.push_back(item);
    }

    // Читаем сюжетные флаги 
    std::string flags_line;
    std::getline(file, flags_line);
    player.story_flags.clear();
    if (!flags_line.empty() && flags_line != "NONE_ITEMS") {
        std::stringstream ss(flags_line); 
        std::string flag;
        while (ss >> flag) player.story_flags.push_back(flag);
    }
    
    return true;
}

void respawn_player() {
    std::cout << "\n========================================\n";
    std::cout << " Свет меркнет в ваших глазах...\n";
    std::cout << " Но неведомая сила возвращает вашу душу назад.\n";
    std::cout << " Вы просыпаетесь в своей постели. Живой.\n";
    std::cout << "========================================\n";
    

    if (player.max_health > 200) player.max_health = 200; 
    player.health = player.max_health;

    current_location = locationID::home;
    
    save_game();
}
