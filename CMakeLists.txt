cmake_minimum_required(VERSION 3.16)
project(BreaqReminder VERSION 1.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Widgets)

qt_standard_project_setup()

qt_add_executable(BreaqReminder WIN32 MACOSX_BUNDLE
    ./resources/res.qrc
    ./src/dialog-editnotes.cpp
    ./src/dialog-editnotes.h
    ./src/dialog-editnotes.ui
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/mainwindow.h
    ./src/mainwindow.ui
)
target_compile_definitions(BreaqReminder PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(BreaqReminder PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Widgets
)

install(TARGETS BreaqReminder
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET BreaqReminder
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
