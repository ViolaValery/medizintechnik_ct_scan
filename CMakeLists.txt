cmake_minimum_required(VERSION 3.19)
project(Aufgabe1 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(Aufgabe1
    WIN32 MACOSX_BUNDLE
    main.cpp
    widget.cpp
    widget.h
    widget.ui
)

qt_add_translations(
    TARGETS Aufgabe1
    TS_FILES Aufgabe1_en_150.ts
)

target_link_libraries(Aufgabe1
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Aufgabe1
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Aufgabe1
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
