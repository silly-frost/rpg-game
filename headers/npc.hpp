#ifndef NPC_HPP
#define NPC_HPP

#include<thread>
#include<chrono>

#define time_stop std::this_thread::sleep_for(std::chrono::seconds(2))

void loc_npc_list(std::string p_choose); // выводит список npc на локации и даёт выбор пользователю
void dialogue(); // начинает диалог с выбранным npc (используется в loc_npc_list();)

#endif
