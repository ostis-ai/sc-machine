set(SOURCES 
	"task.cpp"
)
set(HEADERS 
	"task.hpp"
)

add_library (task SHARED ${SOURCES} ${HEADERS})


include_directories("${SC_KPM_ROOT}/task" ${SC_MEMORY_SRC} ${GLIB2_INCLUDE_DIRS})
add_dependencies(task sc-memory-cpp sc-kpm-common)
target_link_libraries(task sc-memory-cpp sc-kpm-common)
