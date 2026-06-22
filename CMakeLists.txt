cmake_minimum_required(VERSION 3.12)

project(rpg_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES 
    src/main.cpp
    src/map.cpp
    src/player.cpp
)

set(HEADERS 
    headers/json.hpp
    headers/map.hpp
    headers/npc_data.hpp
    headers/player.hpp
    headers/utils.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

# Специфичные настройки для компилятора Microsoft (Windows)
if(MSVC)
    # Принудительное включение UTF-8 для корректного отображения кириллицы
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    
    # Отключение предупреждения о "небезопасных" функциях (getenv и т.д.)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/map.json"
    "${CMAKE_CURRENT_BINARY_DIR}/map.json"
    COPYONLY
)

