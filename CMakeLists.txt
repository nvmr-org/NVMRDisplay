cmake_minimum_required(VERSION 3.14)

project(NVMRDisplay LANGUAGES CXX C)

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

if( NOT ANDROID )
    option( INTEGRATED_JSON "Integrated JSON library" OFF )
else()
    set(INTEGRATED_JSON ON)
endif()

set(json_sources)
set(avahi_sources)
if( ${INTEGRATED_JSON} )
    set(json_sources nvmr-json/networksettings.cpp
        nvmr-json/streaminformation.h
        nvmr-json/videosendermessage.cpp
        nvmr-json/videosettings.h
        nvmr-json/networksettings.h
        nvmr-json/videosenderconfiguration.cpp
        nvmr-json/videosendermessage.h
        nvmr-json/streaminformation.cpp
        nvmr-json/videosenderconfiguration.h
        nvmr-json/videosettings.cpp)
endif()

set(qt_components Core Quick Multimedia WebSockets)

if( NOT ANDROID )
    set( qt_components ${qt_components} WebEngine)
    set(avahi_sources
        avahibrowse.cpp
        avahibrowse.h)
elseif( ANDROID )
    set(avahi_sources
        avahi-core/addr-util.c
        avahi-core/addr-util.h
        avahi-core/announce.c
        avahi-core/announce.h
        avahi-core/browse.c
        avahi-core/browse-dns-server.c
        avahi-core/browse-domain.c
        avahi-core/browse.h
        avahi-core/browse-service.c
        avahi-core/browse-service-type.c
        avahi-core/cache.c
        avahi-core/cache.h
        avahi-core/core.h
        avahi-core/dns.c
        avahi-core/dns.h
        avahi-core/dns-srv-rr.h
        avahi-core/domain-util.c
        avahi-core/domain-util.h
        avahi-core/entry.c
        avahi-core/fdutil.c
        avahi-core/fdutil.h
        avahi-core/hashmap.c
        avahi-core/hashmap.h
        avahi-core/iface.c
        avahi-core/iface.h
        avahi-core/iface-linux.c
        avahi-core/iface-linux.h
        avahi-core/iface-pfroute.c
        avahi-core/iface-pfroute.h
        avahi-core/internal.h
        avahi-core/log.c
        avahi-core/log.h
        avahi-core/lookup.h
        avahi-core/multicast-lookup.c
        avahi-core/multicast-lookup.h
        avahi-core/netlink.c
        avahi-core/netlink.h
        avahi-core/prioq.c
        avahi-core/prioq.h
        avahi-core/probe-sched.c
        avahi-core/probe-sched.h
        avahi-core/publish.h
        avahi-core/querier.c
        avahi-core/querier.h
        avahi-core/query-sched.c
        avahi-core/query-sched.h
        avahi-core/resolve-address.c
        avahi-core/resolve-host-name.c
        avahi-core/resolve-service.c
        avahi-core/response-sched.c
        avahi-core/response-sched.h
        avahi-core/rr.c
        avahi-core/rr.h
        avahi-core/rrlist.c
        avahi-core/rrlist.h
        avahi-core/rr-util.h
        avahi-core/server.c
        avahi-core/socket.c
        avahi-core/socket.h
        avahi-core/timeeventq.c
        avahi-core/timeeventq.h
        avahi-core/util.c
        avahi-core/util.h
        avahi-core/wide-area.c
        avahi-core/wide-area.h
        avahi-common/address.c
        avahi-common/dbus-watch-glue.c
        avahi-common/gccmacro.h
        avahi-common/rlist.h
        avahi-common/timeval.c
        avahi-common/address.h
        avahi-common/dbus-watch-glue.h
        avahi-common/i18n.c
        avahi-common/simple-watch.c
        avahi-common/timeval.h
        avahi-common/alternative.c
        avahi-common/defs.h
        avahi-common/i18n.h
        avahi-common/simple-watch.h
        avahi-common/alternative.h
        avahi-common/domain.c
        avahi-common/llist.h
        avahi-common/strlst.c
        avahi-common/utf8.c
        avahi-common/domain.h
        avahi-common/Makefile.am
        avahi-common/strlst.h
        avahi-common/utf8.h
        avahi-common/cdecl.h
        avahi-common/malloc.c
        avahi-common/dbus.c
        avahi-common/error.c
        avahi-common/malloc.h
        avahi-common/thread-watch.c
        avahi-common/watch.h
        avahi-common/dbus.h
        avahi-common/error.h
        avahi-common/rlist.c
        avahi-common/thread-watch.h
        avahi-qt/qt-watch.cpp
        avahi-qt/qt-watch.h

        embeddedavahibrowse.h
        embeddedavahibrowse.cpp
        )
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS ${qt_components} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${qt_components} REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        videodiscover.cpp
        videodiscover.h
        rpivideosender.cpp
        rpivideosender.h
        qml.qrc
        ${json_sources}
        ${avahi_sources}
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
  Qt${QT_VERSION_MAJOR}::Multimedia
  Qt${QT_VERSION_MAJOR}::WebSockets)

if( NOT ANDROID )
    target_link_libraries(NVMRDisplay
        PRIVATE Qt${QT_VERSION_MAJOR}::WebEngine)
else()
    target_compile_definitions(NVMRDisplay PRIVATE
        HAVE_NETLINK
        HAVE_STRLCPY)
endif( NOT ANDROID )

if( ${INTEGRATED_JSON} )
#    ExternalProject_Add( nvmr-json-lib
#        GIT_REPOSITORY https://github.com/nvmr-org/nvmr-json.git
#        INSTALL_COMMAND "" )
#    add_subdirectory( ${CMAKE_BINARY_DIR}/nvmr-json-lib-prefix )
##    target_link_directories(NVMRDisplay PRIVATE
##        ${CMAKE_BINARY_DIR}/nvmr-json-install/usr/local/lib )
##    target_link_libraries(NVMRDisplay
##        PRIVATE nvmr-json)
##    target_include_directories( NVMRDisplay PRIVATE
##        ${CMAKE_BINARY_DIR}/nvmr-json-install/usr/local/include )
endif()

