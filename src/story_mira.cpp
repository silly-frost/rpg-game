#include "story_mira.hpp"
#include "player.hpp"
#include "map.hpp"
#include "utils.hpp"
#include "battle.hpp"
#include <iostream>
#include <string>


void talk_to_mira() {
    // Состояние 4: Мир уже полностью спасен и очищен
    if (player.has_flag("world_saved")) {
        if (player.has_flag("mira_alone")) {
            std::cout << "\n[Система] Лаборатория Миры пуста. На столе лежит записка...\n\n";
            std::cout << "\"Я всё же добралась живой до замка. Ядро полностью уничтожено. \"\n";
            std::cout << "\"Там же я обнаружила записи Малакара, похоже он не первый, кто проводил подобные эксперименты.\"\n";
            std::cout << "\"Я отправлюсь на другой материк, возможно там можно узнать подробнее об этой заразе.\"\n";
        } 
        else {
            std::cout << "\n[Мира]: Привет, " << player.name << "! Наша вакцина полностью исцелила эти земли. \n[Мира]: Наконец-то люди могут спать спокойно.\n";
        }
        wait_for_player();
        return;
    }

    // Состояние 3: Игрок убил Малакара и принес его прах
    if (player.has_flag("game_completed") && !player.has_flag("world_saved")) {
        bool has_ashes = false;
        size_t ashes_idx = 0;
        for (size_t i = 0; i < player.inventory.size(); ++i) {
            if (player.inventory[i] == "Прах_Малакара") { has_ashes = true; ashes_idx = i; break; }
        }

        if (has_ashes) {
            std::cout << "\n[Мира]: Боги... Ты вернулся живым из Проклятого замка?! \nИ что это у тебя в руках...\n";
            std::cout << "1. [Квест] Отдать Прах Малакара Мире\n2. Назад\nВыбор: ";
            int q; std::cin >> q; std::cin.ignore(1, '\n');
            if (q == 1) {
                clear_screen();
                player.inventory.erase(player.inventory.begin() + ashes_idx);
                player.gold += 500; 
                world_map[locationID::cursed_castle].name = "Замок Малакара";
                world_map[locationID::cursed_castle].description = "Разрушенный замок, в котором раньше обитал Малакар...";

                std::cout << "[Мира]: Прах Малакара... В нём до сих пор теплится изначальная магия!\n";                    
                std::cout << " Смешав его с реактивами, Мира создаёт искрящуюся лазурную сыворотку.\n\n";
                std::cout << "[Мира]: Эту сыворотку нельзя просто вылить в воду. \n[Мира]: Её нужно распылить прямо в сердце Проклятого замка, там, где стояло стальное ядро. \n[Мира]: Иначе, возможно, вирус вернётся.\n";
                std::cout << "[Мира]: Если я не ошибаюсь, то оставшиеся наросты действуют по своей воле. Идти туда в одиночку будет опасно. \n[Мира]: Ты пойдёшь со мной, " << player.name << "?\n\n";
                
                int choice_end = 0;
                while (choice_end != 1 && choice_end != 2) {
                    std::cout << "1. Да, Мира. Мы начали это вместе, вместе и закончим. (Отправиться в Замок вместе)\n";
                    std::cout << "2. Нет, я выполнил твою просьбу и убил мага. Дальше действуй сама. (Мира уйдет одна)\n";
                    std::cout << "Выбор: ";
                    if (!(std::cin >> choice_end)) {
                        std::cin.clear();
                        std::cin.ignore(10000, '\n');
                        continue;
                    }
                    std::cin.ignore(1, '\n');
                }
                
                clear_screen();
                player.story_flags.push_back("world_saved");
                
                if (choice_end == 1) {
                    player.story_flags.push_back("mira_companion");
                    current_location = locationID::cursed_castle;
                    
                    std::cout << "Вы вместе с Мирой держите путь через заснеженные земли и штормовое море...\nВ пути";
                    std::cout << std::flush;
                    for(int i=0; i<12; ++i) { std::this_thread::sleep_for(std::chrono::seconds(1)); std::cout << " ." << std::flush; }
                    
                    clear_screen();
                    std::cout << "\n====================================================================\n";
                    std::cout << "                      ФИНАЛ: ОЧИЩЕНИЕ ЭПИЦЕНТРА \n";
                    std::cout << "====================================================================\n";
                    std::cout << "Вы стоите посреди тронного зала Проклятого замка. Мира разбивает колбу\n";
                    std::cout << "с лазурной сывороткой прямо над ржавыми обломками ядра Малакара.\n";
                    std::cout << "Яркая неоновая волна прокатывается по залу, сжигая живой металл.\n";
                    std::cout << "Мира устало улыбается вам:\n";
                    std::cout << "[Мира]: Мы сделали это. Мир спасён!\n";
                    std::cout << "====================================================================\n";
                } else {
                    player.story_flags.push_back("mira_alone");
                    std::cout << "[Мира]: Что ж... Ты и так сделал достаточно. Прощай, путник. Надеюсь, моё лекарство сработает.\n";
                    std::cout << "\nМира забирает колбу, собирает вещи и в одиночку уходит на северный корабль...\n";
                    std::cout << "Через несколько дней до деревни доходят слухи: Проклятый замок очищен!\n";
                    std::cout << "Багровый туман исчез, но саму Миру в этих землях больше никто не видел.\n";
                }
                wait_for_player();
                return;
            }
        }
    }

    // Состояние 2: Обычный квест на сбор руды (если Малакар еще жив)
    if (player.has_flag("mira_quest_started") && !player.has_flag("mira_quest_finished")) {
        std::cout << "\n[Мира]: Ты вернулся? Удалось добыть образец зараженной стали с Железного острова?\n";
        bool has_metal = false; size_t metal_idx = 0;
        for (size_t i = 0; i < player.inventory.size(); ++i) {
            if (player.inventory[i] == "Образец_металла") { has_metal = true; metal_idx = i; break; }
        }
        
        if (has_metal) {
            std::cout << "1. [Квест] Отдать Образец металла Мире\n2. Еще нет\nВыбор: ";
            int q; std::cin >> q; std::cin.ignore(1, '\n');
            clear_screen();
            if (q == 1) {
                player.inventory.erase(player.inventory.begin() + metal_idx);
                player.story_flags.push_back("mira_quest_finished");
                player.gold += 150;
                std::cout << "\n[Мира]: Спасибо тебе! Вот твоя награда — 150 золотых.\n[Мира]: Если Малакар уцелел, он до сих пор заперт в Проклятом замке. Если пойдешь туда — готовься к худшему.\n";
                wait_for_player(); 
                return;
            }
        } else {
            std::cout << "\n[Мира]: Без образца я не смогу понять структуру заражения. \n[Мира]: Кирку можно взять у кузнеца Грома, а сам металл ищи на скалах Железного острова.\n";
        }
        wait_for_player();
        return;
    }

    // Переходное состояние: руда сдана, босс еще жив
    if (player.has_flag("mira_quest_finished") && !player.has_flag("game_completed")) {
        std::cout << "\n[Мира]: Твои образцы помогли мне. Но истинный источник — в Проклятом замке. \n[Мира]: Малакара нужно остановить, пока вирус не поглотил весь материк.\n";
        wait_for_player();
        return;
    }

    // Состояние 1: Самый первый диалог
    std::cout << "\n[Мира]: Приветствую тебя. Я Мира, алхимик. Я посвятила жизнь борьбе с этой проклятой металлической заразой...\n";
    std::cout << "[Мира]: Мне нужны образцы этого металла с Железного острова для создания противоядия. Поможешь мне?\n";
    std::cout << "1. Да, я добуду образец металла.\n2. Это слишком опасно.\nВыбор: ";
    int q; std::cin >> q; std::cin.ignore(1, '\n');
    clear_screen();
    if (q == 1) {
        player.story_flags.push_back("mira_quest_started");
        std::cout << "\n[Мира]: Спасибо! Зайди к оружейнику Грому, у него была старая кирка. Отколи кусок породы на острове и вези мне.\n";
    } else {
        std::cout << "\n[Мира]: Что ж, я не виню тебя.\n";
    }
    wait_for_player();
}

