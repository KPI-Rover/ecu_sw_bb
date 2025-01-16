cmake_minimum_required(VERSION 3.10)

# Название проекта
project(ServerApp)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Вказуємо вихідний файл
set(SOURCES server.cpp)

# Створюємо виконуваний файл
add_executable(server ${SOURCES})

# Лінкування бібліотеки pthread.h
target_link_libraries(server pthread)

