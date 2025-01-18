cmake_minimum_required(VERSION 3.19)
project(REST_API LANGUAGES CXX)


set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    )

qt_standard_project_setup()

qt_add_executable(REST_API
    WIN32 MACOSX_BUNDLE
    myicon.rc
    main.cpp
    RickAndMorty.cpp
    RickAndMorty.h
    RickAndMorty.ui

    Resources.qrc
    mainwindow.h mainwindow.cpp mainwindow.ui
    WeatherAPI.h WeatherAPI.cpp WeatherAPI.ui





)

target_link_libraries(REST_API
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS REST_API
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET REST_API
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

