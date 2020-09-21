set(SC_AGENTS_COMMON_SRC "${SC_KPM_SRC}/sc-agents-common")

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

include_directories(${SC_AGENTS_COMMON_SRC} ${SC_MEMORY_SRC} ${SC_KPM_SRC})

add_library(sc-agents-common SHARED ${SOURCES})
target_link_libraries(sc-agents-common sc-memory)

sc_codegen_ex(sc-agents-common ${SC_AGENTS_COMMON_SRC} "${SC_AGENTS_COMMON_SRC}/generated")
target_compile_definitions(sc-agents-common PRIVATE "-DSC_AGENTS_COMMON_SELF_BUILD")

