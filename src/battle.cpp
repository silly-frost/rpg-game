#include "battle.hpp"
#include "player.hpp"
#include "map.hpp"
#include <iostream>
#include <vector>
#include <string>

// Функция пошагового боя
bool start_battle(Monster enemy) {
    clear_screen();
    std::cout << "\n========================================\n";
    std::cout << " [!] НА ВАС НАПАЛ: " << enemy.name << "!\n";
    std::cout << "========================================\n";

    while (player.is_alive() && enemy.health > 0) {
        std::cout << "\nВаше HP: " << player.health << "/" << player.max_health << " | Защита: " << player.get_armor() << "\n";        
        std::cout << "HP врага " << enemy.name << " : " << enemy.health << "\n\n";
        std::cout << "Что вы будете делать?\n\n";
        std::cout << "1. Атаковать оружием [" << (player.equipped_weapon == "NONE" ? "Кулаки" : player.equipped_weapon) << "]\n";        
        std::cout << "2. Использовать лечащий предмет из сумки\n";
        std::cout << "3. Попытаться сбежать (Шанс 50%)\n\n"; 
        std::cout << "Выбор: ";
        int action; std::cin >> action; std::cin.ignore(1, '\n');

        clear_screen();
        bool turn_passed = false;

        
        if (action == 1) {
            int final_damage = player.get_damage(); 

            // Защита от ваншота для Малакара
            if (enemy.name.find("Малакар") != std::string::npos) {
                if (player.get_damage() > 90) {
                    final_damage = 0; 
                    std::cout << "\n[Малакар]: Ха-ха-ха! Ты думал, что твоя фальшивая сила пробьет живую сталь?!\n";
                    std::cout << "[!] Магический щит Малакара заблокировал энергию вашего читерского удара!\n";
                }
                if (final_damage > 50) final_damage = 50;
            }

            enemy.health -= final_damage;
            std::cout << "\n Вы бьете врага оружием [" << (player.equipped_weapon == "NONE" ? "Кулаки" : player.equipped_weapon) << "] и наносите " << final_damage << " урона!\n";
            
            if (enemy.health <= 0) break;
            turn_passed = true; 
        } 
        
        else if (action == 2) {
            // Лечение в бою 
            // Ищем все лечащие предметы в инвентаре (Используем точное имя с большой буквы З)
            std::vector<size_t> potion_indices;
            for (size_t i = 0; i < player.inventory.size(); ++i) {
                std::string item = player.inventory[i];
                if (item == "Таинственное_Зелье" || item == "Пиво" || item == "Медовуха" || item == "Kрепкий_Эль" || item == "Вино") {
                    potion_indices.push_back(i);
                }
            }

            if (potion_indices.empty()) {
                std::cout << "\n[Система] В вашей сумке нет лечащих предметов или выпивки!\n";
                continue; // Возвращаем игрока к выбору действий, ход не теряется
            }

            // Выводим список доступных зелий
            std::cout << "\n--- Ваши лечащие предметы ---\n";
            for (size_t i = 0; i < potion_indices.size(); ++i) {
                std::string item_name = player.inventory[potion_indices[i]];
                for (char& c : item_name) if (c == '_') c = ' '; 
                std::cout << i + 1 << ". [" << item_name << "]\n";
            }
            std::cout << "0. Назад (Отмена)\nВыбор предмета: ";
            int pot_choice; std::cin >> pot_choice; std::cin.ignore(1, '\n');

            clear_screen();
            if (pot_choice == 0 || pot_choice < 1 || pot_choice > (int)potion_indices.size()) {
                continue;            
            }
            size_t actual_inventory_index = potion_indices[pot_choice - 1];
            std::string chosen_potion = player.inventory[actual_inventory_index];

            if (player.health >= player.max_health) {
                std::cout << "\n[Система] У вас и так полное здоровье! Лечение отменено.\n";
                continue;
            }

            if (chosen_potion == "Таинственное_Зелье") {
                player.health += 70;
                if(player.base_damage == 20) std::cout << "\n[+] В спешке боя вы выпиваете Таинственное зелье, восстанавливая 70 HP!\n";
                else {
                    player.base_damage += 10;
                    std::cout << "\n[+] В спешке боя вы выпиваете Таинственное зелье, восстанавливаете 70 HP и увеличиваете свой урон на 10!\n";
                }
            }
            else if (chosen_potion == "Пиво") {
                player.health += 20;
                std::cout << "\n[+] Торопясь, вы выпиваете Пиво и восстанавливаете 20 HP!\n";
            }
            else if(chosen_potion == "Медовуха"){
                player.health += 40;
                std::cout << "\n[+] Вы с удовольствием выпиваете Медовуху, несмотря на битву, и восстанавливаете 40 HP!\n";
            }
            else if(chosen_potion == "Крепкий_Эль"){
                player.health += 50;
                std::cout << "\n[+] Вы залпом выпиваете Крепкий Эль, прожигая горло, и восстанавливаете 50 HP!\n";
            }
            else if(chosen_potion == "Вино"){
                player.health += 60;
                std::cout << "\n[+] Вы удивляете соперника своей манерностью и восстанавливаете себе 60 HP!\n";
            }
            // Предохранитель лимита здоровья
            if (player.max_health > 200) player.max_health = 200;
            if (player.health > player.max_health) player.health = player.max_health;

            // Удаляем предмет из инвентаря
            player.inventory.erase(player.inventory.begin() + actual_inventory_index);
            
            // Лечение завершено, игрок тратит ход, монстр будет атаковать
            turn_passed = true;
        }

        else if (action == 3) {
            // Ход игрока, попытка побега
            if(enemy.name.find("Малакар") != std::string::npos) {
                std::cout << "\n[Малакар]: Ты принял бой, бежать некуда!\n";
                std::cout << "[Провал] Железные отростки вонзаются в вас.";
                respawn_player();
                return false;
            }
            if (rand() % 2 == 0) {
                std::cout << "\n[Успех] Вам удалось запутать врага и трусливо сбежать!\n";
                wait_for_player();
                return false; 
            } 
            else {
                std::cout << "\n[Провал] Вы споткнулись! Сбежать не удалось!\n";
                turn_passed = true;
            }
        }

        // Ход монстра (срабатывает, если игрок атаковал, выпил зелье или провалил побег)
        if (turn_passed) {
            int enemy_strike = enemy.damage - player.get_armor(); 
            int min_damage = 15;
            
            if (enemy.name.find("Главарь") != std::string::npos || enemy.name.find("Малакар") != std::string::npos) {
                min_damage = enemy.damage * 70 / 100; 
            }
            
            if (min_damage < 0) min_damage = 0;
            if (enemy_strike < min_damage) enemy_strike = min_damage;
            
            player.health -= enemy_strike;
            std::cout << " [" << enemy.name << "] пользуется моментом и наносит вам " << enemy_strike << " урона!\n";
        }
    }

    if (!player.is_alive()) {
        std::cout << "\n[!] Вы пали в бою против " << enemy.name << "...\n";
        wait_for_player();
        
        respawn_player();        
        return false;
    }

    player.gold += enemy.gold_reward;
    std::cout << "\n========================================\n";
    std::cout << " [ПОБЕДА] Вы одолели " << enemy.name << "!\n";
    std::cout << " Вы нашли: " << enemy.gold_reward << " золотых монет.\n";
    std::cout << "========================================\n";
    
    // Пересохраняем игру сразу после победы, чтобы зафиксировать новый инвентарь и золото
    extern void save_game();
    save_game(); 
    
    wait_for_player();
    return true; 
}
