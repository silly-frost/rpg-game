
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <cstdlib>

struct Monster {
    std::string name;
    int health;
    int damage;
    int gold_reward;
};

inline void wait_for_player() {
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::string pause;
    std::getline(std::cin, pause);
}

inline void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

bool start_battle(Monster enemy);

#endif // UTILS_HPP

