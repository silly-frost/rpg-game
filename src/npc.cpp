#ifndef NPC_CPP
#define NPC_CPP

#include<iostream>
#include<string>
#include"npc.hpp"

static std::string home = "Дом"; // (id = 0)
static std::string desert = "Деревня в пустыне"; // (id = 1)
static std::string unnamed_kingdom = "Безымянное королевство"; // (id = 2)
static std::string forest = "Лес"; // (id = 3) 

void loc_npc_list(std::string p_choose){ // p_choose = выбор игрока
    int choose;
    
    if(p_choose == desert){
        std::cout << "Выберите с кем поговорить, написав номер строки: \n"
                  << "1. Зарак Солнечный, мудрый старец \n"
                  << "2. Фарида Огненная, маг \n"
                  << "3. Тарик, торговец \n"
                  << "4. Джабир Песчанный, охотник на монстров \n\n";
        std::cin >> choose;

         #ifdef _WIN32
        std::system("cls");
    #else
        std::system("clear");
    #endif

        switch(choose){
            case 1:
                std::cout << "Здравствуй, путник. У меня ты можешь спросить что угодно. \n\n";
                break;

            case 2:
                std::cout << "Приветствую тебя, искатель приключений. \n";
                time_stop;
                std::cout << "Благодаря мне ты сможешь познать величайшие тайны магии огня!\n\n";
                break;

            case 3:
                std::cout << "Добро пожаловать в оружейную лавку \"Воткну в хуй булавку\"! \n\n";
                time_stop;
                std::cout << "Выбирай снаряжение, спаси очко от унижения. \n";
                time_stop;
                std::cout << "Приложишь к выбору усилия - избежишь анального насилия. \n\n";
                time_stop;
                std::cout << "В асортименте есть дубовый щит, с ним от удара жопа не трещит. \n";
                time_stop;
                std::cout << "Рядом пики точёные, рубят хуи дрочёные. \n";
                time_stop;
                std::cout << "Тут какой-то дрын торчит, свинопасов крайне огорчит. \n";
                time_stop;
                std::cout << "Ящик с говном метательным, бой будет очень увлекательным. \n";
                time_stop;
                std::cout << "А если не удалось избежать разноса, то вот коврик для удобного отсоса. \n\n";
                break;

            case 4:
                std::cout << "Ты знаешь кто я, поэтому давай не станем тратить время впустую. \n";
                time_stop;
                std::cout << "Говори, чего тебе? \n\n";
        }
    }
    else if(p_choose == unnamed_kingdom){
        std::cout << "Выберите с кем поговорить, написав номер строки: \n"
                  << "1. Артур Непоколебимый, страж \n"
                  << "2. Брант Железный, кузнец \n"
                  << "3. Орион Светлый, священник \n"
                  << "4. Тома Солнечный, горожанин \n\n";
    }
    else if(p_choose == forest){
        std::cout << "Выберите с кем поговорить, написав номер строки: \n"
                  << "1. Громар Древесный, дровосек \n" 
                  << "2. Элиан Травяной, знахарь \n"
                  << "3. Фенрир Лесной, друид \n\n";
    }
}

#endif
