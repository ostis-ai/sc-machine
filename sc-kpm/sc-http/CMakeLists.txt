set(SC_HTTP_SRC ${CMAKE_CURRENT_LIST_DIR})

set(SOURCES
    "http_module.cpp"

    "http_api_service.cpp"
)
set(HEADERS
	"http_module.hpp"

	"http_api_service.hpp"
)

include_directories(${SC_HTTP_SRC} ${SC_MEMORY_SRC})

add_library (sc-http SHARED ${SOURCES} ${HEADERS})
add_dependencies(sc-http sc-memory-cpp)
target_link_libraries(sc-http sc-memory-cpp)

sc_codegen(sc-http ${SC_HTTP_SRC})