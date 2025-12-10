# find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(UI_QML)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Quick)
else()
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
endif()

# qt_policy(QTP0001 NEW)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(UI_QML)
    qt_add_executable(ai-assistant
        main.cpp
    )

    qt_add_qml_module(ai-assistant
        URI Frontend
        VERSION 1.0
        QML_FILES
            qml/Main.qml
    )
    target_link_libraries(ai-assistant PRIVATE Qt6::Quick capture core nlohmann_json)
else()
    set(FRONTEND_SOURCES
        app.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
    )
    add_executable(ai-assistant ${FRONTEND_SOURCES})
    target_link_libraries(ai-assistant PRIVATE Qt6::Widgets capture core nlohmann_json)
endif()

# Make the application a Windows GUI app (no console) on Windows
if(OS_WINDOWS)
  set_target_properties(ai-assistant PROPERTIES WIN32_EXECUTABLE ON OUTPUT_NAME ai-assistant)
endif()

deploy_qt(ai-assistant)

message(STATUS "Executable will be built at: ${CMAKE_CURRENT_BINARY_DIR}")
