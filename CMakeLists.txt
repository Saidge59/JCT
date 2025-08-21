cmake_minimum_required(VERSION 3.16)

project(JCT VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
    main.cpp
    overlaywidget.cpp
    overlaywidget.h
    binanceclient.cpp
    binanceclient.h
    candlechartwidget.cpp
    candlechartwidget.h
    candledata.h
)

if(WIN32)
    set(ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/appicon.ico")
    if(EXISTS ${ICON_FILE})
        configure_file(${ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/appicon.ico COPYONLY)
        set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/jct.rc")
        file(WRITE ${RESOURCE_FILE} "IDI_ICON1 ICON DISCARDABLE \"appicon.ico\"\n")
        set(PROJECT_SOURCES ${PROJECT_SOURCES} ${RESOURCE_FILE})
    else()
        message(WARNING "Icon file appicon.ico not found in ${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(JCT
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        appicon.ico

        candlechartwidget.h candlechartwidget.cpp
        binanceclient.h binanceclient.cpp
        candledata.h
        candlechartwidget.h candlechartwidget.cpp
        overlaywidget.h overlaywidget.cpp
    )
else()
    if(ANDROID)
        add_library(JCT SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(JCT
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(JCT
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Network
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.JCT)
endif()
set_target_properties(JCT PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS JCT
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(JCT)
endif()
