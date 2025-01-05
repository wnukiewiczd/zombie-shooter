set(CMAKE_CXX_STANDARD 17)
include_directories(${ZombieShooter_SOURCE_DIR}/include)
include_directories(${ZombieShooter_SOURCE_DIR}/include/nlohmann)
include_directories(${ZombieShooter_SOURCE_DIR}/assets/fonts)
include_directories(/home/linuxbrew/.linuxbrew/opt/sfml)

add_executable(${PROJECT_NAME} main.cpp Player.cpp CursorManager.cpp Bullet.cpp ClientSideCommunicationManager.cpp)

link_directories(/home/linuxbrew/.linuxbrew/opt/sfml/lib)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-audio sfml-network)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ZombieShooter>/assets)