cmake_minimum_required(VERSION 3.16)
project(BreaqReminder VERSION 1.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)

message(CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia Widgets)
find_package(spdlog CONFIG REQUIRED)

include(cmake/compiler-flags.cmake)
include(cmake/sanitizers.cmake)
include(cmake/git.cmake)

qt_standard_project_setup()

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/resources/res.win.rc")

qt_add_executable(BreaqReminder WIN32 MACOSX_BUNDLE
    ./resources/res.qrc
    ./src/dialog-editnotes.cpp
    ./src/dialog-editnotes.h
    ./src/dialog-editnotes.ui
    ./src/main.cpp
    ./src/mainwindow.cpp
    ./src/mainwindow.h
    ./src/mainwindow.ui
    ./src/logger.h
    ./src/logger.cpp
    ./src/global_variables.h
    ./src/global_variables.cpp
    cmake/compiler-flags.cmake cmake/sanitizers.cmake
    cmake/git.cmake
    ${app_icon_resource_windows}
)
target_compile_definitions(BreaqReminder PRIVATE
    QT_DEPRECATED_WARNINGS
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
)

target_link_libraries(BreaqReminder PRIVATE
    Qt::Core
    Qt::Gui
    Qt6::Multimedia
    Qt::Widgets
    spdlog::spdlog
)

#install(TARGETS BreaqReminder
#    BUNDLE DESTINATION .
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)

#qt_generate_deploy_app_script(
#    TARGET BreaqReminder
#    FILENAME_VARIABLE deploy_script
#    NO_UNSUPPORTED_PLATFORM_ERROR
#)
#install(SCRIPT ${deploy_script})
