find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(FRONTEND_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

add_executable(ai-assistant)

# Make the application a Windows GUI app (no console) on Windows
if(OS_WINDOWS)
  set_target_properties(ai-assistant PROPERTIES WIN32_EXECUTABLE ON OUTPUT_NAME ai-assistant)
endif()

target_sources(ai-assistant PRIVATE ${FRONTEND_SOURCES})

target_link_libraries(ai-assistant PRIVATE Qt6::Widgets core nlohmann_json)

deploy_qt(ai-assistant)

message(STATUS "Executable will be built at: ${CMAKE_CURRENT_BINARY_DIR}")
