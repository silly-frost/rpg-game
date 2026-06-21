#ifndef NPC_DATA_HPP
#define NPC_DATA_HPP

#include "player.hpp"
#include "map.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// Функция пошагового боя
inline bool start_battle(Monster enemy) {
    clear_screen();
    std::cout << "\n========================================\n";
    std::cout << " [!] НА ВАС НАПАЛ: " << enemy.name << "!\n";
    std::cout << "========================================\n";

    while (player.is_alive() && enemy.health > 0) {
        std::cout << "\nВаше HP: " << player.health << "/" << player.max_health << " | Защита: " << player.armor << "\n";
        std::cout << "HP врага " << enemy.name << " : " << enemy.health << "\n\n";
        std::cout << "Что вы будете делать?\n\n";
        std::cout << "1. Атаковать оружием [" << player.weapon_name << "]\n";
        std::cout << "2. Использовать лечащий предмет из сумки\n";
        std::cout << "3. Попытаться сбежать (Шанс 50%)\n\n"; 
        std::cout << "Выбор: ";
        int action; std::cin >> action; std::cin.ignore(1, '\n');

        clear_screen();
        bool turn_passed = false;

        if (action == 1) {
            int final_damage = player.damage;

            // Защита от ваншота для Малакара
            if (enemy.name.find("Малакар") != std::string::npos) {
                if (player.damage > 90) {
                    final_damage = 0; 
                    std::cout << "\n[Малакар]: Ха-ха-ха! Ты думал, что твоя фальшивая сила пробьет живую сталь?!\n";
                    std::cout << "[!] Магический щит Малакара заблокировал энергию вашего читерского удара!\n";
                }
                if (final_damage > 50) final_damage = 50;
            }

            enemy.health -= final_damage;
            std::cout << "\n Вы бьете врага оружием [" << player.weapon_name << "] и наносите " << final_damage << " урона!\n";
            
            if (enemy.health <= 0) break;
            turn_passed = true; 
        } 
        else if (action == 2) {
            // Лечение в бою 
            // 1. Ищем все лечащие предметы в инвентаре
            std::vector<size_t> potion_indices;
            for (size_t i = 0; i < player.inventory.size(); ++i) {
                std::string item = player.inventory[i];
                if (item == "Настойка_трав" || item == "Пиво" || item == "Медовуха" || item == "Крепкий_Эль" || item == "Вино") {
                    potion_indices.push_back(i);
                }
            }

            if (potion_indices.empty()) {
                std::cout << "\n[Система] В вашей сумке нет лечащих предметов или выпивки!\n";
                continue; // Возвращаем игрока к выбору действий, ход не теряется
            }

            // 2. Выводим список доступных зелий
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

            // 3. Применяем эффект лечения
            if (chosen_potion == "Таинственное зелье") {
                player.health += 60;
                player.damage += 10;
                std::cout << "\n[+] В спешке боя вы выпиваете Настойку трав и восстанавливаете 50 HP!\n";
            } 
            else if (chosen_potion == "Пиво") {
                player.health += 20;
                std::cout << "\n[+] Торопясь, вы выпиваете Пиво и восстанавливаете 20 HP!\n";
            }
            else if(chosen_potion == "Медовуха"){
                player.health += 40;
                std::cout << "\n[+] Вы с удоволствием выпиваете Медовуху, несмотря на битву, и восстанавливаете 40 HP!\n";
            }
            else if(chosen_potion == "Вино"){
                player.health += 60;
                std::cout << "\n[+] Вы удивляете соперника своей манерность и восстанваливаете себе 60 HP!\n";
            }
            // Предохранитель лимита здоровья
            if (player.max_health > 200) player.max_health = 200;
            if (player.health > player.max_health) player.health = player.max_health;

            // Удаляем предмет из инвентаря
            player.inventory.erase(player.inventory.begin() + actual_inventory_index);
            
            // Лечение завершено, игрок тратит ход, монстр будет атаковать!
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
            int enemy_strike = enemy.damage - player.armor;
            int min_damage = 15;
            
            if (enemy.name.find("Главарь") != std::string::npos || enemy.name.find("Малакар") != std::string::npos) {
                min_damage = enemy.damage * 70 / 100; 
            }
            
            if (enemy_strike < min_damage) enemy_strike = min_damage;
            
            player.health -= enemy_strike;
            std::cout << " [" << enemy.name << "] пользуется моментом и наносит вам " << enemy_strike << " урона!\n";
        }
    }

    if (!player.is_alive()) {
        std::cout << "\n[!] Вы пали в бою против " << enemy.name << "...\n";
        wait_for_player();
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

// Функция-менеджер диалогов для конкретной локации
inline void interact_with_npcs() {
    std::vector<std::string> local_npcs;    
    // Сбор доступных NPC на текущей локации
    if (current_location == locationID::home) local_npcs.push_back("Гид Олден");
    else if (current_location == locationID::dark_forest) {
        local_npcs.push_back("Шаман Раду");
        local_npcs.push_back("Дух Лешего");
        local_npcs.push_back("Старый Охотник Грей (Заработок)");
    }
    else if (current_location == locationID::ice_caves) local_npcs.push_back("Странник Илай");
    else if (current_location == locationID::snow_village) {
        if (player.has_flag("talked_to_stranger")) local_npcs.push_back("Однорукий старик Бернард");
        local_npcs.push_back("Алхимик Мира");
    }
    else if (current_location == locationID::unnamed_kingdom) {
        local_npcs.push_back("Король Эдриан");
        local_npcs.push_back("Стражник Томас");
        if (player.has_flag("talked_to_guard")) local_npcs.push_back("Оружейник Гром");
    }
    else if (current_location == locationID::desert_village) {
        local_npcs.push_back("Изгнанник Карим");
        if (player.has_flag("talked_to_citizen")) local_npcs.push_back("Разбойник Джафар");
    }
    else if (current_location == locationID::tavern) {
        local_npcs.push_back("Трактирщик Гарри (Бар)");
        local_npcs.push_back("Пьяная компания в углу");
    }
    else if (current_location == locationID::seaport) {
        local_npcs.push_back("Бригадир портовых доков (Заработок)");
    }
    else if (current_location == locationID::cursed_castle) {
        local_npcs.push_back("Безумный маг Малакар (БОСС)");
    }
    else if (current_location == locationID::iron_island) {
        local_npcs.push_back("Кристалл живого металла (Добыча)");
    }


    if (local_npcs.empty()) {
        std::cout << "\nВ этой локации сейчас никого нет.\n";
        wait_for_player();
        return;
    }

    std::cout << "\nПеред вами люди и существа. С кем вы хотите заговорить?\n";
    for (size_t i = 0; i < local_npcs.size(); ++i) std::cout << i + 1 << ". " << local_npcs[i] << "\n";
    std::cout << "0. Назад\nВаш выбор: ";
    int npc_choice; std::cin >> npc_choice; std::cin.ignore(1, '\n');

    if (npc_choice <= 0 || npc_choice > (int)local_npcs.size()) return;
    std::string chosen_npc = local_npcs[npc_choice - 1];

    clear_screen();

    if (chosen_npc == "Гид Олден") {
        if (player.has_flag("world_saved")) {
            std::cout << "\n[Олден]: Слава спасителю! Свитки не врут — ты совершил величайший подвиг в истории. \n[Олден]: Металлический вирус отступает, туманы рассеиваются. Наш мир спасен благодаря тебе, " << player.name << "!\n";
            wait_for_player();
            return;
        }

        std::cout << "\n[Олден]: Приветствую, путник! Мои свитки всегда открыты для ищущих знания. О чём побеседуем?\n\n";
        std::cout << "1. Расскажи про обычную жизнь людей в этих землях.\n";
        std::cout << "2. Откуда и куда ведут дороги на карте мира?\n";
        std::cout << "3. Что такое металлический вирус?\n";
        std::cout << "4. Закончить разговор\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        clear_screen();

        if (q == 1) {
            std::cout << "\n[Олден]: Обычные крестьяне пытаются выжить, как могут. \n[Олден]: В Цветочной поляне мирно, там разводят пчел и собирают мед. \n[Олден]: В Безымянном королевстве процветает торговля, но налоги душат бедняков. \n[Олден]: В порту на севере промышляют рыболовством, хотя шторма там жуткие.\n";
        } else if (q == 2) {
            std::cout << "\n[Олден]: Из Дома ты можешь дойти до бескрайней Пустыни или прекрасной Цветочной поляны. \n[Олден]: Из Пустыни можно прийти в Безымянное королевство или отыскать Оазис. \n[Олден]: Из Цветочной поляны можно отправиться в Лес, из него ты можешь дойти до Разрушенной деревни или отправиться в Снежные земли. \n[Олден]: Будь осторожен!\n";
        } else if (q == 3) {
            std::cout << "\n[Олден]: Мне неизвестны все детали. Но поговаривают, что в Снежной деревне проживают знающие люди. \n[Олден]: Многих пугает металлическая зараза. Не все будут говорить стобой об этом.\n";
        }
        wait_for_player();
    }   
    else if (chosen_npc == "Трактирщик Гарри (Бар)") {
        if (player.has_flag("world_saved")) {
            std::cout << "\n[Гарри]: А вот и наш герой! " << player.name << ", за счет заведения! Сегодня вся таверна пьет за твое здоровье. \n[Гарри]: Ты уничтожил заразу на севере, парень, мы свободны!\n";
            player.inventory.push_back("Королевское_вино");
            std::cout << "[+] Трактирщик Гарри бесплатно налил вам Королевское вино в знак благодарности!\n";
            wait_for_player();
            return;
        }
        std::cout << "\n[Гарри]: Привет! Устал с дороги? У меня лучший погреб во всем королевстве! Выбирай:\n\n";
        std::cout << "1. Дешевое пиво (15 золотых) -> Обычное пойло\n";
        std::cout << "2. Медовуха из Цветочной долины (30 золотых) -> Приятный сладкий вкус\n";
        std::cout << "3. Крепкий Эль гномов (50 золотых) -> Обжигает горло!\n";
        std::cout << "4. Королевское вино (70 золотых) -> Напиток для знати\n";
        std::cout << "5. Расскажи городские сплетни.\n6. Уйти\n\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        
        int cost = 0; std::string item_name = "";
        if (q == 1) { cost = 15; item_name = "Пиво"; }
        else if (q == 2) { cost = 30; item_name = "Медовуха"; }
        else if (q == 3) { cost = 50; item_name = "Крепкий_Эль"; }
        else if (q == 4) { cost = 70; item_name = "Вино"; }
        else if (q == 5) {
            clear_screen();
            std::cout << "\n[Гарри]: Говорят, зреет бунт против Короля. Цены на еду растут, а стражники отбирают последнее золото. \n[Гарри]: Будь аккуратнее, среди песков полно злых людей.\n";
            wait_for_player();
            return;
        } 
        else { 
            return;         
        }
        clear_screen();
        if (player.gold >= cost) {
            player.gold -= cost;
            player.inventory.push_back(item_name);
            std::cout << "[Гарри]: Отличный выбор! Твой [" << item_name << "] отправлен в инвентарь.\n";
        } 
        else {
            std::cout << "[Гарри]: Золота маловато для такой выпивки, приятель!\n";
        }
        
        wait_for_player();
    }
    else if (chosen_npc == "Оружейник Гром") {
         if (player.has_flag("world_saved")) {
            std::cout << "\n[Гром]: Ха! Мое оружие побывало в руках спасителя мира! \n[Гром]: Для меня это высшая честь, путник. Металл больше не пожирает людей, и в этом твоя заслуга.\n";
            wait_for_player();
            return;
        }
        std::cout << "\n[Гром]: Заходи, путник! Мой молот не знает отдыха. Тебе нужна хорошая сталь или защита? Смотри, что есть:\n\n";
        std::cout << "--- ОРУЖИЕ ---\n";
        std::cout << "1. Охотничий нож (Урон +10) (Цена: 25 золотых)\n";
        std::cout << "2. Стальной длинный меч (Урон +25) (Цена: 60 золотых)\n";
        std::cout << "3. Тяжелая секира (Урон +45) (Цена: 150 золотых)\n";
        std::cout << "4. Священный молот (Урон +65) (Цена: 300 золотых\n\n";
        std::cout << "--- БРОНЯ ---\n";
        std::cout << "5. Кожаный доспех (Защита +2) | Цена: 50 золотых\n";
        std::cout << "6. Стальная кираса (Защита +5) | Цена: 120 золотых\n";
        std::cout << "7. Сверкающий доспех (защита +10) | Цена: 200 золотых\n";
        std::cout << "--- ДРУГОЕ ---\n";
        std::cout << "8. Продать что-нибудь кузнецу\n";
        if (!player.has_flag("got_pickaxe")) {
            std::cout << "9. [Диалог] А что это за ржавая кирка в углу комнаты?\n";
        }
        std::cout << "0. Уйти\n\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');

        clear_screen();
        if (q == 1 && player.gold >= 25) { player.gold -= 25; player.damage = 25; player.weapon_name = "Охотничий_нож"; player.inventory.push_back("Охотничий_нож"); std::cout << "Вы вооружились Охотничьим ножом!\n"; }
        else if (q == 2 && player.gold >= 60) { player.gold -= 60; player.damage = 40; player.weapon_name = "Стальной_меч"; player.inventory.push_back("Стальной_меч"); std::cout << "Вы вооружились Стальным мечом!\n"; }
        else if (q == 3 && player.gold >= 110) { player.gold -= 110; player.damage = 60; player.weapon_name = "Тяжелая_секира"; player.inventory.push_back("Тяжелая_секира"); std::cout << "Вы вооружились Тяжелой секирой!\n"; }
        else if (q == 4 && player.gold >= 300) { player.gold -= 300; player.damage = 80; player.weapon_name = "Священный_молот"; player.inventory.push_back("Священный_молот"); std::cout << "Вы получили силу Святого молота!"; }
        else if (q == 5 && player.gold >= 30) { player.gold -= 30; player.armor = 2; player.inventory.push_back("Кожаный_доспех"); std::cout << "Вы надели Кожаный доспех!\n"; }
        else if (q == 6 && player.gold >= 60) { player.gold -= 60; player.armor = 5; player.inventory.push_back("Стальная_кираса"); std::cout << "Вы надели Стальную кирасу!\n"; }
        else if (q == 7 && player.gold >= 200) { player.gold -= 200; player.armor = 5; player.inventory.push_back("Сверкающий_доспех"); std::cout << "Вы надели Сверкающий доспех!\n"; }
        else if (q == 8) {
            if (!player.inventory.empty()) {
                std::string item = player.inventory.back();
                
                // Запрещаем продавать только Кирку и Образец металла
                if (item == "Кирка" || item == "Образец_металла") {
                    std::cout << "[Гром]: Хех, парень, зачем мне старая кирка или этот кусок ржавой породы? \nТакое барахло мне в кузне без надобности. Оставь себе.\n";
                } else {
                    // Обычные предметы продавать можно
                    player.inventory.pop_back();
                    player.gold += 15;
                    std::cout << "[Гром]: Отличный хлам! Забираю [" << item << "], вот тебе 15 золотых монеток.\n";
                }
            } else {
                std::cout << "[Гром]: Твои карманы пусты, мне нечего у тебя покупать.\n";
            }
        }
        else if (q == 9 && !player.has_flag("got_pickaxe")) {
            player.story_flags.push_back("got_pickaxe");
            player.inventory.push_back("Кирка");
            std::cout << "[Гром]: А, эта старая рухлядь? Да забирай просто так, только место занимает. Я ей когда-то породу в шахтах долбил, пока рудники не забросили.\n";
            std::cout << "[+] В ваш инвентарь добавлена Кирка.\n";
        }
        else return;
        wait_for_player();
    }
    else if (chosen_npc == "Старый Охотник Грей (Заработок)") {
        std::cout << "\n[Грей]: Здорово, малый. Волки в Тёмном лесу совсем обнаглели.\n";
        std::cout << "1. [Квест] Помочь Грею зачистить логово Лютоволка\n2. Отказаться\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        if (q == 1) {
            Monster wolf = {"Зараженный Лютоволк", 100, 30, 30}; // ХП, Урон, Награда
            start_battle(wolf);
        }
    }
    
    else if (chosen_npc == "Бригадир портовых доков (Заработок)") {

        if (player.has_flag("world_saved")) {
            std::cout << "\n[Бригадир]: А, это ты, спаситель! Глазам своим не верю — море наконец-то очистилось!\n";
            std::cout << "[Бригадир]: Парни сегодня привезли полный трюм чистой, нормальной трески. Никакой стальной чешуи и гвоздей вместо зубов! \n[Бригадир]: Ты вернул жизнь нашему порту, путник. Спасибо тебе от всех рыбаков материка!\n";
            std::cout << "[Бригадир]: Хочешь подзаработать на разгрузке нормального улова?\n";
            std::cout << "1. [Работа] Разгрузить свежую рыбу (Займет 4 секунды | +15 золотых)\n";
            std::cout << "2. Назад\nВыбор: ";
            int q; std::cin >> q; std::cin.ignore(1, '\n');
            clear_screen();
            
            if (q == 1) {
                std::cout << "Вы весело разгружаете ящики со свежей рыбой"; std::cout << std::flush;
                for(int i=0; i<4; ++i) { std::this_thread::sleep_for(std::chrono::seconds(1)); std::cout << " ." << std::flush; }
                clear_screen();
                player.gold += 15;
                std::cout << "\n[Бригадир]: Отличная работа, держи свои 15 золотых монеток!\n";
            }
            wait_for_player();
            return;
        }

        std::cout << "\n[Бригадир]: Эй, крепкие руки везде пригодятся! Надо разгрузить корабль с вяленой рыбой. \n[Бригадир]: Платят честно. Или ты просто поболтать пришёл?\n";
        std::cout << "1. [Работа] Таскать тяжелые бочки (Займет 4 секунды | +10 золотых)\n";
        std::cout << "2. Что за слухи ходят о море и рыболовстве в последнее время?\n";
        std::cout << "3. Назад\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        clear_screen();

        if (q == 1) {
            std::cout << "Вы надрываетесь, перетаскивая бочки"; std::cout << std::flush;
            for(int i=0; i<4; ++i) { std::this_thread::sleep_for(std::chrono::seconds(1)); std::cout << " ." << std::flush; }
            clear_screen();
            player.gold += 10;
            std::cout << "\n[Бригадир]: Хорошо поработал, держи заслуженные 10 золотых монет!\n";
        } else if (q == 2) {
            std::cout << "\n[Бригадир]: Плохи дела, парень. \n[Бригадир]: Парни из последней шхуны клянутся богами, что выловили сельдь, у которой чешуя была из настоящей стальной крошки, а зубы — как гвозди! \n[Бригадир]: Она порвала им все сети и искусала матроса за палец. Море чернеет, зараза с Железного острова расползается по воде. \n[Бригадир]: Скоро рыбачить станет смертельно опасно.\n";
        }
        wait_for_player();
    }
    else if (chosen_npc == "Шаман Раду") {
      std::cout << "\n[Раду]: Рад видеть живую душу в Тёмном лесу.\n";
      std::cout << "[Раду]: Что тебе нужно?\n1. Вылечиться полностью (Цена: 80 золотых)\n2. Купить таинственное зелье (+50 HP и +10 к урону) (Цена: 100 золотых)\n3. Уйти\nВыбор: ";
      int q; std::cin >> q; 
      std::cin.ignore(1, '\n');
      clear_screen();
      if (q == 1) {
        if (player.gold >= 80) { 
        player.gold -= 80; 
        if (player.max_health > 200) player.max_health = 200;
        player.health = player.max_health; 
        std::cout << "[Шаман взмахнул посохом] Здоровье восстановлено!\n"; 
        }
        else std::cout << "[Раду]: У тебя не хватает золота.\n";
      }
      else if (q == 2) {
        if (player.gold >= 100) { 
          player.gold -= 100; 
          player.inventory.push_back("Настойка_трав"); 
          std::cout << "Вы купили настойку трав!\n";
        }
        else std::cout << "[Раду]: Золото вперёд.\n";
      }
      wait_for_player();
    }
    else if (chosen_npc == "Дух Лешего") {
      std::cout << "\nИз кустов вырастает огромное существо.\n\n";
      std::cout << "1. Приветствую тебя, хранитель чащи.\n2. [Проигнорировать]\nВыбор: ";
      int q; std::cin >> q; 
      std::cin.ignore(1, '\n');
      clear_screen();
      if (q == 1) {
        std::cout << "\n[Леший]: Ты вежлив, человек. Слушай: Разрушенная деревня сгорела не сама. \n[Леший]: Птицы шепчут, что огонь пришёл с севера от безумных железных существ...\n";
      } 
      else {std::cout << "\n[Леший]: Дерзкий червь! Получай!\n";
        player.health -= 10;for(int i=0; i<10; ++i) {
          std::this_thread::sleep_for(std::chrono::seconds(1)); 
          std::cout << ". " << std::flush; 
        }
        clear_screen();
        std::cout << "\n[Система] Вы очнулись с жуткой головной болью.\n";
      }wait_for_player();
    }
    else if (chosen_npc == "Странник Илай") {
      std::cout << "\n[Илай]: Х-холодно... Я Илай. Застрял тут из-за бури. \n[Илай]: Слыхал, в Снежной деревне укрылся старик, который чудом сбежал с Железного острова. \n[Илай]: Спроси его.\n";
      player.story_flags.push_back("talked_to_stranger");
      wait_for_player();
    }
    else if (chosen_npc == "Однорукий старик Бернард") {
      std::cout << "\n[Бернард]: Да, руки нет... Пришлось отрубить топором, пока живой металл не перешёл на другие части тела. \n[Бернард]: На острове творился кошмар, люди кричали и превращались в статуи. \n[Бернард]: Мне до сих пор снится тот день... Если решишь отправиться туда, то поговори с Мирой.\n";
      wait_for_player();
    }
    else if (chosen_npc == "Алхимик Мира") { 
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
                    player.story_flags.push_back("world_saved");
                    player.gold += 500; 
                    world_map[locationID::cursed_castle].name = "Замок Малакара";
                    world_map[locationID::cursed_castle].description = "Разрушенный замок, в котором раньше обитал Малакар...";

                    std::cout << "[Мира]: Прах Малакара... В нём до сих пор теплится изначальная магия!\n";                    
                    std::cout << " Смешав его с реактивами, Мира создаёт искрящуюся лазурную сыворотку.\n\n";
                    std::cout << "[Мира]: Эту сыворотку нельзя просто вылить в воду. \n[Мира]: Её нужно распылить прямо в сердце Проклятого замка, там, где стояло стальное ядро. \n[Мира]: Иначе, возможно, вирус вернётся.\n";
                    std::cout << "[Мира]: Если я не ошибаюсь, то оставшиеся наросты действуют по своей воле. Идти туда в одиночку будет опасно. \n[Мира]: Ты пойдёшь со мной, " << player.name << "?\n\n";
                    std::cout << "1. Да, Мира. Мы начали это вместе, вместе и закончим. (Отправиться в Замок вместе)\n";
                    std::cout << "2. Нет, я выполнил твою просьбу и убил мага. Дальше действуй сама. (Мира уйдет одна)\n";
                    std::cout << "Выбор: ";
                    int choice_end; std::cin >> choice_end; std::cin.ignore(1, '\n');
                    
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
                }
            } 
            else {
                std::cout << "\n[Мира]: Без образца я не смогу понять структуру заражения. \n[Мира]: Кирку можно взять у кузнеца Грома, а сам металл ищи на скалах Железного острова.\n";
            }
            wait_for_player();
            return;
        }

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
        } else std::cout << "\n[Мира]: Что ж, я не виню тебя.\n";
        wait_for_player();
      }
    else if (chosen_npc == "Король Эдриан") {
        if (player.has_flag("world_saved")) {
            std::cout << "\n[Король Эдриан]: Приветствую тебя, легендарный герой. От лица всего Безымянного королевства я склоняю голову. \n[Король Эдриан]: Ты остановил апокалипсис. Теперь наши стены станут символом новой, безопасной эпохи.\n";
            wait_for_player();
            return;
        }

        std::cout << "\n[Король Эдриан]: В моём королевстве рады честным людям, путник. Тяжкое бремя — править посреди наступающих песков и болезней. \n[Король Эдриан]: Что у тебя на уме?\n\n";
        std::cout << "1. Расскажи историю построения этого государства.\n";
        std::cout << "2. Изгнанники в пустынной деревне говорят, что вы тиран и вышвыриваете бедняков за стены.\n";
        std::cout << "3. Кланяться и уйти\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        clear_screen();

        if (q == 1) {
            std::cout << "\n[Король Эдриан]: Когда пустыня начала поглощать наши старые дома, а с севера поползли слухи о вирусе, я собрал верных людей. \n[Король Эдриан]: Мы возвели эти стены посреди песков за считанные месяцы и построили оазисы защиты. \n[Король Эдриан]: Наше государство не имеет имени, ведь в эти темные времена важны дела, а не названия.\n";
        } else if (q == 2) {
            std::cout << "\n[Король Эдриан]: Законы суровы, путник. Стены требуют ресурсов, стража требует платы, а беженцы прибывают каждый день. \n[Король Эдриан]: Если я перестану собирать налоги — стены падут, и песок поглотит всех нас вместе с вирусом. \n[Король Эдриан]: Изгнание — это меньшая плата за выживание большинства. Я сплю спокойно, зная, что спас тысячи жизней.\n";
        }
        wait_for_player();
    }    
    else if (chosen_npc == "Стражник Томас") {
      std::cout << "\n[Томас]: Снаряжение? Ищи кузнеца-оружейника Грома. Он за углом.\n";
      player.story_flags.push_back("talked_to_guard");
      wait_for_player();
    }
    else if (chosen_npc == "Изгнанник Карим") {
        std::cout << "\n[Карим]: Хех, добро пожаловать на дно мира, путник. Наша деревня — сборище тех, кого королевский закон пережевал и выплюнул. \n[Карим]: Хочешь послушать байки?\n\n";
        std::cout << "1. Как ты оказался среди изгнанников?\n";
        std::cout << "2. Кто такой Джафар и можно ли ему верить?\n";
        std::cout << "3. Уйти\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        clear_screen();

        if (q == 1) {
            std::cout << "\n[Карим]: Мой отец был кузнецом в Безымянном королевстве. Когда налоги выросли втрое, он не смог вовремя сдать норму стали для королевских мечей. \n[Карим]: Стража пришла на следующий день. Они отобрали кузню, а нас вышвырнули за ворота без гроша в кармане. \n[Карим]: Мать не перенесла ночёвки в дюнах... Теперь я здесь. Ненавижу этот сияющий 'рай' Эдриана.\n";
            player.story_flags.push_back("talked_to_citizen"); // Разблокируем Джафара, если квест еще не взят
        } else if (q == 2) {
            std::cout << "\n[Карим]: Джафар? Хитрый лис. Он сколотил банду из таких же озлобленных изгнанников, как мы. \n[Карим]: Верить ему на слово нельзя — предаст и глазом не моргнет. Но если дело касается золота или грабежа королевских караванов — тут он держит слово железно. \n[Карим]: У него свои счеты со стражниками.\n";
        }
        wait_for_player();    
    }
      else if (chosen_npc == "Разбойник Джафар") {
      // Состояние 1: Квест выполнен  
      if (player.has_flag("jafar_quest_finished")) {
            if (player.has_flag("jafar_raid_complete")) {
                std::cout << "\n[Джафар]: Ну ты и лютый, парень! Главарь кочевников мёртв, их банда рассеяна. \n[Джафар]: Ты заслужил моё уважение.\n";
                wait_for_player();
                return;
            }
            if (player.has_flag("jafar_raid_ready")) {
                std::cout << "\n[Джафар]: Чего ты ждёшь? Главарь кочевников где-то в Пустыне. \n[Джафар]: Иди туда и прикончи его, пока они не ушли далеко.\n";
                wait_for_player();
                return;
            }

            std::cout << "\n[Джафар]: Ладно, раз ты доказал верность, есть дельце. \n[Джафар]: В Пустыне бродит банда песчаных кочевников. Отрежь голову их лидеру, заберёшь их золото.\n";
            std::cout << "1. [Квест] Я согласен, я выслежу их в Пустыне.\n2. Уйти\nВыбор: ";
            int q; std::cin >> q; std::cin.ignore(1, '\n');
            if (q == 1) {
                player.story_flags.push_back("jafar_raid_ready"); // Активируем триггер засады
                std::cout << "\n[Джафар]: Вот и отлично. Проваливай в Пустыню и держи ухо востро. Ищи их лидера.\n";
            }
            wait_for_player();
            return;
        }

        // Состояние 2: Квест взят, проверяем выполнение сделки
        if (player.has_flag("jafar_quest_started")) {
            std::cout << "\n[Джафар]: Ты ещё здесь? Я же сказал: принеси мне Стальную кирасу королевского стражника!\n";
            std::cout << "Пока не выполнишь свою часть сделки, нам говорить не о чем. Проваливай.\n\n";
            
            // Ищем, есть ли кираса в инвентаре игрока
            bool has_armor = false;
            size_t armor_index = 0;
            for (size_t i = 0; i < player.inventory.size(); ++i) {
                if (player.inventory[i] == "Стальная_кираса") {
                    has_armor = true;
                    armor_index = i;
                    break;
                }
            }

            if (has_armor) {
                std::cout << "1. [Квест] Отдать Стальную кирасу Джафару\n";
                std::cout << "2. Уйти\nВыбор: ";
                int q; std::cin >> q; std::cin.ignore(1, '\n');
                
                clear_screen();
                if (q == 1) {
                    player.inventory.erase(player.inventory.begin() + armor_index);
                    
                    player.gold += 100;
                    
                    player.story_flags.push_back("jafar_quest_finished");
                    
                    std::cout << "\n[Джафар]: О-о-о, отличная работа! Сталь так и блестит. Настоящая королевская ковка.\n";
                    std::cout << "[Джафар]: Как и обещал — держи сотню золотых монет. А теперь забудь дорогу сюда.\n";
                    std::cout << "[+] Из вашего инвентаря удалена Стальная кираса.\n";
                    std::cout << "[+] Вы получили 100 золотых монет!\n";
                }
            } else {
                std::cout << "1. Уйти и продолжить поиски\nВыбор: ";
                int q; std::cin >> q; std::cin.ignore(1, '\n');
            }
            wait_for_player();
            return;
        }

        // Состояние 3: Первый диалог (Взятие квеста)
        std::cout << "\n[Джафар]: Чего вылупился? Да, я бандит. Ищешь грязную работу или пришёл поглазеть?\n";
        std::cout << "[Джафар]: Королевские караваны часто ходят через пустыню, но из-за охраны соваться туда глупо.\n";
        std::cout << "[Джафар]: Давай так: добудь мне Стальную кирасу королевского стражника. \nКупи её у кузнеца Грома или сними с трупа — мне плевать. \nЯ щедро заплачу тебе 100 золотых монет.\n";
        std::cout << "[Джафар]: Согласен на сделку?\n1. Да, я добуду кирасу.\n2. Нет, я не связываюсь с криминалом.\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        
        clear_screen();
        if (q == 1) {
            player.story_flags.push_back("jafar_quest_started"); // Активируем квест
            std::cout << "\n[Джафар]: Вот и отлично. Иди в Безымянное королевство к оружейнику. \nИ помни: без брони не возвращайся, разговаривать я с тобой не стану.\n";
        } else {
            std::cout << "\n[Джафар]: Ну и катись отсюда, святоша. Будешь подыхать с голоду в песках — не приходи.\n";
        }
        wait_for_player();
    }
    else if (chosen_npc == "Пьяная компания в углу") {
        if (player.has_flag("world_saved")) {
            std::cout << "\n[Завсигдатай]: О-о-о! Наш спаситель! Иди сюда, выпей с нами! \nНам больше не страшны стальные монстры! \n[Завсигдатай]: За твой подвиг!\n";
            wait_for_player();
            return;
        }

        std::cout << "\nИз угла доносится громкий хохот и звон кружек. Заметив вас, один из завсегдатаев икает и кричит:\n\n";
        std::cout << "[Пьяница]: Опа-на, свежая кровь! Эй, путник, плесни-ка ума в наши кружки! \n[Пьяница]: О чём тереть будем?\n\n";
        std::cout << "1. Расскажите легенду про возникновение Железного острова.\n";
        std::cout << "2. [Выпивка] Сыграть с ними в застольную игру на интерес (Выпить на брудершафт)\n";
        std::cout << "3. Игнорировать крики и отойти\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');
        clear_screen();

        if (q == 1) {
            std::cout << "\n[Завсегдатай]: Э-э-э, слушай сюда... Маг Малакар сошел с ума от жажды бессмертия. \n[Завсигдатай]: Его ритуал создал Железный остров, где камни и люди покрыты ржавой живой чешуей! \n[Завсигдатай]: Целый цветущий архипелаг за одну ночь превратился в груду холодного, злого металла. \n[Завсигдатай]: Мира знает больше, обратись к ней.\n";
        } else if (q == 2) {
            std::cout << "\nВы садитесь за грязный дубовый стол. Пьяницы наливают вам полную кружку мутного эля.\n";
            std::cout << "Вы со звоном стукаетесь кружками и выпиваете залпом под одобрительный рёв компании!\n";
            if (player.max_health < 200) {
              player.max_health += 5;
              std::cout << "Вы получили +5 к максимальному здоровью за стойкость и потеряли 5 текущего из-за похмелья!\n";
            } 
            else {
              std::cout << "Вы уже достигли пика физической формы, макс. HP не может быть выше 200!\n";
            }
            player.health -= 5;
            if (player.health <= 0) player.health = 1;        
        }
             wait_for_player();
    }
    else if (chosen_npc == "Кристалл живого металла (Добыча)") {
        if (player.has_flag("mined_metal")) {
            std::cout << "\n[Система] Вы уже откололи образец с этого нароста. Больше здесь взять нечего.\n";
            wait_for_player();
            return;
        }

        std::cout << "\nПеред вами из земли прорастает жуткий темно-стальной кристалл. Металл пульсирует, словно живой.\n";
        std::cout << "1. Попробовать отколоть образец руками\n";
        std::cout << "2. Использовать Кирку из инвентаря\n";
        std::cout << "3. Назад\nВыбор: ";
        int q; std::cin >> q; std::cin.ignore(1, '\n');

        clear_screen();
        if (q == 1) {
            std::cout << "\n[!] Вы хватаетесь за острый металл голыми руками! Живой вирус мгновенно жалит вас.\n";
            player.health -= 30;
            std::cout << "Вы теряете 30 HP, а на кристалле не осталось ни царапины. Нужен подходящий инструмент.\n";
        } 
        else if (q == 2) {
            // Проверяем наличие кирки в инвентаре
            bool has_pickaxe = false;
            for (const auto& item : player.inventory) {
                if (item == "Кирка") { has_pickaxe = true; break; }
            }

            if (has_pickaxe) {
                player.story_flags.push_back("mined_metal");
                player.inventory.push_back("Образец_металла");
                std::cout << "\n[!] Вы со свистом бьете киркой по пульсирующей жиле. Искры летят во все стороны!\n";
                std::cout << "С тяжелым звоном от кристалла откалывается кусок живой стали.\n";
                std::cout << "[+] В ваш инвентарь добавлен Образец металла.\n";
            } else {
                std::cout << "\n[Система] У вас в инвентаре нет кирки! Махать руками бесполезно.\n";
            }
        }
        wait_for_player();
    }
    
    else if (chosen_npc == "Безумный маг Малакар (БОСС)") {
        if (player.has_flag("game_completed")) {
            std::cout << "\n[Система] Проклятый замок пуст. Ядро вируса уничтожено вами навсегда.\n";
            wait_for_player();
            return;
        }

        std::cout << "\nИз багрового тумана Проклятого замка выходит исполинская фигура. Его тело — это сплав плоти, доспехов и шевелящихся стальных лезвий. \nНо когда он заговаривает, его голос звучит на удивление спокойно и чисто.\n\n";
        std::cout << "[Малакар]: Ты проделал долгий путь, дитя.\n";
        std::cout << "[Малакар]: Но не хочу убивать тебя просто так. Я даю тебе выбор. \n\n[Малакар]: Ты можешь развернуться и мирно покинуть этот остров. \n[Малакар]: Либо мы сойдемся в честном бою за судьбу этого мира.\n";
        std::cout << "[Малакар]: Что ты выберешь, путник?\n\n";
        std::cout << "1. [БИТВА] Я пришел положить конец вирусу. Я принимаю бой!\n";
        std::cout << "2. [Уйти] Развернуться и покинуть замок\nВыбор: ";
        if(player.has_flag("mira_quest_completed")){
            std::cout << "3. [БИТВА] Бросить зелье Миры в Малакара\n";
        }
        int q; std::cin >> q; std::cin.ignore(1, '\n');

        clear_screen();
        if (q == 1) {
            std::cout << "\n[Малакар]: Да будет так. Твоя решимость заслуживает уважения.\n\n";
            std::cout << "[!] Малакар поднимает сверкающую стальную руку. Вспышка чистой магии окутывает ваше тело...\n";
            
            // Полное исцеление перед боем
            player.health = player.max_health;
            std::cout << "[+] Ваши раны мгновенно затягиваются! Здоровье полностью восстановлено: " << player.health << " HP!\n\n";
            std::cout << "[Малакар]: А теперь — защищайся!\n";
            wait_for_player();

            Monster boss = {"Малакар, Повелитель Стали", 600, 40, 400};
            if (start_battle(boss)) {
                clear_screen();
                std::cout << "\n================================================================\n";
                std::cout << "                              ПОБЕДА! \n";
                std::cout << " Вы сокрушили Малакара. Перед смертью стальные наросты на его лице\n";
                std::cout << " исчезают, и он слабо улыбается:\n";
                std::cout << " [Малакар]: Ты сражался достойно... Забирай мой посох...\n";
                std::cout << " Исполинское тело мага рассыпается, оставляя после себя лишь горсть\n";
                std::cout << " странного, мерцающего праха и посох.\n";
                std::cout << "================================================================\n";
                
                player.story_flags.push_back("game_completed");
                player.inventory.push_back("Прах_Малакара");
                player.inventory.push_back("Осквернённый посох");
                std::cout << "[+] В ваш инвентарь добавлен Осквернённый посох";
                std::cout << "[+] В ваш инвентарь добавлен Прах Малакара.\n";
                wait_for_player();
            }        
        } 
        else if(q == 2) {
            std::cout << "\n[Малакар]: Мудрое решение. \nЖиви своей смертной жизнью, пока сталь не пришла и за твоим домом. Уходи.\n";
            wait_for_player();
        }
        else if(q == 3 && player.has_flag("mira_quest_completed")) {
            std::cout << "Вы бросаете зелье Миры в Малакара\n";
            std::cout << "[!] Сила Малакара ослабла";
            std::cout << "[Малакар]: Ничтожество!";
            wait_for_player();

            Monster boss = {"Малакар, Повелитель Стали", 400, 30, 100};
            if (start_battle(boss)) {
                clear_screen();
                std::cout << "\n================================================================\n";
                std::cout << "                              ПОБЕДА! \n";
                std::cout << " Вы сокрушили Малакара. Перед смертью стальные наросты на его лице\n";
                std::cout << " исчезают, и он слабо улыбается:\n";
                std::cout << " [Малакар]: Мира прекрасный алхимик... Без неё ты бы не справился...\n";
                std::cout << " Исполинское тело мага рассыпается, оставляя после себя лишь горсть\n";
                std::cout << " странного, мерцающего праха.\n";
                std::cout << "================================================================\n";
                
                player.story_flags.push_back("game_completed");
                player.inventory.push_back("Прах_Малакара");
                std::cout << "[+] В ваш инвентарь добавлен Прах_Малакара.\n";
                wait_for_player();
        }
    }
}
}
#endif // NPC_DATA_HPP
