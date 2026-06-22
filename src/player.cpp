#include"map.hpp"
#include"player.hpp"
#include"utils.hpp"
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
    PLAYER_NAME,        // name
    100,                // health
    0,                  // gold
    100,                // max_health
    10,                 // base_damage (базовый урон кулаками)
    0,                  // base_armor  (базовая защита без одежды)
    "Ржавый_меч",       // equipped_weapon (на старте надет ржавый меч. Итого: 10 + 5 = 15 урона!)
    "NONE",             // equipped_armor  (на старте брони нет)
    {"Ржавый_меч"},     // inventory (ржавый меч лежит и в слоте, и в сумке, чтобы его можно было сменить)
    {}                  // story_flags
}; 

void show_player_status() {
    while (true) {
        extern void clear_screen();
        clear_screen();

        std::cout << "\n========================================" << "\n";
        std::cout << " ИГРОК: " << player.name << "\n";
        std::cout << " Здоровье: " << player.health << " / " << player.max_health << "\n";
        
        // Отображение брони с учетом надетого доспеха
        std::string armor_display = player.equipped_armor;
        if (armor_display == "NONE") armor_display = "Нет доспеха";
        else { for (char& c : armor_display) if (c == '_') c = ' '; }
        std::cout << " Защита (Броня): " << player.get_armor() << " [Экипировано: " << armor_display << "]\n";
        
        // Отображение оружия с учетом надетого слота
        std::string weapon_display = player.equipped_weapon;
        if (weapon_display == "NONE") weapon_display = "Кулаки";
        else { for (char& c : weapon_display) if (c == '_') c = ' '; }
        std::cout << " Оружие: \"" << weapon_display << "\" | Текущий урон: " << player.get_damage() << "\n";
        
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
                
                std::string status_tag = "";
                if (player.inventory[i] == player.equipped_weapon || player.inventory[i] == player.equipped_armor) {
                    status_tag = " [Экипировано]";
                }
                std::cout << "  " << i + 1 << ". [" << item_display << "]" << status_tag << "\n";
            }
        }
        std::cout << "========================================" << "\n";

        if (player.inventory.empty()) {
            extern void wait_for_player();
            wait_for_player();
            break;
        }

        std::cout << "Выберите номер предмета, чтобы использовать/надеть (или 0, чтобы выйти): ";
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

        
        // Логика оружия
        if (chosen_item == "Ржавый_меч" || chosen_item == "Охотничий_нож" || 
            chosen_item == "Стальной_меч" || chosen_item == "Тяжелая_секира" || 
            chosen_item == "Священный_молот" || chosen_item == "Посох_Света" ||
            chosen_item == "Осквернённый_посох") {
            
            // Ограничение 1: Полный запрет на оскверненный посох Малакара
            if (chosen_item == "Осквернённый_посох") {
                std::cout << "\n[Система] Ржавая скверна обжигает вам руки! Вы чувствуете, как живой металл пытается проникнуть под кожу.\n";
                std::cout << "[Система] Посох Малакара невозможно экипировать в текущем состоянии. Ему нужно очищение...\n";
                wait_for_player();
                continue;
            }

            // Ограничение 2: Запрет на Посох Света, пока игрок не обучился магии у Аурелиуса
            if (chosen_item == "Посох_Света" && !player.has_flag("learned_magic")) {
                std::cout << "\n[Система] Вы держите в руках чистый Искрящийся посох Света, но его магические руны молчат.\n";
                std::cout << "[Система] Вы не умеете направить его энергию. Найдите того, кто обучит вас тайным искусствам.\n";
                wait_for_player();
                continue;
            }

            // Стандартный безопасный блок экипировки / снятия оружия
            if (player.equipped_weapon == chosen_item) {
                std::string weapon_clean = chosen_item;
                for (char& c : weapon_clean) if (c == '_') c = ' ';
                
                std::cout << "\n[Оружие] Предмет [" << weapon_clean << "] уже в руках.\n";
                std::cout << "1. Убрать оружие в ножны (Снять)\n";
                std::cout << "2. Назад\nВыбор: ";
                int unequip_choice; std::cin >> unequip_choice; std::cin.ignore(1, '\n');
                
                if (unequip_choice == 1) {
                    player.equipped_weapon = "NONE"; 
                    std::cout << "\n[Система] Вы убрали оружие. Теперь вы сражаетесь кулаками! Текущий урон: " << player.get_damage() << "\n";
                    save_game();
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            } 
            else {
                player.equipped_weapon = chosen_item;
                std::string weapon_name_clean = chosen_item;
                for (char& c : weapon_name_clean) if (c == '_') c = ' ';
                std::cout << "\n[Оружие] Вы вооружились: [" << weapon_name_clean << "]. Новый урон: " << player.get_damage() << "\n";
                save_game();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
        // Логика брони
        else if (chosen_item == "Кожаный_доспех" || chosen_item == "Стальная_кираса" || chosen_item == "Сверкающий_доспех") {
            
            // Если доспех надет — даем возможность его снять
            if (player.equipped_armor == chosen_item) {
                std::string armor_clean = chosen_item;
                for (char& c : armor_clean) if (c == '_') c = ' ';
                
                std::cout << "\n[Броня] Доспех [" << armor_clean << "] уже надет на вас.\n";
                std::cout << "1. Снять доспех\n";
                std::cout << "2. Назад\nВыбор: ";
                int unequip_choice; std::cin >> unequip_choice; std::cin.ignore(1, '\n');
                
                if (unequip_choice == 1) {
                    player.equipped_armor = "NONE"; // Очищаем слот, защита падает до базовой
                    std::cout << "\n Вы сняли доспех. Ваша защита упала до: " << player.get_armor() << "\n";
                    save_game();
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            } 
            // Если доспех не надет — экипируем его
            else {
                player.equipped_armor = chosen_item;
                std::string armor_name_clean = chosen_item;
                for (char& c : armor_name_clean) if (c == '_') c = ' ';
                std::cout << "\n[Броня] Вы надели доспех: [" << armor_name_clean << "]. Новая защита: " << player.get_armor() << "\n";
                save_game();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }

        // Логика лечения
        else if (chosen_item == "Таинственное_Зелье") {
            if (player.health == player.max_health) { std::cout << "\n[Система] У вас полное здоровье! Использовать зелье ради урона в мирной зоне нет смысла.\n"; } 
            else {
                player.health += 70; 
                if (player.health > player.max_health) player.health = player.max_health;
                item_used = true;

                if(player.base_damage == 20) std::cout << "\n[+] Вы выпиваете таинственное зелье, восстанавливая 70HP!\n";
                else {
                    player.base_damage += 10;
                    std::cout << "\n[+] Вы выпиваете Таинственное зелье, восстанавливаете 70 HP и навсегда увеличиваете базовый урон на 10!\n";
                }
            }
        }
        else if (chosen_item == "Пиво") {
            if (player.health == player.max_health) { std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n"; } 
            else {
                player.health += 20; if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Торопясь, вы выпиваете Пиво и восстанавливаете 20 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Медовуха") {
            if (player.health == player.max_health) { std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n"; } 
            else {
                player.health += 40; if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы с удовольствием выпиваете Медовуху и восстанавливаете 40 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Крепкий_Эль") {
            if (player.health == player.max_health) { std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n"; } 
            else {
                player.health += 50; if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы залпом выпиваете Крепкий Эль, прожигая горло, и восстанавливаете 50 HP!\n";
                item_used = true;
            }
        }
        else if (chosen_item == "Вино") {
            if (player.health == player.max_health) { std::cout << "\n[Система] Вы не хотите переводить ценную выпивку при полном здоровье.\n"; } 
            else {
                player.health += 60; if (player.health > player.max_health) player.health = player.max_health;
                std::cout << "\n[+] Вы изящно выпиваете Вино и восстанавливаете себе 60 HP!\n";
                item_used = true;
            }
        }
        else {
            std::cout << "\n[Система] Предмет [" << chosen_item << "] нельзя использовать!\n";
        }

        // Если предмет был именно выпит/потрачен — удаляем его из сумки
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
    else if (current_location == locationID::sunny_steppes) loc_str = "sunny_steppes";
    else if (current_location == locationID::forgotten_bastion) loc_str = "forgotten_bastion";
    else if (current_location == locationID::rotten_swamp) loc_str = "rotten_swamp";
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
    else if (current_location == locationID::forgotten_rocks_island) loc_str = "forgotten_rocks_island";

    file << loc_str << "\n";
    file << player.health << "\n";
    file << player.max_health << "\n";
    file << player.gold << "\n";
    file << player.base_armor << "\n";
    file << player.base_damage << "\n";
    file << player.equipped_weapon << "\n"; 
    file << player.equipped_armor << "\n";  

    // Безопасная запись инвентаря
    if (player.inventory.empty()) {
        file << "NONE_ITEMS";
    } else {
        for (size_t i = 0; i < player.inventory.size(); ++i) {
            file << player.inventory[i] << (i + 1 == player.inventory.size() ? "" : " ");
        }
    }
    file << "\n"; 

    // Безопасная запись сюжетных флагов
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

    // Считываем статы и слоты экипировки в строгом порядке записи
    file >> player.health >> player.max_health >> player.gold 
         >> player.base_armor >> player.base_damage 
         >> player.equipped_weapon >> player.equipped_armor;
    
    file >> std::ws;
 
    if (player.max_health > 200) {
        player.max_health = 100;
        std::cout << "[Система] Подозрительно высокий уровень максимального здоровья сброшен до 100!\n";
    }
    if (player.health > player.max_health) {
        player.health = player.max_health;
    }
 
    // Чтение инвентаря
    std::string inv_line;
    std::getline(file, inv_line);
    player.inventory.clear();
    if (!inv_line.empty() && inv_line != "NONE_ITEMS") {
        std::stringstream ss(inv_line); 
        std::string item;
        while (ss >> item) player.inventory.push_back(item);
    }

    // Чтение сюжетных флагов
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


int Player::get_damage() const {
    int weapon_bonus = 0;
    
    if (equipped_weapon == "Охотничий_нож")       weapon_bonus = 10;
    else if (equipped_weapon == "Стальной_меч")   weapon_bonus = 25;
    else if (equipped_weapon == "Тяжелая_секира") weapon_bonus = 45;
    else if (equipped_weapon == "Священный_молот") weapon_bonus = 65;
    else if (equipped_weapon == "Посох_Света")     weapon_bonus = 90; 
    else if (equipped_weapon == "Ржавый_меч")     weapon_bonus = 5;
    
    return base_damage + weapon_bonus;
}

int Player::get_armor() const {
    int armor_bonus = 0;
    
    if (equipped_armor == "Кожаный_доспех")       armor_bonus = 2;
    else if (equipped_armor == "Стальная_кираса") armor_bonus = 5;
    else if (equipped_armor == "Сверкающий_доспех") armor_bonus = 10;
    
    return base_armor + armor_bonus;
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
