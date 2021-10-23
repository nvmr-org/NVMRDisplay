cmake_minimum_required(VERSION 3.14)

project(NVMRDisplay LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

set(qt_components Core Quick Multimedia)

if( NOT ANDROID )
    set( qt_components ${qt_components} WebEngine)
endif( NOT ANDROID )

find_package(QT NAMES Qt6 Qt5 COMPONENTS ${qt_components} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${qt_components} REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        videodiscover.cpp
        videodiscover.h
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NVMRDisplay
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(NVMRDisplay SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(NVMRDisplay
          ${PROJECT_SOURCES}
        )
    endif()
endif()

target_compile_definitions(NVMRDisplay
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(NVMRDisplay
  PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Quick
  Qt${QT_VERSION_MAJOR}::Multimedia)

if( NOT ANDROID )
    target_link_libraries(NVMRDisplay
        PRIVATE Qt${QT_VERSION_MAJOR}::WebEngine)
else()
    target_link_libraries(NVMRDisplay
      PRIVATE
      ${gstreamer_libs})
endif( NOT ANDROID )
