cmake_minimum_required(VERSION 3.16)

project(menu-stop VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        menustop.cpp
        menustop.h
        menustop.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(menu-stop
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        lnk.h lnk.cpp
        verticallabel.h verticallabel.cpp
        hoverbutton.h hoverbutton.cpp
        subdirwindow.h subdirwindow.cpp
        versiondialog.h versiondialog.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET menu-stop APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(menu-stop SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(menu-stop
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(menu-stop PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


# --- GUARANTEED EVERY-BUILD VERSION TARGET ---

# 1. Tell the compiler where to find the generated header
target_include_directories(menu-stop PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# 2. Force a command to run on every compile by matching it to a non-existent output
add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AlwaysRun"
    COMMAND ${CMAKE_COMMAND} 
        -DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR} 
        -DBIN_DIR=${CMAKE_CURRENT_BINARY_DIR} 
        -P ${CMAKE_CURRENT_SOURCE_DIR}/GenerateVersion.cmake
)

# 3. Tie the header generation to the main executable
add_custom_target(generate_version_header DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/AlwaysRun")
add_dependencies(menu-stop generate_version_header)


# -------------------------------------------


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.menu-stop)
endif()
set_target_properties(menu-stop PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS menu-stop
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(menu-stop)
endif()
