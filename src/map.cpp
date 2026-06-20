#ifndef MAP_CPP
#define MAP_CPP

#include<string>
#include"map.hpp"
#include"utils.hpp"
#include<iostream>

const std::unordered_map<locationID, Location> world_map = {
  {locationID::home, {
      "Дом",
      "Уютное место, где вам всегда рады.",
      {
        {locationID::desert, 3},
        {locationID::flower_valley, 2}
      }
  }},
  {locationID::desert, {
      "Пустыня",
      "Страшная жара и бесконечные пески.",
      {
        {locationID::home, 3},
        {locationID::unnamed_kingdom, 3},
        {locationID::oasis, 6}
      }
  }},
  {locationID::oasis, {
      "Оазис",
      "Помощь Божья для заблудших в бесконечных песках",
      {
        {locationID::desert, 6},
        {locationID::desert_village, 4}
      }
  }},
  {locationID::desert_village, {
      "Пустынная деревня",
      "Здесь проживают сломленные духом люди, потерявшие веру в мир.",
      {
        {locationID::oasis, 4}
      }
  }},
  {locationID::unnamed_kingdom, {
      "Безымянное королевство",
      "Несмотря на расположение в пустыне, король смог создать рай для своих жителей.",
      {
        {locationID::desert, 3},
        {locationID::tavern, 1}
      }
  }},
  {locationID::tavern, {
      "Таверна",
      "Дружественная атмосфера и множество выпивки!",
      {
        {locationID::unnamed_kingdom, 1}
      }
  }},
  {locationID::flower_valley, {
      "Цветочная поляна",
      "Жужжание пчёл и прекрасные виды наполняют вас решимостью.",
      {
        {locationID::home, 2},
        {locationID::oak_forest, 3}
      }
  }},
  {locationID::oak_forest, {
      "Дубовый лес",
      "Вокруг вас шумят вековые деревья.",
      {
        {locationID::flower_valley, 3},
        {locationID::dark_forest, 5}
      }
  }},
  {locationID::dark_forest, {
      "Темный лес",
      "Старый жуткий лес, в котором легко заблудиться.",
      {
        {locationID::oak_forest, 5},
        {locationID::broken_village, 2},
        {locationID::snowlands, 6}
      }
  }},
  {locationID::broken_village, {
      "Разрушенная деревня",
      "Когда-то здесь было шумно и весело, пока не начался пожар...",
      {
        {locationID::dark_forest, 2}
      }
  }},
  {locationID::snowlands, {
      "Снежные земли",
      "Холод пробирает вас до костей.",
      {
        {locationID::dark_forest, 6},
        {locationID::ice_caves, 4},
        {locationID::snow_village, 2}
      }
  }},
  {locationID::snow_village, {
      "Снежная деревня",
      "Горящие здесь огни являются путеводной звездой для заблудших в буре.",
      {
        {locationID::snowlands, 2},
        {locationID::seaport, 1}
      }
  }},
  {locationID::ice_caves, {
      "Ледяные пещеры",
      "Мрачная, но безопасная пещера. Здесь можно спрятаться от бури.",
      {
        {locationID::snowlands, 4}
      }
  }},
  {locationID::seaport, {
      "Заснеженный порт",
      "После обнаружения металлического заражения всем запретили покидать материк.",
      {
        {locationID::snowlands, 1}, // Исправлено логическое замыкание: порт соединен со snow_village, а snow_village со snowlands. Если порт ведет сразу в snowlands, то время 1 сек.
        {locationID::iron_island, 10}
      }
  }},
  {locationID::iron_island, {
      "Железный остров",
      "Некогда прекрасное место, но однажды всё изменилось...",
      {
        {locationID::seaport, 10},
        {locationID::cursed_castle, 2}
      }
  }},
  {locationID::cursed_castle, {
      "Проклятый замок",
      "Здесь началось распространение железного вируса... Никто не выжил...",
      {
        {locationID::iron_island, 2}
      }
  }}
};

void map_time(){
    std::time_t now = std::time(nullptr);

    std::tm local_time;
    localtime_r(&now, &local_time);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << "\n" << std::endl;
}


locationID current_location = locationID::home;

void change_location(){
  clear_screen();
  const Location& current = world_map.at(current_location);
    
    std::cout << "\nВы находитесь в локации \"" << current.name << "\".\n";
    std::cout << current.description << "\n\n";
    std::cout << "Доступные пути:\n";
    
    for (const Connection& conn : current.connected_locations) {
        std::cout << "- " << world_map.at(conn.target).name << "\n";
    }
    
    std::cout << "\nКуда вы хотите пойти? ";
    std::string choice;
    std::getline(std::cin, choice);
    
    bool moved = false;
    for (const Connection& conn : current.connected_locations) {
        const Location& next_loc = world_map.at(conn.target);
        
        if (next_loc.name == choice) {
            clear_screen();
            std::cout << "Вы отправляетесь в локацию \"" << next_loc.name << "\".\n";
            std::cout << "Путь займет " << conn.travel_time << " сек.\n\nВ пути";
            std::cout << std::flush;
            
            for (int i = 0; i < conn.travel_time; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << " ." << std::flush;
            }

            clear_screen();
            std::cout << "\nВы прибыли!\n";
            map_time();

            current_location = conn.target;
            moved = true;
            break;
        }
    }
  
  if(!moved){ 
      clear_screen();
      std::cout << "Название указано неверно." << "\n";
      time_stop(3);
      clear_screen();
  }
}

#endif
