set(CMAKE_CXX_STANDARD 17)
include_directories(${ZombieShooter_SOURCE_DIR}/include)
include_directories(${ZombieShooter_SOURCE_DIR}/include/nlohmann)
include_directories(/home/linuxbrew/.linuxbrew/opt/sfml)

add_executable(server server.cpp ServerManager.cpp)

link_directories(/home/linuxbrew/.linuxbrew/opt/sfml/lib)
target_link_libraries(server sfml-graphics sfml-window sfml-system sfml-audio sfml-network)