#ifndef MAP_CPP
#define MAP_CPP

#include"map.hpp"
#include<iostream>
#include<string>

std::string home = "Дом"; // (id = 0)
std::string desert = "Пустыня"; // (id = 1)
std::string unnamed_kingdom = "Безымянное королевство"; // (id = 2)
int void_counter;

void map_time(){
    std::time_t now = std::time(nullptr);

    std::tm local_time;
    localtime_r(&now, &local_time);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << std::endl;
}

int go_to_location(){
wrong_loc: //goto метка для того, чтобы начать заново функцию, при ошибке пользователя, а также небольшой фишкой
    int loc_id;
    std::string player_choose;
    
    if(player_choose.empty() || player_choose == home){
        std::cout << "Список доступных локаций: \n"
                << "1. Пустыня \n"
                << "2. Безымянное королевство \n\n"
                << "Напиши навание локации, в которую хочешь отправиться: ";
        std::getline(std::cin, player_choose);
    }
    else if(player_choose == desert){
        std::cout << "Список доступных локаций: \n"
                << "1. Дом \n"
                << "2. Безымянное королевство \n\n"
                << "Напиши навание локации, в которую хочешь отправиться: ";
        std::getline(std::cin, player_choose);
    }
    else if(player_choose == unnamed_kingdom){
        std::cout << "Список доступных локаций: \n"
                << "1. Дом \n"
                << "2. Пустыня \n\n"
                << "Напиши навание локации, в которую хочешь отправиться: ";
        std::getline(std::cin, player_choose);
    }

    if(void_counter > 2){
         #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif

        std::cout << "Вы оказались в пустоте! Вас окутывает страх, а тело перестаёт слушаться...\n"
                  << "Текущее время на локации: NULL";
        std::this_thread::sleep_for(std::chrono::seconds(4));
        // в будущем у этой локации будет свой npc
    }
    
    if(player_choose != home && player_choose != desert && player_choose != unnamed_kingdom){
        std::cout << "\nВыбрана неизвестная локация. Выберите заново, либо отправьтесь в указанный путь. \n"
                  << "Однако будьте осторожны, многие пропали без вести, пройдя по неизвестному маршруту. \n";
        void_counter++;
        goto wrong_loc;
    }

    if(player_choose == home) loc_id = 0;
    else if(player_choose == desert) loc_id = 1;
    else if(player_choose == unnamed_kingdom) loc_id = 2;
    else loc_id = -1;
    
    std::cout << "\nОтправляемся в " << player_choose << ". \nВремя в пути: 3 секунды \n";

    // Остановка на 3 секунды
    std::this_thread::sleep_for(std::chrono::seconds(3));

    #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif

    std::cout << "Вы прибыли! ";
    map_time();

    return loc_id;
}

#endif
