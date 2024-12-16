include_directories(${ZombieShooter_SOURCE_DIR}/include)
include_directories(/home/linuxbrew/.linuxbrew/opt/sfml)

add_executable(${PROJECT_NAME} main.cpp Player.cpp CursorManager.cpp Bullet.cpp)

link_directories(/home/linuxbrew/.linuxbrew/opt/sfml/lib)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-audio sfml-network)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})