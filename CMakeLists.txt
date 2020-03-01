cmake_minimum_required(VERSION 3.12)
project(bulkmt VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(bulkmt ISubscriber.h CoutSubscriber.h FileSubscriber.h CommandPackage.h)
target_sources(bulkmt PRIVATE
        main.cpp
        )

target_link_libraries(bulkmt
        ${CMAKE_THREAD_LIBS_INIT}
        )

install(TARGETS bulkmt RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT tnovits@yandex.ru)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
