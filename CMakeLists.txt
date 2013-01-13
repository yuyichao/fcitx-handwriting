cmake_minimum_required(VERSION 2.6)
project(fcitx-handwriting)

find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)
find_package(Fcitx 4.2.7 REQUIRED)
pkg_check_modules(GTK2 "gtk+-2.0" REQUIRED)
pkg_check_modules(DBUS "dbus-1" REQUIRED)
pkg_check_modules(DBUS_GLIB "dbus-glib-1" REQUIRED)
pkg_check_modules(ZINNIA "zinnia" REQUIRED)

set(BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
set(DATADIR ${CMAKE_INSTALL_PREFIX}/share)

configure_file(config.h.in config.h)

_fcitx_add_uninstall_target()

set(CMAKE_C_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,--as-needed")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-undefined,--as-needed")

add_subdirectory(po)
add_subdirectory(src)
add_subdirectory(theme)
add_subdirectory(im)
add_subdirectory(data)
