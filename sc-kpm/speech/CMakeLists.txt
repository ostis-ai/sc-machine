set(SC_SPEECH_SRC "${SC_KPM_ROOT}/speech")

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")

include_directories(${SC_SPEECH_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})

add_library (speech SHARED ${SOURCES} ${HEADERS})
add_dependencies(speech sc-memory-cpp sc-kpm-common)
target_link_libraries(speech sc-memory-cpp sc-kpm-common)

sc_codegen(speech ${SC_SPEECH_SRC})
