#ifndef MAP_CPP
#define MAP_CPP

#include"map.hpp"
#include<iostream>
#include<string>

static std::string home = "Дом"; // (id = 0)
static std::string desert = "Деревня в пустыне"; // (id = 1)
static std::string unnamed_kingdom = "Безымянное королевство"; // (id = 2)

void map_time(){
    std::time_t now = std::time(nullptr);

    std::tm local_time;
    localtime_r(&now, &local_time);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << "\n" << std::endl;
}




std::string go_to_location(){
wrong_loc: //goto метка для того, чтобы начать заново функцию, при ошибке пользователя
    std::string p_choose;

    if(p_choose.empty() || p_choose == home){
        std::cout << "Список доступных локаций: \n"
                << "1. " << desert << "\n"
                << "2. " << unnamed_kingdom << "\n\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);
    }
    else if(p_choose == desert){
        std::cout << "Список доступных локаций: \n"
                << "1. " << home << "\n"
                << "2. " << unnamed_kingdom << "\n\n"
                << "Напите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);
    }
    else if(p_choose == unnamed_kingdom){
        std::cout << "Список доступных локаций: \n"
                << "1. " << home << "\n"
                << "2. " << desert << "\n\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);
    }

    if(p_choose != home && p_choose != desert && p_choose != unnamed_kingdom){
        std::cout << "\nВыбрана неизвестная локация. Выберите заново и будьте осторожны. \n\n";
        goto wrong_loc;
    }
   
    std::cout << "\nОтправляемся в локацию " << p_choose << ". \nВремя в пути: 3 секунды \n";

    // Остановка на 3 секунды
    std::this_thread::sleep_for(std::chrono::seconds(3));

    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif

    std::cout << "Вы прибыли! ";
    map_time();

    return p_choose;
}

#endif
