set(MINECO_SRC "${SC_KPM_ROOT}/mineco")

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.hpp")

include_directories(${MINECO_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (mineco SHARED ${SOURCES} ${HEADERS})
add_dependencies(mineco sc-memory-cpp sc-kpm-common)
target_link_libraries(mineco sc-memory-cpp sc-kpm-common)

sc_codegen(mineco ${MINECO_SRC})
