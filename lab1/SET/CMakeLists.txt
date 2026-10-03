cmake_minimum_required(VERSION 3.16)
project(CantorSet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предметная область: класс Set/Element. Общая для приложения и обоих
# наборов тестов, чтобы не компилировать её трижды.
add_library(cantor_set STATIC
    src/set.cpp
)
target_include_directories(cantor_set PUBLIC include)

# --coverage добавляется только по явному запросу (см. CI): он делает
# бинарник медленнее и больше, в обычной сборке он не нужен.
option(ENABLE_COVERAGE "Build with --coverage instrumentation for gcov" OFF)
if(ENABLE_COVERAGE)
    target_compile_options(cantor_set PUBLIC --coverage -O0)
    target_link_options(cantor_set PUBLIC --coverage)
endif()

# Приложение с консольным меню (всё в src/main.cpp — CLI не вынесен отдельно).
add_executable(set_cli src/main.cpp)
target_link_libraries(set_cli PRIVATE cantor_set)

enable_testing()

# Новый тестовый набор на GoogleTest (unit-test/test_set.cpp): своей main()
# не имеет, её даёт GTest::gtest_main.
#
# Сначала пробуем найти уже установленный GoogleTest в системе (быстрее,
# не требует сети). Если не нашли — FetchContent сам скачает исходники
# с GitHub и соберёт их как часть этого же проекта. Это работает одинаково
# на Linux, Windows/MSVC и macOS, без apt/vcpkg/ручной установки.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )
    # Обязательно для MSVC: иначе тесты и основной проект могут линковаться
    # с разными вариантами рантайма (/MD против /MT) и либо не собраться,
    # либо упасть на старте с ошибкой вроде "LNK2038 mismatch detected".
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(run_gtests unit-test/test.cpp)
target_link_libraries(run_gtests PRIVATE cantor_set GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(run_gtests)