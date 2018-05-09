set(SC_TASK_SRC ${CMAKE_CURRENT_LIST_DIR})

set(SOURCES 
	"task.cpp"
)
set(HEADERS 
	"task.hpp"
)

add_library (sc-task SHARED ${SOURCES} ${HEADERS})


include_directories(${SC_TASK_SRC} ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(sc-task sc-memory-cpp sc-kpm-common)
target_link_libraries(sc-task sc-memory-cpp sc-kpm-common)
