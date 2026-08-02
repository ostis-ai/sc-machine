cmake_minimum_required(VERSION 3.10)
project(sc-memory-benchmarks)

# Находим необходимые зависимости
find_package(benchmark REQUIRED)
find_package(ScMemory REQUIRED)

# Собираем все исходники бенчмарков
file(GLOB_RECURSE BENCHMARK_SOURCES
        "*.cpp"
        "units/*/*.hpp"
)

# Создаем единый исполняемый файл для всех бенчмарков
add_executable(sc-memory-benchmarks ${BENCHMARK_SOURCES})

# Подключаем зависимости
target_link_libraries(sc-memory-benchmarks
        PRIVATE
        ScMemory::sc-memory
        benchmark::benchmark
)

# Добавляем тест в CTest
add_test(
        NAME sc-memory-benchmarks
        COMMAND sc-memory-benchmarks
)

# Настройка clang-format если нужно
if(SC_CLANG_FORMAT_CODE)
    target_clangformat_setup(sc-memory-benchmarks)
endif()

# Указываем пути для включения заголовочных файлов
target_include_directories(sc-memory-benchmarks
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${benchmark_INCLUDE_DIRS}
)