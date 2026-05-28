#ifndef MAP_CPP
#define MAP_CPP

#include"map.hpp"
#include<iostream>

void map_time(){
    std::time_t now = std::time(nullptr);

    std::tm local_time;
    #ifndef _WIN32
        localtime_s(&local_time, &now); // windows
    #else
        localtime_r(&now, &local_time); // linux/macOS
    #endif

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%H:%M", &local_time);

    std::cout << "Текущее время на локации: " << buffer << std::endl;
}



#endif
