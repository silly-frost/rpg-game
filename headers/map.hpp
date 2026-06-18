#ifndef MAP_HPP
#define MAP_HPP

#include<ctime>
#include<thread>
#include<chrono>

#define dialogue_time std::this_thread::sleep_for(std::chrono::seconds(2))
#define location static std::string

void map_time(); // время на карте (привязано к времени пользователя)
std::string go_to_location(std::string p_choose); // перемещение к выбранной локации
void map_description(std::string p_choose); // описание локации

#endif
