cmake_minimum_required(VERSION 3.14)

set(BOOST_VERSION 1.71.0)
set(CMAKE_CXX_STANDARD 20)

project("mirror_dpms" VERSION 0.1 LANGUAGES C CXX)

find_package(X11 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost ${BOOST_VERSION} COMPONENTS program_options REQUIRED )

pkg_check_modules(DBUS REQUIRED dbus-1)

add_executable(mirror_dpms src/main.cpp)

target_include_directories(mirror_dpms PRIVATE ${Boost_INCLUDE_DIR} ${DBUS_INCLUDE_DIRS})
target_link_libraries(mirror_dpms X11 Xext ${Boost_LIBRARIES} ${DBUS_LIBRARIES} )

target_compile_features(mirror_dpms PRIVATE cxx_std_20)
target_compile_definitions(mirror_dpms PRIVATE DEBUG)