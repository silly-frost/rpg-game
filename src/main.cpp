#include"utils.hpp"
#include"../headers/player.hpp"    
#include"../headers/map.hpp"       
#include"../headers/npc_data.hpp" 
#include<iostream>
#include<string>

#ifdef _WIN32
    #include <windows.h>
#endif

void game_menu() {
  while (true) {
        if (!player.is_alive()) {
            respawn_player(); 
            continue;
        }

        const Location& current = world_map.at(current_location);

        std::cout << "\nЛокация: \"" << current.name << "\"\n";
        std::cout << "Что вы хотите сделать?\n";
        std::cout << "1. Отправиться в путь (Сменить локацию)\n";
        std::cout << "2. Посмотреть своё состояние (Статы/инвентарь)\n";
        std::cout << "3. Осмотреться вокруг (Описание локации)\n";
        std::cout << "4. Поговорить с кем-нибудь из местных (NPC/Торговля)\n";
        std::cout << "5. Выйти из игры (С сохранением)\n\n";
        std::cout << "Ваш выбор (1-5): ";
        
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cout << "\nПожалуйста, вводите только цифры!\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n'); 
            continue;
        }
        
        std::cin.ignore(1, '\n');
        switch (choice) {
            case 1:
                change_location();
                break;
            case 2:
                clear_screen();
                show_player_status();
                break;
            case 3:
                clear_screen();
                std::cout << "\n" << current.description << "\n";
                break;
            case 4:
                clear_screen();
                interact_with_npcs();
                break;
            case 5:
                save_game();
                std::cout << "\nСпасибо за игру! До встречи.\n";
                exit(0);
            default:
                std::cout << "\nНеверный пункт меню. Попробуйте еще раз.\n";
                break;            
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    std::setlocale(LC_ALL, ".UTF-8"); 
#endif
    clear_screen();

    if (!load_map_from_file("map.json")) {
        std::cout << "Критическая ошибка: невозможно запустить игру без карты.\n";
        return 1; 
    }
    if (!load_game()) {
        std::cout << "[Система] Файл сохранения не найден. Начата новая игра.\n";
    }

    if (player.has_flag("world_saved")) {
        world_map[locationID::cursed_castle].name = "Замок Малакара";
        world_map[locationID::cursed_castle].description = "Разрушенный замок, в котором раньше обитал Малакр...";
    }

    std::cout << "Добро пожаловать, " << player.name << "!\n";
    
    game_menu();
    
    return 0;
}
