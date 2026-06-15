#include<iostream>
#include"map.hpp"
#include"npc.hpp"

int main(){
    std::string p_choose = go_to_location();
    loc_npc_list(p_choose);
    return 0;
}
