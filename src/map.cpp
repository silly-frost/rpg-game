#ifndef MAP_CPP
#define MAP_CPP

#include"map.hpp"
#include"utils.hpp"
#include<iostream>
#include<string>

// Лесная зона
location home = "Дом"; 
location flower_valley = "Долина цветов";
location old_forest = "Старый лес";
location broken_village = "Разрушенная деревня";

// Пустынная зона
location desert = "Пустыня";
location unnamed_kingdom = "Безымянное королевство";
location oasis = "Оазис";
location desert_village = "Деревня в пустыне";

// Снежная зона
location snowlands = "Заснеженные земли";
location ice_caves = "Ледяные пещеры";

// Проклятые земли
location iron_island = "Железный остров";
location cursed_castle = "Проклятый замок";

// Наказание :)
location punishment = ":)";

void map_time(){
    std::time_t now = std::time(nullptr);

    std::tm local_time;
    localtime_r(&now, &local_time);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << "\n" << std::endl;
}


std::string go_to_location(std::string p_choose){
    clear_screen();

    std::cout << "Вам будет предоставлен список локаций, в которые можно отправиться. \n";
    dialogue_time;
    std::cout << "В некоторые локации нельзя попасть просто так. Вы должны методом проб и ошибок самостоятельно искать путь. \n";
    dialogue_time;
    std::cout << "Однако, если вы попробуете обмануть систему, то понесёте наказание. Вы предупреждены. \n";

    std::this_thread::sleep_for(std::chrono::seconds(4));

    clear_screen();

    if(p_choose.empty() || p_choose == home){
        std::cout << "Список доступных локаций: \n"
                << "1. " << desert << "\n"
                << "2. " << flower_valley << "\n" 
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != desert && p_choose != flower_valley) p_choose = punishment;
    }
    else if(p_choose == desert){
         std::cout << "Список доступных локаций: \n"
                << "1. " << home << "\n"
                << "2. " << unnamed_kingdom << "\n"
                << "3. " << oasis << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != home && p_choose != unnamed_kingdom && p_choose != oasis) p_choose = punishment;
    }
    else if(p_choose == oasis){
        std::cout << "Список доступных локаций: \n"
            << "1. " << desert << "\n"
            << "2. " << desert_village << "\n" 
            << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != desert && p_choose != desert_village) p_choose = punishment;
    }
    else if(p_choose == flower_valley){
         std::cout << "Список доступных локаций: \n"
                << "1. " << home << "\n"
                << "2. " << old_forest << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != home && p_choose != old_forest) p_choose = punishment;
    }
    else if(p_choose == old_forest){
        std::cout << "Список доступных локаций: \n"
                << "1. " << flower_valley << "\n"
                << "2. " << broken_village << "\n" 
                << "3. " << snowlands << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != flower_valley && p_choose != broken_village && p_choose != snowlands) p_choose = punishment;
    }
    else if(p_choose == snowlands){
         std::cout << "Список доступных локаций: \n"
                << "1. " << old_forest << "\n"
                << "2. " << ice_caves << "\n"
                << "3. " << iron_island << "\n" 
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != old_forest && p_choose != ice_caves && p_choose != iron_island) p_choose = punishment;
    }
    else if(p_choose == iron_island){
         std::cout << "Список доступных локаций: \n"
                << "1. " << snowlands << "\n"
                << "2. " << cursed_castle << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != snowlands && p_choose != cursed_castle) p_choose = punishment;
    }
    else if(p_choose == cursed_castle){
        std::cout << "Список доступных локаций: \n"
                << "1. " << iron_island << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != iron_island) p_choose = punishment;
    }
    else if(p_choose == desert_village){
        std::cout << "Список доступных локаций: \n"
                << "1. " << oasis << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != oasis) p_choose = punishment;
    }
    else if(p_choose == unnamed_kingdom){
        std::cout << "Список доступных локаций: \n"
                << "1. " << desert << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);

        if(p_choose != desert) p_choose = punishment;
    }
    else if(p_choose == ice_caves){
        std::cout << "Список доступных локаций: \n"
                << "1. " << snowlands << "\n"
                << "Напишите навание локации, в которую хотите отправиться: ";
        std::getline(std::cin, p_choose);
        
        if(p_choose != snowlands) p_choose = punishment;
    }
    else {
        std::cout << "Вы находитесь в неизвестной локации. Перемещу вас домой через 4 секунды.\n";
        p_choose = home;
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }

    clear_screen();

    if(p_choose == punishment){
        std::cout << "Добро пожаловать в пустоту! Вы здесь не просто так. \n";
        dialogue_time; 
        std::cout << "В самом начале я вас предупреждал, что нарушение правил ведёт к наказанию.\n";
        dialogue_time;
        std::cout << "Вашим наказанием будет путь домой. \n";
        dialogue_time;
        std::cout << "Но идти вам придётся немного дольше, чем обычно...\n";
        dialogue_time;

        clear_screen();
        
        std::cout << "Время в пути: 5 минут :)\n";

        std::this_thread::sleep_for(std::chrono::seconds(300));
        std::cout << "Вы прибыли, впредь больше не нарушайте правила. \n";
        return home;
    }

    std::cout << "Отправляемся в локацию \"" << p_choose << "\"!\n";

    int time_in_travel = 0;
    if (p_choose == "flower_valley" || p_choose == "oasis" || 
        p_choose == "broken_village" || p_choose == "cursed_castle") {
        time_in_travel = 3;
    } else if (p_choose == "desert" || p_choose == "desert_village" || 
               p_choose == "old_forest" || p_choose == "iron_island" || 
               p_choose == "snowlands") {
        time_in_travel = 5;
    } else if (p_choose == "unnamed_kingdom" || p_choose == "ice_caves" || 
               p_choose == "home") {
        time_in_travel = 4;
    }

    std::cout << "Время в пути: " << travelTime << " секунд \n";
    std::this_thread::sleep_for(std::chrono::seconds(travelTime));
    std::cout << "Вы прибыли! \n";
}

    clear_screen();
    
    std::cout << "Вы прибыли! ";
    map_time();

    return p_choose;
}

#endif
